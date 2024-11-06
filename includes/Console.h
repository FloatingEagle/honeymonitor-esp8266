#ifndef HONEYMONITOR_CONSOLE_H
#define HONEYMONITOR_CONSOLE_H

#define CONSOLE_HISTORY_SIZE 50

#include <ESP8266WiFi.h>
#include "OnboardWifi.h"

class ConsoleClass {
	public:
		String history[CONSOLE_HISTORY_SIZE];
		int current_line = 0;
		void log(String message);
};

extern ConsoleClass Console;

#endif