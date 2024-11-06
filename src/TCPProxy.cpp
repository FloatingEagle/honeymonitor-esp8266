#include "../includes/TCPProxy.h"

TCPProxy::TCPProxy(String protocol, int port, IPAddress serverIP) {
	this->protocol = protocol;
	this->port = port;
	this->server = new WiFiServer(port);
	this->serverIP = serverIP;
	setup();
}

void TCPProxy::setup() {
	server->begin();
	Console.log("[" + String(protocol) + "] Initialized");
}

void TCPProxy::loop() {
	for (int i = 0; i < clients.size(); i++)
	{
		if(clients[i]->loop() == 0) {
			Console.log("[" + String(protocol) + "] Client disconnected");
			clients.erase(std::next(clients.begin(), i));
			i--;
		}
	}
	
	if (WiFiClient client = server->available())
	{
		String ip = OnboardWifi.ipToString(client.remoteIP());
		String remote_port = String(client.remotePort());
		String id = ip + ":" + remote_port;

		bool found = false;
		for (int i = 0; i < clients.size(); i++)
		{
			if(clients[i] != NULL && clients[i]->id == id)
				found = true;
		}
		
		if(!found) {
			Console.log("[" + String(protocol) + "] Client connected from " + String(id));
			WiFiClient proxy;
			if(proxy.connect(serverIP, port)) {
				Console.log("[" + String(protocol) + "] Connected to server, proxying...");
				clients.push_back(new TCPProxyClient(client, proxy, id));
				Console.log("[" + String(protocol) + "] Reporting...");
				HoneyMonitorAPI.attempt(ip, remote_port, protocol);
			}
		}
	}
}