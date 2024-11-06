#include "../includes/HoneyMonitorAPI.h"

/*
    A note on encryption:
    Encryption was tried over and over again.
    The small RAM available on the device led to OOM's every single time - it didn't work regardless of the approach.
    In the end: This passes irrelevant data with no security concern and will be left over standard TCP for now.
    If we manage to implement some low overhead solution that allows for encrypted coms we'll update this later.
*/

void HoneyMonitorAPIClass::setup(String network_key, IPAddress server_ip) {
    this->server_ip = server_ip;
    this->network_key = network_key;
    Console.log("[API] Initialized");
}

String HoneyMonitorAPIClass::request(String data) {
    String result = "failed_connect";
    if(client.connect(server_ip, 19889)) {
        client.println(data);

        char msg[128];
        int n = 0;
        size_t size;
        while((size = client.available()) > 0) {
            msg[n++] = client.read();
        }
        
        result = "";
        for (int i = 0; i < n; i++)
            result += msg[i];

        client.stop();
    }
    return result;
}

bool HoneyMonitorAPIClass::ping(String if_ip, String if_port) {
    String result = request(
                        "ping/" + 
                        String(network_key) + "/" + 
                        String(OnboardWifi.getMacAddress()) + "/" + 
                        String(if_ip) + "/" + 
                        String(if_port)
                    );

    Console.log("[API] Ping to HoneyMonitor Insights Server: " + result);
    return result == "200";
}

bool HoneyMonitorAPIClass::attempt(String ip, String port, String protocol) {
    String result = request(
                        "attempt/" + 
                        String(network_key) + "/" + 
                        String(OnboardWifi.getMacAddress()) + "/" + 
                        String(protocol) + "/" + 
                        String(ip) + "/" + 
                        String(port)
                    );

    Console.log("[API] ["+protocol+"] Reporting to HoneyMonitor Insights Server: " + result);
    return result == "200";
}

HoneyMonitorAPIClass HoneyMonitorAPI;