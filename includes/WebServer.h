#ifndef HONEYMONITOR_WEBSERVER_H
#define HONEYMONITOR_WEBSERVER_H

#include <ESPAsyncWebSrv.h>
#include "Config.h"
#include "Console.h"
#include "OnboardWifi.h"
#include "FileSystem.h"

class WebServerClass {
		AsyncWebServer* server;
		String networks_text = "";
		String http_username = "";
		String http_password = "";
		int http_port;
		int opCounter = -1;
		String op = "";

	public:
		void setup();
		void loop();

	private:
		bool requireAuth(AsyncWebServerRequest *request);
		void responseRedirect(AsyncWebServerRequest *request, String path);
		void onNotFound(AsyncWebServerRequest *request);
		void interfaceAction(AsyncWebServerRequest *request);
		void interfaceActionPOST(AsyncWebServerRequest *request);
		void waitAction(AsyncWebServerRequest *request);
		void logsAction(AsyncWebServerRequest *request);
		void resetAction(AsyncWebServerRequest *request);
		void restartAction(AsyncWebServerRequest *request);
		void configure();
};

extern WebServerClass WebServer;

#endif