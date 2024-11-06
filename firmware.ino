#include <EEPROM.h>
#include "includes/Config.h"
#include "includes/Console.h"
#include "includes/WebServer.h"
#include "includes/TCPProxy.h"
#include "includes/OnboardWifi.h"
#include "includes/FileSystem.h"

TCPProxy* honeymonitor_protocols[16];
String network_key;
String http_port;
IPAddress server_ip;
bool wifi_connected = false;
int active_protocols = 0;
unsigned long time_target = 0;

void setup() {

	// Init serial
	Serial.begin(115200);

	delay(1000);

	// Print banner
	
	Console.log("");
	Console.log("---------------------------------------------------------------------------------------");
	Console.log("  _    _                        __  __             _ _                                ");
	Console.log(" | |  | |                      |  \\/  |           (_) |                               ");
	Console.log(" | |__| | ___  _ __   ___ _   _| \\  / | ___  _ __  _| |_ ___  _ __ ___ ___  _ __ ___  ");
	Console.log(" |  __  |/ _ \\| '_ \\ / _ \\ | | | |\\/| |/ _ \\| '_ \\| | __/ _ \\| '__/ __/ _ \\| '_ ` _ \\ ");
	Console.log(" | |  | | (_) | | | |  __/ |_| | |  | | (_) | | | | | || (_) | |_| (_| (_) | | | | | |");
	Console.log(" |_|  |_|\\___/|_| |_|\\___|\\__, |_|  |_|\\___/|_| |_|_|\\__\\___/|_(_)\\___\\___/|_| |_| |_|");
	Console.log("                           __/ |                                                      ");
	Console.log("                          |___/                                                       ");
	Console.log("---------------------------------------------------------------------------------------");
	Console.log("");

	Console.log("[FIRMWARE] Initializing device "+String(WiFi.macAddress())+"...");

	// Initialize FS
	FileSystem.setup();
	
	Config.load();


	// Setup WiFi
	OnboardWifi.setup(Config.getValueAsString("wifi_ssid", false), Config.getValueAsString("wifi_password", false));

	if (!OnboardWifi.isConnected())
		Console.log("[WIFI] Cannot connect to the wifi network. Wifi started as AP mode.");
	else {
		wifi_connected = true;
		Console.log("[WIFI] Succesfully connected to the configured network. Wifi started as client mode.");

		// Set up proxies
		network_key = Config.getValueAsString("network_key");
		http_port = Config.getValueAsString("http_port");

		server_ip.fromString(Config.getValueAsString("server_ip", false));

		HoneyMonitorAPI.setup(network_key, server_ip); // Setup reporting API - for attacker identification
		
		if(Config.getValueAsString("ftp") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("FTP", 21, server_ip));
		
		if(Config.getValueAsString("ssh") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("SSH", 22, server_ip));
		
		if(Config.getValueAsString("telnet") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("TELNET", 23, server_ip));
		
		if(Config.getValueAsString("smtp") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("SMTP", 25, server_ip));
		
		if(Config.getValueAsString("http") == "1" && http_port != "80")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("HTTP", 80, server_ip));
		
		if(Config.getValueAsString("pop3") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("POP3", 110, server_ip));
		
		if(Config.getValueAsString("imap") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("IMAP", 143, server_ip));
		
		if(Config.getValueAsString("https") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("HTTPS", 443, server_ip));
		
		if(Config.getValueAsString("smtps") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("SMTPS", 465, server_ip));
		
		if(Config.getValueAsString("imaps") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("IMAPS", 993, server_ip));
		
		if(Config.getValueAsString("pop3s") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("POP3S", 995, server_ip));
		
		if(Config.getValueAsString("socks5") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("Socks5", 1080, server_ip));
		
		if(Config.getValueAsString("mysql") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("MySQL", 3306, server_ip));
		
		if(Config.getValueAsString("rdp") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("RDP", 3389, server_ip));
		
		if(Config.getValueAsString("psql") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("PSQL", 5432, server_ip));
		
		if(Config.getValueAsString("vnc") == "1")
			honeymonitor_protocols[active_protocols++] = (new TCPProxy("VNC", 5900, server_ip));
	}

	// Set up WebServer
	WebServer.setup();

}

void loop() {
	WebServer.loop();
	if (wifi_connected) {
		// Ping every 30 minutes.
		if(OnboardWifi.clock_configured) {
			if(millis() > time_target) {
				time_target = millis() + (1000 * 60 * 30);
				HoneyMonitorAPI.ping(OnboardWifi.getIPAddress(), http_port);
			}
		} else {
			OnboardWifi.setClock();
		}

		for(int i = 0; i < active_protocols; i++)
			honeymonitor_protocols[i]->loop();

	}
}
