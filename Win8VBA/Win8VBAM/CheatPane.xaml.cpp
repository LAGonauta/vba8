﻿//
// CheatPane.xaml.cpp
// Implementation of the CheatPane class
//

#include "pch.h"
#include "CheatPane.xaml.h"
#include "EmulatorFileHandler.h"
#include "CheatData.h"
#include "stringhelper.h"
#include <string>
#include <sstream>

using namespace std;

using namespace Win8Snes9x;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

#define IS_HEX(a) (\
	(a >= '0' && a <= '9') || (a >= 'a' && a <= 'f') || (a >= 'A' && a <= 'F'))

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

CheatPane::CheatPane()
{
	InitializeComponent();

	create_task([this]()
	{
		return LoadCheats();
	}).then([this](IVector<CheatData ^> ^cheats)
	{
		this->cheatCodes = (Vector<CheatData ^> ^) cheats;

		return this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
		{
			this->RefreshCheatList();
		}));
	}).then([this](task<void> t)
	{
		try
		{
			t.get();
		}catch(Platform::Exception ^ex)
		{
#if _DEBUG
			String ^str = ex->Message;
			wstring wstr (str->Begin(), str->End());

			OutputDebugStringW(wstr.c_str());
#endif
		}
	});
}


void Win8Snes9x::CheatPane::RefreshCheatList(void)
{
	this->cheatList->ItemsSource = nullptr;
	this->cheatList->ItemsSource = this->cheatCodes;
}


void Win8Snes9x::CheatPane::DeleteCheatButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Button ^button = safe_cast<Button ^>(sender);
	CheatData ^data = safe_cast<CheatData ^>(button->DataContext);

	for (int i = 0; i < this->cheatCodes->Size; i++)
	{
		if(this->cheatCodes->GetAt(i) == data)
		{
			this->cheatCodes->RemoveAt(i);
			this->RefreshCheatList();
			break;
		}
	}
}


void Win8Snes9x::CheatPane::backbutton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->Cancelled = true;
	this->Close();
}

void CheatPane::Close()
{
	try
	{
		SaveCheats(this->cheatCodes);
		(safe_cast<Popup ^>(this->Parent))->IsOpen = false;
	}catch(InvalidCastException ^ex)
	{
#if _DEBUG
		Platform::String ^message = ex->Message;
		wstring wstr(message->Begin(), message->End());
		OutputDebugStringW(L"InvalidCastException");
#endif
	}
}


void Win8Snes9x::CheatPane::addButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if(!this->CheckCodeFormat(this->codeBox->Text, 
		[](Platform::String ^message){
			MessageDialog ^dialog = ref new MessageDialog(message);
			dialog->ShowAsync();
		}))
	{
		return;
	}
	Vector<Platform::String ^> ^codes = this->GetCodes(this->codeBox->Text);
	for (int i = 0; i < codes->Size; i++)
	{
		CheatData ^data = ref new CheatData();
		data->CheatCode = codes->GetAt(i);
		data->Description = this->descBox->Text;
		data->Enabled = true;

		this->cheatCodes->Append(data);
	}

	this->RefreshCheatList();
	
	this->codeBox->Text = "";
	this->descBox->Text = "";
}

#ifdef GBC
bool Win8Snes9x::CheatPane::CheckCodeFormat(Platform::String ^codeText, void (*messageCallback)(Platform::String ^))
{
	if(codeText == nullptr || codeText->IsEmpty())
	{
		messageCallback("You must enter a cheat code first.");
		return false;
	}

	vector<string> codeParts;
	string code(codeText->Begin(), codeText->End());

	strreplace(code, '\n', '\r');
	strSplitLines(code, codeParts);

	for (int i = 0; i < codeParts.size(); i++)
	{
		string line = codeParts.at(i);
		StrToUpper(line);
		replaceAll(line, "\t", "");
		replaceAll(line, " ", "");
		replaceAll(line, "-", "");
		for (int i = 0; i < line.length(); i++)
		{
			if(!IS_HEX(line.at(i)))
			{
				messageCallback("Invalid code format.");
				return false;
			}
		}
		if(line.length() != 6 && line.length() != 9 && line.length() != 8)
		{
			messageCallback("Invalid code format.");
            return false;
		}
	}

	return true;
}

