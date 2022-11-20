#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <string>
#include <Arduino.h>
#include <SPIFFS.h>

using namespace std;

#define KEY_MAX_LENGTH    100 // change it if key is longer
#define VALUE_MAX_LENGTH  100 // change it if value is longer

class ConfigParser {
public:
    static void lsAll();
    static bool configAvailable(const __FlashStringHelper * key);
    static int findInt(const __FlashStringHelper * key);
    static float findFloat(const __FlashStringHelper * key);
    static string findString(const __FlashStringHelper * key);
private:
    ConfigParser();
    static bool fileSystemUp();
    static int ascii2Int(char * ascii, int length);
    static float ascii2Float(char *ascii, int length);
    static string ascii2String(char *ascii, int length); 
    static int findKey(const __FlashStringHelper * key, char * value);
};

#endif