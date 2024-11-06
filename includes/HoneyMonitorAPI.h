#ifndef HONEYMONITOR__HONEYMONITORAPI_H
#define HONEYMONITOR__HONEYMONITORAPI_H

#include <WString.h>
#include "Console.h"
#include "OnboardWifi.h"
#include "FileSystem.h"

class HoneyMonitorAPIClass {
		WiFiClient client;
		IPAddress server_ip;
		String network_key;
	public:
		void setup(String network_key, IPAddress server_ip);
		bool ping(String if_ip, String if_port);
		bool attempt(String ip, String port, String protocol);
		String request(String param);
};

extern HoneyMonitorAPIClass HoneyMonitorAPI;

#endif