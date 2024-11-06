#include "../includes/TCPProxyClient.h"

TCPProxyClient::TCPProxyClient(WiFiClient client, WiFiClient proxy, String id) {
	this->id = id;
	this->client = client;
	this->proxy = proxy;
	loop();
}

int TCPProxyClient::loop() {
	if (client.connected() && proxy.connected())
	{
  		while((size = client.available()) > 0) {
       		uint8_t* msg = (uint8_t*)malloc(size);
       		size = client.read(msg, size);
			proxy.write(msg, size);
       		free(msg);
    	}
		while((size = proxy.available()) > 0) {
       		uint8_t* msg = (uint8_t*)malloc(size);
       		size = proxy.read(msg,size);
				client.write(msg, size);
   			free(msg);
		}
    	return 1;
    } else {
    	if(client.connected())
    		client.stop();

    	if(proxy.connected())
    		proxy.stop();
    }
    return 0;
}