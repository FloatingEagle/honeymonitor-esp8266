#include "../includes/WebServer.h"

void WebServerClass::setup() {
	http_port = Config.getValueAsInt("http_port");
	if(http_port < 10000 || http_port > 60000 || !OnboardWifi.isConnected())
		http_port = 80;

	server = new AsyncWebServer(http_port);

	String ssid = Config.getValueAsString("wifi_ssid");
	for (int i = 0; i < OnboardWifi.networks.size(); i++) {
		String net = OnboardWifi.networks[i];
		net.replace("*", "");
		String netName = OnboardWifi.networks[i];
		networks_text += "<option value=\"" + net+ "\"" + (ssid == net ? " selected=\"selected\"" : "") + ">" + netName + "</option>\r\n";
	}

	configure();

	// Set HTTP Auth
	http_username = Config.getValueAsString("http_username");
	http_password = Config.getValueAsString("http_password");

	server->begin();

	Console.log("[Admin Interface] Started on " + String(http_port));
}

void WebServerClass::loop() {
	if(opCounter > 0 && (opCounter--) == 1) {
		if(op == "restart")
			Config.restart();
		else if(op == "reset")
			Config.reset();
		else
			opCounter = 0;
	}
}

bool WebServerClass::requireAuth(AsyncWebServerRequest *request) {
	if(OnboardWifi.isConnected() && http_username != NULL && http_password != NULL && http_username != "" && http_password != "" && !request->authenticate(http_username.c_str(), http_password.c_str())) {
    	request->requestAuthentication();
    	return false;
	}
	return true;
}

void WebServerClass::responseRedirect(AsyncWebServerRequest *request, String path) {
	AsyncWebServerResponse *response = request->beginResponse_P(307, "plain/text", "Redirecting...");
    response->addHeader("Location", path);
	request->send(response);
}

void WebServerClass::onNotFound(AsyncWebServerRequest *request) {
	request->send(404, "text/plain", "Not found");
}

void WebServerClass::interfaceAction(AsyncWebServerRequest *request) {
	if(!requireAuth(request))
		return;

	String content = FileSystem.get("/pages/index.html");
	// Always send the same conent with an updated data in it
	if (OnboardWifi.isConnected())
		content.replace("{networks}", "<input type='text' name='wifi_ssid' id='wifi_ssid' class='form-control' value='" + Config.getValueAsString("wifi_ssid") + "' />");
	else
		content.replace("{networks}", "<select name='wifi_ssid' id='wifi_ssid' class='form-control'>" + networks_text + "</select>");

	content.replace("{mac_address}", String(WiFi.macAddress()));
	content.replace("{network_key}", Config.getValueAsString("network_key"));
	content.replace("{server_ip}", Config.getValueAsString("server_ip"));

	content.replace("{ftp_checked}", Config.getValueAsString("ftp") == "1" ? "checked" : "");
	content.replace("{ssh_checked}", Config.getValueAsString("ssh") == "1" ? "checked" : "");
	content.replace("{telnet_checked}", Config.getValueAsString("telnet") == "1" ? "checked" : "");
	content.replace("{smtp_checked}", Config.getValueAsString("smtp") == "1" ? "checked" : "");
	content.replace("{http_checked}", Config.getValueAsString("http") == "1" ? "checked" : "");
	content.replace("{pop3_checked}", Config.getValueAsString("pop3") == "1" ? "checked" : "");
	content.replace("{imap_checked}", Config.getValueAsString("imap") == "1" ? "checked" : "");
	content.replace("{https_checked}", Config.getValueAsString("https") == "1" ? "checked" : "");
	content.replace("{smtps_checked}", Config.getValueAsString("smtps") == "1" ? "checked" : "");
	content.replace("{imaps_checked}", Config.getValueAsString("imaps") == "1" ? "checked" : "");
	content.replace("{pop3s_checked}", Config.getValueAsString("pop3s") == "1" ? "checked" : "");
	content.replace("{socks5_checked}", Config.getValueAsString("socks5") == "1" ? "checked" : "");
	content.replace("{mysql_checked}", Config.getValueAsString("mysql") == "1" ? "checked" : "");
	content.replace("{rdp_checked}", Config.getValueAsString("rdp") == "1" ? "checked" : "");
	content.replace("{psql_checked}", Config.getValueAsString("psql") == "1" ? "checked" : "");
	content.replace("{vnc_checked}", Config.getValueAsString("vnc") == "1" ? "checked" : "");

	content.replace("{http_port}", Config.getValueAsString("http_port"));
	content.replace("{http_username}", Config.getValueAsString("http_username"));
	content.replace("{http_password}", Config.getValueAsString("http_password"));

	content.replace("{wifi_hostname}", Config.getValueAsString("wifi_hostname"));

	request->send(200, "text/html", content);
}


