﻿

#pragma once
//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------

namespace Windows {
    namespace UI {
        namespace Xaml {
            namespace Controls {
                ref class SwapChainBackgroundPanel;
                ref class Button;
            }
        }
    }
}

namespace Win8Snes9x
{
    partial ref class DirectXPage : public ::Windows::UI::Xaml::Controls::Page, 
        public ::Windows::UI::Xaml::Markup::IComponentConnector
    {
    public:
        void InitializeComponent();
        virtual void Connect(int connectionId, ::Platform::Object^ target);
    
    private:
        bool _contentLoaded;
    
        private: ::Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ SwapChainPanel;
        private: ::Windows::UI::Xaml::Controls::Button^ CheatButton;
        private: ::Windows::UI::Xaml::Controls::Button^ LoadStateButton;
        private: ::Windows::UI::Xaml::Controls::Button^ SelectStateButton;
        private: ::Windows::UI::Xaml::Controls::Button^ SaveStateButton;
        private: ::Windows::UI::Xaml::Controls::Button^ StopButton;
        private: ::Windows::UI::Xaml::Controls::Button^ ResetButton;
        private: ::Windows::UI::Xaml::Controls::Button^ PauseButton;
        private: ::Windows::UI::Xaml::Controls::Button^ TurboButton;
    };
}

