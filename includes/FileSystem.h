#ifndef HONEYMONITOR_FILESYSTEM_H
#define HONEYMONITOR_FILESYSTEM_H

#include <LittleFS.h>
#include "WString.h"
#include "Console.h"

class FileSystemClass {
	public:
		void setup();

		String get(String file);
		bool save(String path, String content);
		bool remove(String path);
};

extern FileSystemClass FileSystem;

#endif