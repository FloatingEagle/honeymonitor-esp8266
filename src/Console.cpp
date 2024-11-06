#include "../includes/Console.h"

void ConsoleClass::log(String msg) {
    String msg_time = OnboardWifi.getTime() + " : " + "\t" + msg;
    Serial.println(msg_time);
    #ifdef DEBUG_RAM_USAGE
        Serial.println("\tRAM: "+String(ESP.getFreeHeap()));
    #endif

    history[current_line++] = msg_time;
    if(current_line >= CONSOLE_HISTORY_SIZE)
        current_line = 0;
}

ConsoleClass Console;
