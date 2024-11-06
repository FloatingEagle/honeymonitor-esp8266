#ifndef EXTERNAL_CONFIG_H
#define EXTERNAL_CONFIG_H

#include <string.h>
#include <ArduinoJson.h>
#include "Console.h"
#include "FileSystem.h"

#define HONEYMONITOR_CONFIG "/config.json"

class ConfigClass {
	StaticJsonDocument<800> jsonDoc;
	
	public:
		bool save();
		bool updateProperty(String property, String value, boolean saveValues = false);
		void load();
		String getValueAsString(String name, bool loadbefore = false);
		int getValueAsInt(String name, bool loadbefore = false);
		void reset();
		void restart();
				
	private:    

		bool write_StringEE(int Addr, String input);
		String read_StringEE(int Addr, int length);    
		boolean IsNumeric(String str);
};

extern ConfigClass Config;

#endif