void WebServerClass::interfaceActionPOST(AsyncWebServerRequest *request) {
	if(!requireAuth(request))
		return;

	if (request->hasParam("action", true)) {
		String action = request->getParam("action", true)->value();

		String network_key = request->hasParam("network_key", true) ? request->getParam("network_key", true)->value() : "";
		String server_ip = request->hasParam("server_ip", true) ? request->getParam("server_ip", true)->value() : "";

		String ftp = request->hasParam("ftp", true) && request->getParam("ftp", true)->value() == "1" ? "1" : "0";
		String ssh = request->hasParam("ssh", true) && request->getParam("ssh", true)->value() == "1" ? "1" : "0";
		String telnet = request->hasParam("telnet", true) && request->getParam("telnet", true)->value() == "1" ? "1" : "0";
		String smtp = request->hasParam("smtp", true) && request->getParam("smtp", true)->value() == "1" ? "1" : "0";
		String http = request->hasParam("http", true) && request->getParam("http", true)->value() == "1" ? "1" : "0";
		String pop3 = request->hasParam("pop3", true) && request->getParam("pop3", true)->value() == "1" ? "1" : "0";
		String imap = request->hasParam("imap", true) && request->getParam("imap", true)->value() == "1" ? "1" : "0";
		String https = request->hasParam("https", true) && request->getParam("https", true)->value() == "1" ? "1" : "0";
		String smtps = request->hasParam("smtps", true) && request->getParam("smtps", true)->value() == "1" ? "1" : "0";
		String imaps = request->hasParam("imaps", true) && request->getParam("imaps", true)->value() == "1" ? "1" : "0";
		String pop3s = request->hasParam("pop3s", true) && request->getParam("pop3s", true)->value() == "1" ? "1" : "0";
		String socks5 = request->hasParam("socks5", true) && request->getParam("socks5", true)->value() == "1" ? "1" : "0";
		String mysql = request->hasParam("mysql", true) && request->getParam("mysql", true)->value() == "1" ? "1" : "0";
		String rdp = request->hasParam("rdp", true) && request->getParam("rdp", true)->value() == "1" ? "1" : "0";
		String psql = request->hasParam("psql", true) && request->getParam("psql", true)->value() == "1" ? "1" : "0";
		String vnc = request->hasParam("vnc", true) && request->getParam("vnc", true)->value() == "1" ? "1" : "0";

		String http_port = request->hasParam("http_port", true) ? request->getParam("http_port", true)->value() : "";
		String http_username = request->hasParam("http_username", true) ? request->getParam("http_username", true)->value() : "";
		String http_password = request->hasParam("http_password", true) ? request->getParam("http_password", true)->value() : "";

		String wifi_hostname = request->hasParam("wifi_hostname", true) ? request->getParam("wifi_hostname", true)->value() : "";
		String wifi_ssid = request->hasParam("wifi_ssid", true) ? request->getParam("wifi_ssid", true)->value() : "";
		String wifi_password = request->hasParam("wifi_password", true) ? request->getParam("wifi_password", true)->value() : "";

		if (action == "updatesettings") {
			Config.load();

			Config.updateProperty("network_key", network_key);
			Config.updateProperty("server_ip", server_ip);

			Config.updateProperty("ftp", ftp);
			Config.updateProperty("ssh", ssh);
			Config.updateProperty("telnet", telnet);
			Config.updateProperty("smtp", smtp);
			Config.updateProperty("http", http);
			Config.updateProperty("pop3", pop3);
			Config.updateProperty("imap", imap);
			Config.updateProperty("https", https);
			Config.updateProperty("smtps", smtps);
			Config.updateProperty("imaps", imaps);
			Config.updateProperty("pop3s", pop3s);
			Config.updateProperty("socks5", socks5);
			Config.updateProperty("mysql", mysql);
			Config.updateProperty("rdp", rdp);
			Config.updateProperty("psql", psql);
			Config.updateProperty("vnc", vnc);

			Config.updateProperty("http_port", http_port);
			Config.updateProperty("http_username", http_username);
			Config.updateProperty("http_password", http_password);

			Config.updateProperty("wifi_hostname", wifi_hostname);

			if(wifi_ssid != "")
				Config. updateProperty("wifi_ssid", wifi_ssid);
			if(wifi_password != "")
				Config.updateProperty("wifi_password", wifi_password);

			Config.save();

			op = "restart";
			responseRedirect(request, "/wait?op=restart");

			return;
		}
	}
	responseRedirect(request, "/");
}

void WebServerClass::logsAction(AsyncWebServerRequest *request) {
	String content = FileSystem.get("/pages/logs.html");
	String logs = "";
	
	for(int i = Console.current_line; i < CONSOLE_HISTORY_SIZE; i++)
		if(Console.history[i] != NULL && Console.history[i] != "")
			logs += Console.history[i] + "\r\n";

	for(int i = 0; i < Console.current_line; i++)
		if(Console.history[i] != NULL && Console.history[i] != "")
			logs += Console.history[i] + "\r\n";

	content.replace("{logs}", logs);
	request->send(200, "text/html", content);
}

void WebServerClass::waitAction(AsyncWebServerRequest *request) {
	request->send(200, "text/html", FileSystem.get("/pages/wait.html"));
	opCounter = 1000; // Allow time to reply to the request
}

void WebServerClass::resetAction(AsyncWebServerRequest *request) {
	if(!requireAuth(request))
		return;

	op = "reset";
	responseRedirect(request, "/wait");
}

void WebServerClass::restartAction(AsyncWebServerRequest *request) {
	if(!requireAuth(request))
		return;

	op = "restart";
	responseRedirect(request, "/wait");
}

void WebServerClass::configure() {
	server->on("/", HTTP_GET, std::bind(&WebServerClass::interfaceAction, this, std::placeholders::_1));
	server->on("/", HTTP_POST, std::bind(&WebServerClass::interfaceActionPOST, this, std::placeholders::_1));
	server->on("/logs", std::bind(&WebServerClass::logsAction, this, std::placeholders::_1));
	server->on("/wait", std::bind(&WebServerClass::waitAction, this, std::placeholders::_1));
	server->on("/reset", std::bind(&WebServerClass::resetAction, this, std::placeholders::_1));
	server->on("/restart", std::bind(&WebServerClass::restartAction, this, std::placeholders::_1));
	server->onNotFound(std::bind(&WebServerClass::onNotFound, this, std::placeholders::_1));
}

WebServerClass WebServer;