#include "../includes/FileSystem.h"

void FileSystemClass::setup() {
	bool initialized = LittleFS.begin();
	if (initialized) {
		Console.log("[FileSystem] Initialized");
	} else {
		Console.log("[FileSystem] ERROR INITIALIZING.");
		delay(10000);
	    Config.restart();
	}
}

String FileSystemClass::get(String path) {
	if(!LittleFS.exists(path)) {
	    Console.log("[FileSystem] (get) " + String(path) + " does not exist.");
	    return "";
	}

	File file = LittleFS.open(path, "r");
	if (!file) {
	    Console.log("[FileSystem] (get) Error opening " + String(path) + " for reading.");
	    return "";
	}
	
	String content = file.readString();
	file.close();
	return content;
}

bool FileSystemClass::save(String path, String content) {
	File file = LittleFS.open(path, "w");
	if (!file) {
	    Console.log("[FileSystem] (save) Error opening " + String(path) + " for writting.");
	    return "";
	}
	
	bool success = file.print(content);
	file.close();
	return success;
}

bool FileSystemClass::remove(String path) {
	if(!LittleFS.exists(path)) {
	    Console.log("[FileSystem] (remove) " + String(path) + " does not exist.");
	    return true;
	}

	return LittleFS.remove(path);
}

FileSystemClass FileSystem;