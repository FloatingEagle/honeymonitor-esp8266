#ifndef _TCPPROXYCLIENT_H_
#define _TCPPROXYCLIENT_H_

#include <ESP8266WiFi.h>
#include "Console.h"

class TCPProxyClient {
		WiFiClient client;
		WiFiClient proxy;
		bool has_messages;
		size_t size;

	public:
		String id;
		TCPProxyClient(WiFiClient client, WiFiClient proxy, String id);
		int loop();
};

#endif