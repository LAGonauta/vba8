﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;
using Microsoft.Phone.Tasks;
using PhoneDirect3DXamlAppInterop.Resources;

namespace PhoneDirect3DXamlAppInterop
{
    public partial class AboutPage : PhoneApplicationPage
    {
        public AboutPage()
        {
            InitializeComponent();

#if GBC
            contactBlock.Text = AppResources.AboutContact2;
            appnameLabel.Text = AppResources.ApplicationTitle2;
            versionLabel.Text = AppResources.AboutVersion2;
            titleLabel.Text = AppResources.ApplicationTitle2;
#endif
        }

        private void contactBlock_Tap_1(object sender, System.Windows.Input.GestureEventArgs e)
        {
            EmailComposeTask emailcomposer = new EmailComposeTask();
#if !GBC
        	emailcomposer.To = AppResources.AboutContact;
#else
            emailcomposer.To = AppResources.AboutContact2;
#endif
            emailcomposer.Subject = "bug report or feature suggestion";
        	emailcomposer.Body = "Insert your bug report or feature request here.";
        	emailcomposer.Show();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new Uri("/LicensePage.xaml", UriKind.Relative));
        }
    }
}