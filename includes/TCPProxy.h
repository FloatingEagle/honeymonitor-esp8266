#ifndef HONEYMONITOR__TCPPROXY_H
#define HONEYMONITOR__TCPPROXY_H

#include <ESP8266WiFi.h>
#include "Config.h"
#include "TCPProxyClient.h"
#include "Console.h"
#include "HoneyMonitorAPI.h"
#include "OnboardWifi.h"

class TCPProxy {
		WiFiServer* server;
		String protocol;
		IPAddress serverIP;
		std::vector<TCPProxyClient*> clients;
		int port;

	public:
		TCPProxy(String protocol, int port, IPAddress serverIP);
		// No need for a destructor as TCPProxy should always be running once created.
		// No danger of memory leaks as the memory should always be allocated and in use.

	    String ipToString(IPAddress ip);
		void setup();
		void loop();
};

#endif