Vector<Platform::String ^> ^Win8Snes9x::CheatPane::GetCodes(Platform::String ^codeText)
{
	vector<string> codeParts;
	string code(codeText->Begin(), codeText->End());

	strreplace(code, '\n', '\r');
	strSplitLines(code, codeParts);

	Vector<Platform::String ^> ^codes = ref new Vector<Platform::String ^>();
	for (int i = 0; i < codeParts.size(); i++)
	{
		string line = codeParts.at(i);
		StrToUpper(line);
		replaceAll(line, "\t", "");
		replaceAll(line, " ", "");
		replaceAll(line, "-", "");

		stringstream ss;
		if(line.size() == 6)
		{
			ss << line.substr(0, 3);
			ss << '-';
			ss << line.substr(3, 3);
		}else if(line.size() == 9)
		{
			ss << line.substr(0, 3);
			ss << '-';
			ss << line.substr(3, 3);
			ss << '-';
			ss << line.substr(6, 3);
		}else if(line.size() == 8)
		{
			ss << line;
		}
		string finalCode = ss.str();
		wstring wstr (finalCode.begin(), finalCode.end());
		codes->Append(ref new Platform::String(wstr.c_str()));
	}

	return codes;
}

#else
bool Win8Snes9x::CheatPane::CheckCodeFormat(Platform::String ^codeText, void (*messageCallback)(Platform::String ^))
{
	if(codeText == nullptr || codeText->IsEmpty())
	{
		messageCallback("You must enter a cheat code first.");
		return false;
	}

	vector<string> codeParts;
	string code(codeText->Begin(), codeText->End());

	strreplace(code, '\n', '\r');
	strSplitLines(code, codeParts);

	for (int i = 0; i < codeParts.size(); i++)
	{
		string line = codeParts.at(i);
		StrToUpper(line);
		replaceAll(line, "\t", "");
		replaceAll(line, " ", "");
		for (int i = 0; i < line.length(); i++)
		{
			if(!IS_HEX(line.at(i)))
			{
				messageCallback("Invalid code format.");
				return false;
			}
		}
		if(line.length() != 12 && line.length() != 16)
		{
			messageCallback("Invalid code format.");
            return false;
		}
	}

	return true;
}

Vector<Platform::String ^> ^Win8Snes9x::CheatPane::GetCodes(Platform::String ^codeText)
{
	vector<string> codeParts;
	string code(codeText->Begin(), codeText->End());

	strreplace(code, '\n', '\r');
	strSplitLines(code, codeParts);

	Vector<Platform::String ^> ^codes = ref new Vector<Platform::String ^>();
	for (int i = 0; i < codeParts.size(); i++)
	{
		string line = codeParts.at(i);
		StrToUpper(line);
		replaceAll(line, "\t", "");
		replaceAll(line, " ", "");

		stringstream ss;
		if(line.size() == 12)
		{
			ss << line.substr(0, 8);
			ss << ' ';
			ss << line.substr(8, 4);
		}else if(line.size() == 16)
		{
			ss << line;
		}
		string finalCode = ss.str();
		wstring wstr (finalCode.begin(), finalCode.end());
		codes->Append(ref new Platform::String(wstr.c_str()));
	}

	return codes;
}
#endif



void Win8Snes9x::CheatPane::enableCheatBox_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	CheckBox ^box = safe_cast<CheckBox ^>(sender);
	CheatData ^data = safe_cast<CheatData ^>(box->DataContext);

	data->Enabled = box->IsChecked->Value;
}
