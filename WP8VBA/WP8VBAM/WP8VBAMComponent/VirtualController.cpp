#include "pch.h"
#include "VirtualController.h"
#include "EmulatorSettings.h"
#include <string>
#include <xstring>
#include <sstream>

using namespace std;

using namespace PhoneDirect3DXamlAppComponent;
	
#define VCONTROLLER_Y_OFFSET_WVGA				218
#define VCONTROLLER_Y_OFFSET_WXGA				348
#define VCONTROLLER_Y_OFFSET_720P				308
#define VCONTROLLER_BUTTON_Y_OFFSET_WVGA			303
#define VCONTROLLER_BUTTON_Y_OFFSET_WXGA			498
#define VCONTROLLER_BUTTON_Y_OFFSET_720P			428

namespace Emulator
{
	extern bool gbaROMLoaded;

	VirtualController *VirtualController::singleton = nullptr;

	VirtualController *VirtualController::GetInstance(void)
	{
		return singleton;
	}

	VirtualController::VirtualController(void)
		: virtualControllerOnTop(false), stickFingerDown(false)
	{
		InitializeCriticalSectionEx(&this->cs, 0, 0);
		this->pointers = ref new Platform::Collections::Map<unsigned int, PointerPoint ^>();
		singleton = this;
	}

	VirtualController::~VirtualController(void)
	{
		DeleteCriticalSection(&this->cs);
	}
	
	int VirtualController::GetFormat(void)
	{
		return this->format;
	}
	
	void VirtualController::UpdateFormat(int format)
	{
		this->format = format;
		switch(format)
		{
		case WXGA:
			this->width = 1280;
			this->height = 768;
			this->touchWidth = 800;
			this->touchHeight = 480;
			if(this->orientation != ORIENTATION_PORTRAIT)
			{
				this->CreateWXGARectangles();
			}else
			{
				this->CreateWXGAPortraitRectangles();
			}
			break;
		case HD720P:
			this->width = 1280;
			this->height = 720;
			this->touchWidth = 853;
			this->touchHeight = 480;
			if(this->orientation != ORIENTATION_PORTRAIT)
			{
				this->Create720PRectangles();
			}else
			{
				this->Create720PPortraitRectangles();
			}
			break;
		default:
			this->width = 800;
			this->height = 480;
			this->touchWidth = 800;
			this->touchHeight = 480;
			if(this->orientation != ORIENTATION_PORTRAIT)
			{
				this->CreateWVGARectangles();
			}else
			{
				this->CreateWVGAPortraitRectangles();
			}
			break;
		}
	}

