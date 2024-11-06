#ifndef HONEYMONITOR__ONBOARDWIFI_H
#define HONEYMONITOR__ONBOARDWIFI_H

#include <ESP8266WiFi.h>
#include <time.h>
#include "Config.h"
#include "Console.h"
#include "OnboardWifi.h"

class OnboardWifiClass {
		String ipAddress = "";
		bool connected = false;
		bool clock_setup = false;
	public:
		bool clock_configured = false;
		std::vector<String> networks;
		
		void setup(String str_ssid, String str_password);
		String getMacAddress();
		IPAddress getIPAddressRaw();
		void loop();
		bool isConnected();
		String getIPAddress();
		void setupAP();
		String ipToString(IPAddress ip);
		void setClock();
		String getTime();
};

extern OnboardWifiClass OnboardWifi;

#endif
