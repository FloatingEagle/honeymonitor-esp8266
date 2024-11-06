#include "../includes/Config.h"

bool ConfigClass::save(){

	String data = "";
	serializeJson(jsonDoc, data);
						
	//Console.log("Data to save to Config: " + String(data));
	return FileSystem.save(HONEYMONITOR_CONFIG, data);
}

bool ConfigClass::updateProperty(String property, String value, boolean saveValues){
	this->jsonDoc[property] = value;
	if (saveValues) {
		return save();
	} else {
		return true;
	}
}

// This method should call before every getParameter() call to get the update config from eeprom
void ConfigClass::load(){
	String savedData = FileSystem.get(HONEYMONITOR_CONFIG);      
	DeserializationError error = deserializeJson(jsonDoc, savedData);
	if (error) {
		Console.log("[Config] DeserializationError: " + String(error.c_str()));
	} else {
		Console.log("[Config] Data loaded.");
		//serializeJsonPretty(jsonDoc, Serial);
	}
}

String ConfigClass::getValueAsString(String name, bool loadbefore) {
	if (loadbefore){
		load();
	}
	
	String value = jsonDoc[name.c_str()].as<String>();
	
	//Console.log("Get value as String.\nName: \"" + String(name) + "\"\nvalue: \"" + String(value) + "\"");      
	//Console.log("Return value: " + String(value) + "\n-------------");
	if (!value || value == "null") {
		value = "";
	}
			 
	return value;
}

int ConfigClass::getValueAsInt(String name, bool loadbefore) {
	if (loadbefore){
		load();
	}
	
	int ret = -1;
	String value = jsonDoc[name.c_str()].as<String>();

	/* 
		 In case of interval the default value should be 1000ms
		 Set this value to prevent user typo
	*/

	if (name == "interval"){
		ret = 1000;
	}
	
	
	//Console.log("Get value as int.\nName: \"" + String(name) + "\"\nvalue: \"" + String(value) + "\"");                  
	if (IsNumeric(value)) {
		ret = value.toInt();
	}
	//Console.log("Return value: " + String(ret) + "\n-------------");      
	return ret;
}

void ConfigClass::reset(){
	FileSystem.remove(HONEYMONITOR_CONFIG);
}

void ConfigClass::restart(){
	Console.log("[Config] Restarting module...");
	ESP.restart();
}

boolean ConfigClass::IsNumeric(String str) {
	unsigned int stringLength = str.length();

	if (stringLength == 0) {
			return false;
	}

	boolean seenDecimal = false;

	for(unsigned int i = 0; i < stringLength; ++i) {
			if (isDigit(str.charAt(i))) {
					continue;
			}

			if (str.charAt(i) == '.') {
					if (seenDecimal) {
							return false;
					}
					seenDecimal = true;
					continue;
			}
			return false;
	}
	return true;
}

ConfigClass Config;