	void VirtualController::CreateWXGARectangles(void)
	{
		int yOffset = 0;
		int buttonYOffset = 0;
		if(virtualControllerOnTop)
		{
			yOffset = VCONTROLLER_Y_OFFSET_WXGA;
			buttonYOffset = VCONTROLLER_BUTTON_Y_OFFSET_WXGA;
		}

		// Visible Rectangles
		this->padCrossRectangle.left = 30;
		this->padCrossRectangle.right = 380;
		this->padCrossRectangle.top = 388;
		this->padCrossRectangle.bottom = 738;

		this->buttonsRectangle.left = 860;
		this->buttonsRectangle.right = 1270;
		this->buttonsRectangle.top = 348;
		this->buttonsRectangle.bottom = 758;

		this->startSelectRectangle.left = 430;
		this->startSelectRectangle.right = 850;
		this->startSelectRectangle.top = 670;
		this->startSelectRectangle.bottom = 748;

		this->lRectangle.left = 0;
		this->lRectangle.right = 170;
		this->lRectangle.top = 75;
		this->lRectangle.bottom = 170;

		this->rRectangle.left = 1110;
		this->rRectangle.right = 1280;
		this->rRectangle.top = 75;
		this->rRectangle.bottom = 170;

		// Scale controller
		float value = 1.0f - EmulatorSettings::Current->ControllerScale / 100.0f;
		this->padCrossRectangle.right -= (LONG)(350.0f * value);
		this->padCrossRectangle.top += (LONG)(350.0f * value);
		this->buttonsRectangle.left += (LONG)(410.0f * value);
		this->buttonsRectangle.top += (LONG)(410.0f * value);
		this->startSelectRectangle.left += (LONG)(125.0f * value);
		this->startSelectRectangle.right -= (LONG)(125.0f * value);
		this->startSelectRectangle.top += (LONG)(46.0f * value);
		this->lRectangle.right -= (LONG)(100.0f * value);
		this->lRectangle.bottom -= (LONG)(56.0f * value);
		this->rRectangle.left += (LONG)(100.0f * value);
		this->rRectangle.bottom -= (LONG)(56.0f * value);		

		this->padCrossRectangle.top -= yOffset;
		this->padCrossRectangle.bottom -= yOffset;

		this->buttonsRectangle.top -= buttonYOffset;
		this->buttonsRectangle.bottom -= buttonYOffset;

		this->lRectangle.top += yOffset + 60;
		this->lRectangle.bottom += yOffset + 60;
		
		this->rRectangle.top += yOffset + 60;
		this->rRectangle.bottom += yOffset + 60;

		this->visibleStickPos.x = (LONG) (this->padCrossRectangle.left + (this->padCrossRectangle.right - this->padCrossRectangle.left) / 2.0f);
		this->visibleStickPos.y = (LONG) (this->padCrossRectangle.top + (this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 2.0f);

		this->visibleStickOffset.x = 0;
		this->visibleStickOffset.y = 0;

		// Touch Rectangles
		this->CreateTouchLandscapeRectangles();
	}

	void VirtualController::CreateWVGARectangles(void)
	{	
		int yOffset = 0;
		int buttonYOffset = 0;
		if(virtualControllerOnTop)
		{
			yOffset = VCONTROLLER_Y_OFFSET_WVGA;
			buttonYOffset = VCONTROLLER_BUTTON_Y_OFFSET_WVGA;
		}

		// Visible Rectangles
		this->padCrossRectangle.left = 19;
		this->padCrossRectangle.right = 238;
		this->padCrossRectangle.top = 242;
		this->padCrossRectangle.bottom = 461;

		this->buttonsRectangle.left = 538;
		this->buttonsRectangle.right = 794;
		this->buttonsRectangle.top = 217;
		this->buttonsRectangle.bottom = 474;

		this->startSelectRectangle.left = 269;
		this->startSelectRectangle.right = 531;
		this->startSelectRectangle.top = 419;
		this->startSelectRectangle.bottom = 468;

		this->lRectangle.left = 0;
		this->lRectangle.right = 106;
		this->lRectangle.top = 47;
		this->lRectangle.bottom = 106;

		this->rRectangle.left = 694;
		this->rRectangle.right = 800;
		this->rRectangle.top = 47;
		this->rRectangle.bottom = 106;

		// Scale controller
		float value = 1.0f - EmulatorSettings::Current->ControllerScale / 100.0f;
		this->padCrossRectangle.right -= (LONG)(219.0f * value);
		this->padCrossRectangle.top += (LONG)(219.0f * value);
		this->buttonsRectangle.left += (LONG)(256.0f * value);
		this->buttonsRectangle.top += (LONG)(256.0f * value);
		this->startSelectRectangle.left += (LONG)(78.0f * value);
		this->startSelectRectangle.right -= (LONG)(78.0f * value);
		this->startSelectRectangle.top += (LONG)(29.0f * value);
		this->lRectangle.right -= (LONG)(61.0f * value);
		this->lRectangle.bottom -= (LONG)(34.0f * value);
		this->rRectangle.left += (LONG)(61.0f * value);
		this->rRectangle.bottom -= (LONG)(34.0f * value);

		this->padCrossRectangle.top -= yOffset;
		this->padCrossRectangle.bottom -= yOffset;

		this->buttonsRectangle.top -= buttonYOffset;
		this->buttonsRectangle.bottom -= buttonYOffset;

		this->lRectangle.top += yOffset + 38;
		this->lRectangle.bottom += yOffset + 38;
		
		this->rRectangle.top += yOffset + 38;
		this->rRectangle.bottom += yOffset + 38;

		this->visibleStickPos.x = (LONG) (this->padCrossRectangle.left + (this->padCrossRectangle.right - this->padCrossRectangle.left) / 2.0f);
		this->visibleStickPos.y = (LONG) (this->padCrossRectangle.top + (this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 2.0f);

		this->visibleStickOffset.x = 0;
		this->visibleStickOffset.y = 0;

		// Touch Rectangles
		this->CreateTouchLandscapeRectangles();
	}

	void VirtualController::Create720PRectangles(void)
	{
		int yOffset = 0;
		int buttonYOffset = 0;
		if(virtualControllerOnTop)
		{
			yOffset = VCONTROLLER_Y_OFFSET_720P;
			buttonYOffset = VCONTROLLER_BUTTON_Y_OFFSET_720P;
		}

		// Visible Rectangles
		this->padCrossRectangle.left = 30;
		this->padCrossRectangle.right = 380;
		this->padCrossRectangle.top = 340;
		this->padCrossRectangle.bottom = 690;

		this->buttonsRectangle.left = 860;
		this->buttonsRectangle.right = 1270;
		this->buttonsRectangle.top = 300;
		this->buttonsRectangle.bottom = 710;

		this->startSelectRectangle.left = 430;
		this->startSelectRectangle.right = 850;
		this->startSelectRectangle.top = 622;
		this->startSelectRectangle.bottom = 700;

		this->lRectangle.left = 0;
		this->lRectangle.right = 170;
		this->lRectangle.top = 27;
		this->lRectangle.bottom = 122;

		this->rRectangle.left = 1110;
		this->rRectangle.right = 1280;
		this->rRectangle.top = 27;
		this->rRectangle.bottom = 122;

		// Scale controller
		float value = 1.0f - EmulatorSettings::Current->ControllerScale / 100.0f;
		this->padCrossRectangle.right -= (LONG)(350.0f * value);
		this->padCrossRectangle.top += (LONG)(350.0f * value);
		this->buttonsRectangle.left += (LONG)(410.0f * value);
		this->buttonsRectangle.top += (LONG)(410.0f * value);
		this->startSelectRectangle.left += (LONG)(125.0f * value);
		this->startSelectRectangle.right -= (LONG)(125.0f * value);
		this->startSelectRectangle.top += (LONG)(46.0f * value);
		this->lRectangle.right -= (LONG)(100.0f * value);
		this->lRectangle.bottom -= (LONG)(56.0f * value);
		this->rRectangle.left += (LONG)(100.0f * value);
		this->rRectangle.bottom -= (LONG)(56.0f * value);

		this->padCrossRectangle.top -= yOffset;
		this->padCrossRectangle.bottom -= yOffset;

		this->buttonsRectangle.top -= buttonYOffset;
		this->buttonsRectangle.bottom -= buttonYOffset;

		this->lRectangle.top += yOffset + 120;
		this->lRectangle.bottom += yOffset + 120;
		
		this->rRectangle.top += yOffset + 120;
		this->rRectangle.bottom += yOffset + 120;

		this->visibleStickPos.x = (LONG) (this->padCrossRectangle.left + (this->padCrossRectangle.right - this->padCrossRectangle.left) / 2.0f);
		this->visibleStickPos.y = (LONG) (this->padCrossRectangle.top + (this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 2.0f);

		this->visibleStickOffset.x = 0;
		this->visibleStickOffset.y = 0;

		// Touch Rectangles
		this->CreateTouchLandscapeRectangles();
	}

	void VirtualController::CreateTouchLandscapeRectangles(void)
	{
		EmulatorSettings ^settings = EmulatorSettings::Current;

		float touchVisualQuotientH = (this->height / (float) this->touchHeight);
		float touchVisualQuotientW = (this->width / (float) this->touchWidth);

		this->lRect.X = lRectangle.left / touchVisualQuotientW;
		this->lRect.Y = (this->height - lRectangle.bottom) / touchVisualQuotientH;
		this->lRect.Width = (lRectangle.right - lRectangle.left) / touchVisualQuotientW;
		this->lRect.Height = (lRectangle.bottom - lRectangle.top) / touchVisualQuotientH;
	
		this->rRect.X = rRectangle.left / touchVisualQuotientW;
		this->rRect.Y = (this->height - rRectangle.bottom) / touchVisualQuotientH;
		this->rRect.Width = (rRectangle.right - rRectangle.left) / touchVisualQuotientW;
		this->rRect.Height = (rRectangle.bottom - rRectangle.top) / touchVisualQuotientH;

		// Cross		
		this->leftRect.X = this->padCrossRectangle.left / touchVisualQuotientW;
		this->leftRect.Y = (this->height - this->padCrossRectangle.bottom) / touchVisualQuotientH;
		this->leftRect.Width = ((this->padCrossRectangle.right - this->padCrossRectangle.left) / 3.0f) / touchVisualQuotientW;
		this->leftRect.Height = (this->padCrossRectangle.bottom - this->padCrossRectangle.top) / touchVisualQuotientH;
				
		this->rightRect.Width = ((this->padCrossRectangle.right - this->padCrossRectangle.left) / 3.0f) / touchVisualQuotientW;
		this->rightRect.X = (this->padCrossRectangle.left / touchVisualQuotientW) + 2.0f * this->rightRect.Width;
		this->rightRect.Y = (this->height - this->padCrossRectangle.bottom) / touchVisualQuotientH;
		this->rightRect.Height = (this->padCrossRectangle.bottom - this->padCrossRectangle.top) / touchVisualQuotientH;
		
		this->upRect.Height = ((this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 3.0f) / touchVisualQuotientH;
		this->upRect.X = (this->padCrossRectangle.left) / touchVisualQuotientW;
		this->upRect.Y = (this->height - this->padCrossRectangle.bottom) / touchVisualQuotientH + 2.0f * this->upRect.Height;
		this->upRect.Width = (this->padCrossRectangle.right - this->padCrossRectangle.left) / touchVisualQuotientW;
		
		this->downRect.Height = ((this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 3.0f) / touchVisualQuotientH;
		this->downRect.X = (this->padCrossRectangle.left) / touchVisualQuotientW;
		this->downRect.Y = (this->height - this->padCrossRectangle.bottom) / touchVisualQuotientH;
		this->downRect.Width = (this->padCrossRectangle.right - this->padCrossRectangle.left) / touchVisualQuotientW;
				
		this->aRect.Width = ((this->buttonsRectangle.right - this->buttonsRectangle.left) / 2.0f) / touchVisualQuotientW;
		this->aRect.X = (this->buttonsRectangle.left / touchVisualQuotientW) + this->aRect.Width;
		this->aRect.Y = (this->height - this->buttonsRectangle.bottom) / touchVisualQuotientH;
		this->aRect.Height = ((this->buttonsRectangle.bottom - this->buttonsRectangle.top) / 2.0f) / touchVisualQuotientH;
			
		this->bRect.Height = ((this->buttonsRectangle.bottom - this->buttonsRectangle.top) / 2.0f) / touchVisualQuotientH;
		this->bRect.X = (this->buttonsRectangle.left) / touchVisualQuotientW;
		this->bRect.Y = (this->height - this->buttonsRectangle.bottom) / touchVisualQuotientH;
		this->bRect.Width = ((this->buttonsRectangle.right - this->buttonsRectangle.left) / 2.0f) / touchVisualQuotientW;
		
		this->selectRect.X = startSelectRectangle.left / touchVisualQuotientW;
		this->selectRect.Y = (this->height - startSelectRectangle.bottom) / touchVisualQuotientH;
		this->selectRect.Width = ((this->startSelectRectangle.right - this->startSelectRectangle.left) / 2.0f) / touchVisualQuotientW;
		this->selectRect.Height = (this->startSelectRectangle.bottom - this->startSelectRectangle.top) / touchVisualQuotientH;
		
		this->startRect.Width = ((this->startSelectRectangle.right - this->startSelectRectangle.left) / 2.0f) / touchVisualQuotientW;
		this->startRect.X = startSelectRectangle.left / touchVisualQuotientW + this->startRect.Width;
		this->startRect.Y = (this->height - startSelectRectangle.bottom) / touchVisualQuotientH;
		this->startRect.Height = (this->startSelectRectangle.bottom - this->startSelectRectangle.top) / touchVisualQuotientH;

		int dpad = settings->DPadStyle;
		if(dpad >= 1)
		{
			this->stickPos.Y = this->leftRect.X + this->leftRect.Width * 1.5f;
			this->stickPos.X = this->leftRect.Y + this->leftRect.Height / 2.0f;

			this->stickOffset.X = 0.0f;
			this->stickOffset.Y = 0.0f;

			if(dpad == 1)
			{
				this->stickBoundaries.Y = this->leftRect.X;
				this->stickBoundaries.X = this->leftRect.Y;
				this->stickBoundaries.Width = this->leftRect.Width * 3;
				this->stickBoundaries.Height = this->leftRect.Height;
			}else
			{
				if(!settings->VirtualControllerOnTop)
				{
					this->stickBoundaries.Y = 0;
					this->stickBoundaries.X = 0;
					this->stickBoundaries.Height = this->selectRect.X;
					this->stickBoundaries.Width = this->lRect.Y;
				}
				else
				{					
					this->stickBoundaries.Y = 0;
					this->stickBoundaries.X = this->lRect.Y + this->lRect.Height;
					this->stickBoundaries.Height = this->selectRect.X;
					this->stickBoundaries.Width = this->touchHeight;
				}
			}
		}
	}

	void VirtualController::CreateWXGAPortraitRectangles(void)
	{
		// Visible Rectangles
		this->padCrossRectangle.left = 10;
		this->padCrossRectangle.right = 310;
		this->padCrossRectangle.top = 700;
		this->padCrossRectangle.bottom = 1000;

		this->buttonsRectangle.left = 400;
		this->buttonsRectangle.right = 790;
		this->buttonsRectangle.top = 670;
		this->buttonsRectangle.bottom = 1060;

		this->startSelectRectangle.left = 174;
		this->startSelectRectangle.right = 594;
		this->startSelectRectangle.top = 1182;
		this->startSelectRectangle.bottom = 1260;

		this->lRectangle.left = 0;
		this->lRectangle.right = 150;
		this->lRectangle.top = 1165;
		this->lRectangle.bottom = 1260;

		this->rRectangle.left = 618;
		this->rRectangle.right = 768;
		this->rRectangle.top = 1165;
		this->rRectangle.bottom = 1260;

		if(EmulatorSettings::Current->AspectRatio == AspectRatioMode::One || !gbaROMLoaded)
		{
			this->padCrossRectangle.top += 80;
			this->padCrossRectangle.bottom += 80;
			
			this->buttonsRectangle.top += 80;
			this->buttonsRectangle.bottom += 80;
		}else if(EmulatorSettings::Current->AspectRatio == AspectRatioMode::Original)
		{
			this->padCrossRectangle.top -= 80;
			this->padCrossRectangle.bottom -= 80;
			
			this->buttonsRectangle.top -= 80;
			this->buttonsRectangle.bottom -= 80;
		}

		// Scale controller
		float value = 1.0f - EmulatorSettings::Current->ControllerScale / 100.0f;
		this->padCrossRectangle.right -= (LONG)(120.0f * value);
		this->padCrossRectangle.left += (LONG)(120.0f * value);
		this->padCrossRectangle.top += (LONG)(120.0f * value);
		this->padCrossRectangle.bottom -= (LONG)(120.0f * value);

		this->buttonsRectangle.left += (LONG)(156.0f * value);
		this->buttonsRectangle.right -= (LONG)(156.0f * value);
		this->buttonsRectangle.top += (LONG)(156.0f * value);
		this->buttonsRectangle.bottom -= (LONG)(156.0f * value);

		this->startSelectRectangle.left += (LONG)(100.0f * value);
		this->startSelectRectangle.right -= (LONG)(100.0f * value);
		this->startSelectRectangle.top += (LONG)(36.8f * value);

		this->lRectangle.right -= (LONG)(70.0f * value);
		this->lRectangle.bottom -= (LONG)(39.0f * value);

		this->rRectangle.left += (LONG)(70.0f * value);
		this->rRectangle.bottom -= (LONG)(39.0f * value);

		this->visibleStickPos.x = (LONG) (this->padCrossRectangle.left + ((this->padCrossRectangle.right - this->padCrossRectangle.left) * 1.2f) / 2.0f);
		this->visibleStickPos.y = (LONG) (this->padCrossRectangle.top + (this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 2.0f);

		this->visibleStickOffset.x = 0;
		this->visibleStickOffset.y = 0;

		// Touch Rectangles
		this->CreateTouchPortraitRectangles();	
	}

	void VirtualController::CreateWVGAPortraitRectangles(void)
	{	
		// Visible Rectangles
		this->padCrossRectangle.left = 6;
		this->padCrossRectangle.right = 194;
		this->padCrossRectangle.top = 438;
		this->padCrossRectangle.bottom = 625;

		this->buttonsRectangle.left = 250;
		this->buttonsRectangle.right = 494;
		this->buttonsRectangle.top = 419;
		this->buttonsRectangle.bottom = 663;

		this->startSelectRectangle.left = 109;
		this->startSelectRectangle.right = 371;
		this->startSelectRectangle.top = 739;
		this->startSelectRectangle.bottom = 788;

		this->lRectangle.left = 0;
		this->lRectangle.right = 94;
		this->lRectangle.top = 728;
		this->lRectangle.bottom = 788;

		this->rRectangle.left = 386;
		this->rRectangle.right = 480;
		this->rRectangle.top = 728;
		this->rRectangle.bottom = 788;	

		if(EmulatorSettings::Current->AspectRatio == AspectRatioMode::One || !gbaROMLoaded)
		{
			this->padCrossRectangle.top += 50;
			this->padCrossRectangle.bottom += 50;
			
			this->buttonsRectangle.top += 50;
			this->buttonsRectangle.bottom += 50;
		}else if(EmulatorSettings::Current->AspectRatio == AspectRatioMode::Original)
		{
			this->padCrossRectangle.top -= 50;
			this->padCrossRectangle.bottom -= 50;
			
			this->buttonsRectangle.top -= 50;
			this->buttonsRectangle.bottom -= 50;
		}
		
		// Scale controller
		float value = 1.0f - EmulatorSettings::Current->ControllerScale / 100.0f;
		this->padCrossRectangle.right -= (LONG)(75.0f * value);
		this->padCrossRectangle.left += (LONG)(75.0f * value);
		this->padCrossRectangle.top += (LONG)(75.0f * value);
		this->padCrossRectangle.bottom -= (LONG)(75.0f * value);

		this->buttonsRectangle.left += (LONG)(97.0f * value);
		this->buttonsRectangle.right -= (LONG)(97.0f * value);
		this->buttonsRectangle.top += (LONG)(97.0f * value);
		this->buttonsRectangle.bottom -= (LONG)(97.0f * value);

		this->startSelectRectangle.left += (LONG)(70.0f * value);
		this->startSelectRectangle.right -= (LONG)(70.0f * value);
		this->startSelectRectangle.top += (LONG)(25.0f * value);

		this->lRectangle.right -= (LONG)(55.0f * value);
		this->lRectangle.bottom -= (LONG)(30.0f * value);

		this->rRectangle.left += (LONG)(55.0f * value);
		this->rRectangle.bottom -= (LONG)(30.0f * value);

		this->visibleStickPos.x = (LONG) (this->padCrossRectangle.left + ((this->padCrossRectangle.right - this->padCrossRectangle.left) * 1.2f) / 2.0f);
		this->visibleStickPos.y = (LONG) (this->padCrossRectangle.top + (this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 2.0f);

		this->visibleStickOffset.x = 0;
		this->visibleStickOffset.y = 0;

		// Touch Rectangles
		this->CreateTouchPortraitRectangles();		
	}

	void VirtualController::Create720PPortraitRectangles(void)
	{
		// Visible Rectangles
		this->padCrossRectangle.left = 20;
		this->padCrossRectangle.right = 310;
		this->padCrossRectangle.top = 700;
		this->padCrossRectangle.bottom = 990;

		this->buttonsRectangle.left = 380;
		this->buttonsRectangle.right = 720;
		this->buttonsRectangle.top = 670;
		this->buttonsRectangle.bottom = 1010;

		this->startSelectRectangle.left = 194;
		this->startSelectRectangle.right = 526;
		this->startSelectRectangle.top = 1198;
		this->startSelectRectangle.bottom = 1260;

		this->lRectangle.left = 0;
		this->lRectangle.right = 120;
		this->lRectangle.top = 1184;
		this->lRectangle.bottom = 1260;

		this->rRectangle.left = 600;
		this->rRectangle.right = 720;
		this->rRectangle.top = 1184;
		this->rRectangle.bottom = 1260;

		if(EmulatorSettings::Current->AspectRatio == AspectRatioMode::One || !gbaROMLoaded)
		{
			this->padCrossRectangle.top += 40;
			this->padCrossRectangle.bottom += 40;
			
			this->buttonsRectangle.top += 40;
			this->buttonsRectangle.bottom += 40;
		}else if(EmulatorSettings::Current->AspectRatio == AspectRatioMode::Original)
		{
			this->padCrossRectangle.top -= 80;
			this->padCrossRectangle.bottom -= 80;
			
			this->buttonsRectangle.top -= 80;
			this->buttonsRectangle.bottom -= 80;
		}

		// Scale controller
		float value = 1.0f - EmulatorSettings::Current->ControllerScale / 100.0f;
		this->padCrossRectangle.right -= (LONG)(100.0f * value);
		this->padCrossRectangle.left += (LONG)(100.0f * value);
		this->padCrossRectangle.top += (LONG)(100.0f * value);
		this->padCrossRectangle.bottom -= (LONG)(100.0f * value);

		this->buttonsRectangle.left += (LONG)(125.0f * value);
		this->buttonsRectangle.right -= (LONG)(125.0f * value);
		this->buttonsRectangle.top += (LONG)(125.0f * value);
		this->buttonsRectangle.bottom -= (LONG)(125.0f * value);

		this->startSelectRectangle.left += (LONG)(100.0f * value);
		this->startSelectRectangle.right -= (LONG)(100.0f * value);
		this->startSelectRectangle.top += (LONG)(36.8f * value);

		this->lRectangle.right -= (LONG)(60.0f * value);
		this->lRectangle.bottom -= (LONG)(32.0f * value);

		this->rRectangle.left += (LONG)(60.0f * value);
		this->rRectangle.bottom -= (LONG)(32.0f * value);

		this->visibleStickPos.x = (LONG) (this->padCrossRectangle.left + ((this->padCrossRectangle.right - this->padCrossRectangle.left) * 1.2f) / 2.0f);
		this->visibleStickPos.y = (LONG) (this->padCrossRectangle.top + (this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 2.0f);

		this->visibleStickOffset.x = 0;
		this->visibleStickOffset.y = 0;

		// Touch Rectangles
		this->CreateTouchPortraitRectangles();	
	}
	
	void VirtualController::CreateTouchPortraitRectangles(void)
	{
		EmulatorSettings ^settings = EmulatorSettings::Current;

		this->lRect.X = (this->lRectangle.top / (float)this->width) * this->touchWidth;
		this->lRect.Y = (this->lRectangle.left / (float) this->height) * this->touchHeight;
		this->lRect.Width = ((this->lRectangle.bottom - this->lRectangle.top) / (float)this->width) * this->touchWidth;
		this->lRect.Height = ((this->lRectangle.right - this->lRectangle.left) / (float)this->height) * this->touchHeight;
	
		this->rRect.X = (this->rRectangle.top / (float)this->width) * this->touchWidth;
		this->rRect.Y = (this->rRectangle.left / (float) this->height) * this->touchHeight;
		this->rRect.Width = ((this->rRectangle.bottom - this->rRectangle.top) / (float)this->width) * this->touchWidth;
		this->rRect.Height = ((this->rRectangle.right - this->rRectangle.left) / (float)this->height) * this->touchHeight;

		// Cross		
		this->leftRect.X = (this->padCrossRectangle.top / (float)this->width) * this->touchWidth;
		this->leftRect.Y = (this->padCrossRectangle.left / (float) this->height) * this->touchHeight;
		this->leftRect.Width = ((this->padCrossRectangle.bottom - this->padCrossRectangle.top) / (float)this->width) * this->touchWidth;
		this->leftRect.Height = (((this->padCrossRectangle.right - this->padCrossRectangle.left) / 3.0f) / (float)this->height) * this->touchHeight;
		
		this->rightRect.Height = (((this->padCrossRectangle.right - this->padCrossRectangle.left) / 3.0f) / (float)this->height) * this->touchHeight;
		this->rightRect.X = (this->padCrossRectangle.top / (float)this->width) * this->touchWidth;
		this->rightRect.Y = ((this->padCrossRectangle.left / (float) this->height) * this->touchHeight) + 2.0f * this->rightRect.Height;
		this->rightRect.Width = ((this->padCrossRectangle.bottom - this->padCrossRectangle.top) / (float)this->width) * this->touchWidth;

		this->upRect.X = (this->padCrossRectangle.top / (float)this->width) * this->touchWidth;
		this->upRect.Y = (this->padCrossRectangle.left / (float) this->height) * this->touchHeight;
		this->upRect.Width = (((this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 3.0f) / (float)this->width) * this->touchWidth;
		this->upRect.Height = ((this->padCrossRectangle.right - this->padCrossRectangle.left) / (float)this->height) * this->touchHeight;
		
		this->downRect.Width = (((this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 3.0f) / (float)this->width) * this->touchWidth;
		this->downRect.X = ((this->padCrossRectangle.top / (float)this->width) * this->touchWidth) + 2.0f * this->downRect.Width;
		this->downRect.Y = (this->padCrossRectangle.left / (float) this->height) * this->touchHeight;
		this->downRect.Height = ((this->padCrossRectangle.right - this->padCrossRectangle.left) / (float)this->height) * this->touchHeight;

		// Buttons
	
		this->aRect.Height = (((this->buttonsRectangle.right - this->buttonsRectangle.left) / 2.0f) / (float)this->height) * this->touchHeight;
		this->aRect.X = (this->buttonsRectangle.top / (float)this->width) * this->touchWidth;
		this->aRect.Y = ((this->buttonsRectangle.left / (float) this->height) * this->touchHeight) + this->aRect.Height;
		this->aRect.Width = (((this->buttonsRectangle.bottom - this->buttonsRectangle.top) / 2.0f) / (float)this->width) * this->touchWidth;

		this->bRect.Height = (((this->buttonsRectangle.right - this->buttonsRectangle.left) / 2.0f) / (float)this->height) * this->touchHeight;
		this->bRect.X = ((this->buttonsRectangle.top / (float)this->width) * this->touchWidth) + this->bRect.Height;
		this->bRect.Y = ((this->buttonsRectangle.left / (float) this->height) * this->touchHeight);
		this->bRect.Width = (((this->buttonsRectangle.bottom - this->buttonsRectangle.top) / 2.0f) / (float)this->width) * this->touchWidth;
	
		/*this->bRect.Width = (((this->buttonsRectangle.bottom - this->buttonsRectangle.top) / 2.0f) / (float)this->width) * this->touchWidth;
		this->bRect.X = ((this->buttonsRectangle.top / (float)this->width) * this->touchWidth) + 2.0f * this->bRect.Width;
		this->bRect.Y = (this->buttonsRectangle.left / (float) this->height) * this->touchHeight;
		this->bRect.Height = ((this->buttonsRectangle.right - this->buttonsRectangle.left) / (float)this->height) * this->touchHeight;*/
		
		this->selectRect.X = (this->startSelectRectangle.top / (float)this->width) * this->touchWidth;
		this->selectRect.Y = (this->startSelectRectangle.left / (float) this->height) * this->touchHeight;
		this->selectRect.Width = ((this->startSelectRectangle.bottom - this->startSelectRectangle.top) / (float)this->width) * this->touchWidth;
		this->selectRect.Height = (((this->startSelectRectangle.right - this->startSelectRectangle.left) / 2.0f) / (float)this->height) * this->touchHeight;
		
		this->startRect.Height = (((this->startSelectRectangle.right - this->startSelectRectangle.left) / 2.0f) / (float)this->height) * this->touchHeight;
		this->startRect.X = (this->startSelectRectangle.top / (float)this->width) * this->touchWidth;
		this->startRect.Y = (this->startSelectRectangle.left / (float) this->height) * this->touchHeight + this->startRect.Height;
		this->startRect.Width = ((this->startSelectRectangle.bottom - this->startSelectRectangle.top) / (float)this->width) * this->touchWidth;

		int dpad = EmulatorSettings::Current->DPadStyle;
		if(dpad >= 1)
		{
			this->stickPos.X = this->leftRect.Y + this->leftRect.Height * 1.8f;
			this->stickPos.Y = this->leftRect.X + this->leftRect.Width / 2.0f;

			this->stickOffset.X = 0.0f;
			this->stickOffset.Y = 0.0f;
			if(dpad == 1)
			{
				this->stickBoundaries.Y = this->leftRect.X;
				this->stickBoundaries.X = this->leftRect.Y;
				this->stickBoundaries.Width = this->leftRect.Width;
				this->stickBoundaries.Height = this->leftRect.Height * 3;
			}else
			{
				this->stickBoundaries.Y = this->leftRect.X;
				this->stickBoundaries.X = 0;
				this->stickBoundaries.Height = abs(this->stickBoundaries.Y - this->lRect.X);
				this->stickBoundaries.Width = this->bRect.Y;
			}
		}
	}

	void VirtualController::VirtualControllerOnTop(bool onTop)
	{
		this->virtualControllerOnTop = onTop;
		this->UpdateFormat(this->format);
	}

	void VirtualController::SetOrientation(int orientation)
	{
		this->orientation = orientation;
		this->UpdateFormat(this->format);
	}

	void VirtualController::PointerPressed(PointerPoint ^point)
	{
		EnterCriticalSection(&this->cs);
		this->pointers->Insert(point->PointerId, point);
		LeaveCriticalSection(&this->cs);

		int dpad = EmulatorSettings::Current->DPadStyle;
		if(dpad >= 1)
		{
			Windows::Foundation::Point p = point->Position;
			if(this->orientation == ORIENTATION_LANDSCAPE_RIGHT)
			{
				p.X = this->touchHeight - p.X;
				p.Y = this->touchWidth - p.Y;
			}

			if(this->stickBoundaries.Contains(p) && !stickFingerDown)
			{
				float scale = (int) Windows::Graphics::Display::DisplayProperties::ResolutionScale / 100.0f;
				if(dpad == 2)
				{
					stickPos = p;
				}
				if(dpad == 2)
				{
					if(this->orientation != ORIENTATION_PORTRAIT)
					{
						this->visibleStickPos.x = this->stickPos.Y * scale;
						this->visibleStickPos.y = this->height - this->stickPos.X * scale;
					}else
					{
						this->visibleStickPos.x = this->stickPos.X * scale;
						this->visibleStickPos.y = this->stickPos.Y * scale;
					}
				}

				stickFingerID = point->PointerId;
				stickFingerDown = true;

				stickOffset.X = p.X - this->stickPos.X;
				stickOffset.Y = p.Y - this->stickPos.Y;

				this->visibleStickOffset.x = this->stickOffset.X * scale;
				this->visibleStickOffset.y = this->stickOffset.Y * scale;
			}
		}
	}

	void VirtualController::PointerMoved(PointerPoint ^point)
	{
		EnterCriticalSection(&this->cs);
		if(this->pointers->HasKey(point->PointerId))
		{
			this->pointers->Insert(point->PointerId, point);
		}
		LeaveCriticalSection(&this->cs);

		int dpad = EmulatorSettings::Current->DPadStyle;
		if(dpad >= 1)
		{
			if(this->stickFingerDown && point->PointerId == this->stickFingerID)
			{
				Windows::Foundation::Point p = point->Position;
				if(this->orientation == ORIENTATION_LANDSCAPE_RIGHT)
				{
					p.X = this->touchHeight - p.X;
					p.Y = this->touchWidth - p.Y;
				}
				float scale = (int) Windows::Graphics::Display::DisplayProperties::ResolutionScale / 100.0f;

				stickOffset.X = p.X - this->stickPos.X;
				stickOffset.Y = p.Y - this->stickPos.Y;

				this->visibleStickOffset.x = this->stickOffset.X * scale;
				this->visibleStickOffset.y = this->stickOffset.Y * scale;
			}
		}
	}

	void VirtualController::PointerReleased(PointerPoint ^point)
	{
		EnterCriticalSection(&this->cs);
		if(this->pointers->HasKey(point->PointerId))
		{
			this->pointers->Remove(point->PointerId);
		}
		LeaveCriticalSection(&this->cs);

		int dpad = EmulatorSettings::Current->DPadStyle;
		if(dpad >= 1)
		{
			if(this->stickFingerDown && point->PointerId == this->stickFingerID)
			{
				this->stickFingerDown = false;
				this->stickFingerID = 0;

				this->stickOffset.X = 0;
				this->stickOffset.Y = 0;

				this->visibleStickOffset.x = 0;
				this->visibleStickOffset.y = 0;
			}
		}
	}
	
	const ControllerState *VirtualController::GetControllerState(void)
	{
		ZeroMemory(&this->state, sizeof(ControllerState));

		int dpad = EmulatorSettings::Current->DPadStyle;

		EnterCriticalSection(&this->cs);
		for (auto i = this->pointers->First(); i->HasCurrent; i->MoveNext())
		{
			PointerPoint ^p = i->Current->Value;
			Windows::Foundation::Point point = Windows::Foundation::Point(p->Position.Y, p->Position.X);
			if(this->orientation == ORIENTATION_LANDSCAPE_RIGHT)
			{
				point.X = this->touchWidth - point.X;
				point.Y = this->touchHeight - point.Y;
			}

			if(dpad == 0)
			{
				if(this->leftRect.Contains(point))
				{
					state.LeftPressed = true;
				}
				if(this->upRect.Contains(point))
				{
					state.UpPressed = true;
				}
				if(this->rightRect.Contains(point))
				{
					state.RightPressed = true;
				}
				if(this->downRect.Contains(point))
				{
					state.DownPressed = true;
				}
			}else
			{
				if(this->stickFingerDown && p->PointerId == this->stickFingerID)
				{
					float deadzone = EmulatorSettings::Current->Deadzone;
					float controllerScale = EmulatorSettings::Current->ControllerScale / 100.0f;
					float length = (float) sqrt(this->stickOffset.X * this->stickOffset.X + this->stickOffset.Y * this->stickOffset.Y);					
					float scale = (int) Windows::Graphics::Display::DisplayProperties::ResolutionScale / 100.0f;
					if(length >= deadzone * scale * controllerScale)
					{
						// Deadzone of 15
						float unitX = 1.0f;
						float unitY = 0.0f;
						float normX = this->stickOffset.X / length;
						float normY = this->stickOffset.Y / length;

						float dot = unitX * normX + unitY * normY;
						float rad = (float) acos(dot);

						if(normY > 0.0f)
						{
							rad = 6.28f - rad;
						}

						if(this->orientation != ORIENTATION_PORTRAIT)
						{
							rad = (rad + 3.14f / 2.0f);
							if(rad > 6.28f)
							{
								rad -= 6.28f;
							}
						}

						if((rad >= 0 && rad < 1.046f) || (rad > 5.234f && rad < 6.28f))
						{
							state.RightPressed = true;
						}
						if(rad >= 0.523f && rad < 2.626f)
						{
							state.UpPressed = true;
						}
						if(rad >= 2.093f && rad < 4.186f)
						{
							state.LeftPressed = true;
						}
						if(rad >= 3.663f && rad < 5.756f)
						{
							state.DownPressed = true;
						}
					}
				}
			}
			if(this->startRect.Contains(point))
			{
				state.StartPressed = true;
			}
			if(this->selectRect.Contains(point))
			{
				state.SelectPressed = true;
			}
			if(this->lRect.Contains(point))
			{
				state.LPressed = true;
			}
			if(this->rRect.Contains(point))
			{
				state.RPressed = true;
			}
			if(this->aRect.Contains(point))
			{
				state.APressed = true;
			}
			if(this->bRect.Contains(point))
			{
				state.BPressed = true;
			}
		}
		LeaveCriticalSection(&this->cs);

		return &this->state;
	}

	void VirtualController::GetCrossRectangle(RECT *rect)
	{
		*rect = this->padCrossRectangle;
	}

	void VirtualController::GetButtonsRectangle(RECT *rect)
	{
		*rect = this->buttonsRectangle;
	}

	void VirtualController::GetStartSelectRectangle(RECT *rect)
	{
		*rect = this->startSelectRectangle;
	}

	void VirtualController::GetLRectangle(RECT *rect)
	{
		*rect = this->lRectangle;
	}

	void VirtualController::GetRRectangle(RECT *rect)
	{
		*rect = this->rRectangle;
	}
	
	void VirtualController::GetStickRectangle(RECT *rect)
	{
		int quarterWidth = (this->padCrossRectangle.right - this->padCrossRectangle.left) / 4;
		int quarterHeight = (this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 4;

		if(this->orientation != ORIENTATION_PORTRAIT)
		{
			rect->left = (this->visibleStickPos.x + this->visibleStickOffset.y) - quarterWidth;
			rect->right = rect->left + 2 * quarterWidth;
			rect->top = (this->visibleStickPos.y - this->visibleStickOffset.x) - quarterHeight;
			rect->bottom = rect->top + 2 * quarterHeight;
		}else
		{

			rect->left = (this->visibleStickPos.x + this->visibleStickOffset.x) - quarterWidth;
			rect->right = rect->left + 2 * quarterWidth;
			rect->top = (this->visibleStickPos.y + this->visibleStickOffset.y) - quarterHeight;
			rect->bottom = rect->top + 2 * quarterHeight;
		}
	}
	
	void VirtualController::GetStickCenterRectangle(RECT *rect)
	{
		int quarterWidth = (this->padCrossRectangle.right - this->padCrossRectangle.left) / 16;
		int quarterHeight = (this->padCrossRectangle.bottom - this->padCrossRectangle.top) / 16;

		if(this->orientation != ORIENTATION_PORTRAIT)
		{
			rect->left = this->visibleStickPos.x - quarterWidth;
			rect->right = rect->left + 2 * quarterWidth;
			rect->top = this->visibleStickPos.y - quarterHeight;
			rect->bottom = rect->top + 2 * quarterHeight;
		}else
		{

			rect->left = this->visibleStickPos.x - quarterWidth;
			rect->right = rect->left + 2 * quarterWidth;
			rect->top = this->visibleStickPos.y - quarterHeight;
			rect->bottom = rect->top + 2 * quarterHeight;
		}
	}

	bool VirtualController::StickFingerDown(void)
	{
		return this->stickFingerDown;
	}
}