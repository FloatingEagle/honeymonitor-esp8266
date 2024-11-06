#include "../includes/OnboardWifi.h"

void OnboardWifiClass::setup(String str_ssid, String str_password) {
	char* ssid = const_cast<char*>(str_ssid.c_str());
	char* password = const_cast<char*>(str_password.c_str());
	int c = 0;
	bool done_full_retry = false;
	int tryCounter = 100;
	delay(10);

	if (strlen(ssid) != 0) {
fullretry:
		Console.log("[WiFi] Connecting to " + String(ssid));
		WiFi.persistent(false);
		WiFi.softAPdisconnect();
		WiFi.disconnect();
		WiFi.mode(WIFI_STA);
		delay(100);

		String wifi_hostname = Config.getValueAsString("wifi_hostname");
		
		if(wifi_hostname == NULL || wifi_hostname == "")
			wifi_hostname = "internal-server";

		WiFi.hostname(wifi_hostname);
		

		WiFi.begin(ssid, password);

		while ( c < tryCounter ) {
			if (WiFi.status() == WL_CONNECTED) {
				break;
			}
			delay(500);
			//Console.log(".");
			c++;
		}


		if (WiFi.status() != WL_CONNECTED && !done_full_retry) {
			done_full_retry = true;
			c = 0;

			Console.log("[WiFi] Failed first connection to WiFi, waitting for 10 seconds...");
			delay(10000);
			if (WiFi.status() != WL_CONNECTED) {
				goto fullretry;
			} else {
				Console.log("[WiFi] WiFi connected while waitting!");
			}
		}
	}

	if (c == tryCounter || strlen(ssid) == 0) {
		Console.log("[WiFi] Wifi connection was unsuccesful. Setup access point.");
		//WiFi.disconnect();
		setupAP();
		this->connected = false;
	} else {
		randomSeed(micros());
		this->ipAddress = ipToString(WiFi.localIP());
		Console.log("[WiFi] Connected! | IP: " + String(this->ipAddress) + " | MAC: " + String(WiFi.macAddress()));
		this->connected = true;
		setClock();
	}
}

String OnboardWifiClass::getMacAddress() {
	return String(WiFi.macAddress());
}

IPAddress OnboardWifiClass::getIPAddressRaw() {
	return WiFi.localIP();
}

void OnboardWifiClass::loop() {

}

bool OnboardWifiClass::isConnected() {
	return this->connected;
}

String OnboardWifiClass::getIPAddress() {
	return this->ipAddress;
}

void OnboardWifiClass::setupAP() {
	WiFi.mode(WIFI_AP_STA);
	//WiFi.disconnect();
	delay(100);
	Console.log("[WiFi] Starting network scan...");
	int n = WiFi.scanNetworks();
	Console.log("[WiFi] Network scan completed.");
	if (n == 0) {
		Console.log("[WiFi] No networks found.");
	} else {
		Serial.print(n);
		Console.log("[WiFi] Networks found:");

		//networks = new String*[n];

		for (int i = 0; i < n; ++i) {
			// Print SSID and RSSI for each network found
			Serial.print(i + 1);
			Serial.print(": ");
			Serial.print(WiFi.SSID(i));
			Serial.print(" (");
			Serial.print(WiFi.RSSI(i));
			Serial.print(")");
			Serial.print((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
			String ssid = String (WiFi.SSID(i));
			ssid += WiFi.encryptionType(i) == ENC_TYPE_NONE ? String("") : String("*");
			//Console.log(*ssid);
			networks.push_back(ssid);

			delay(10);
		}
		Console.log("----------------------------------");
	}
	delay(100);

	Console.log("Mac Address:" + String(WiFi.macAddress()));
	

	IPAddress ip(192, 168, 128, 1);
	IPAddress gateway(192, 168, 128, 1);
	IPAddress subnet(255, 255, 255, 0);
	WiFi.softAPConfig(ip, gateway, subnet);
	String ssid = "HoneyMonitor " + String(WiFi.macAddress());
	const char* ssid_ap = const_cast<char*>(ssid.c_str());

	WiFi.softAP(ssid_ap);
	IPAddress apIP = WiFi.softAPIP();
	Console.log("[WiFi] Ready for connection. | IP: " + String(ipToString(apIP)) + " | SSID: " + String(ssid_ap));

}

String OnboardWifiClass::ipToString(IPAddress ip) {
	String s = "";
	for (int i = 0; i < 4; i++)
		s += i  ? "." + String(ip[i]) : String(ip[i]);
	return s;
}

void OnboardWifiClass::setClock() {
	if(clock_configured)
		return;

  	if(!clock_setup) {
  		configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  		clock_setup = true;
		Console.log("[WiFi] Setting time via NTP...");
 	}

	time_t now = time(nullptr);
	if(now < 8 * 3600 * 2)
		return;

	struct tm time_info;
	gmtime_r(&now, &time_info);
	String time_str = String(asctime(&time_info));
	time_str.trim();
	Console.log("[WiFi] Time updated, current time:" + time_str);
	clock_configured = true;
}

String OnboardWifiClass::getTime() {
	if(clock_configured) {
		time_t now = time(nullptr);
		struct tm timeinfo;
  		gmtime_r(&now, &timeinfo);
  		String time = String(asctime(&timeinfo));
  		time.trim();
  		return time;
	}
	return String(millis());
}

OnboardWifiClass OnboardWifi;