#include "ConfigParser.h"

std::string ConfigParser::backUpFilePath = "";

bool ConfigParser::fileSystemUp() {
    if (!SPIFFS.begin(true)) {
        log_e("An Error has occurred while mounting SPIFFS");
        return false;
    } 
    log_i("Filesystem opened");
    return true;
}

void ConfigParser::lsAll() {
    if (ConfigParser::fileSystemUp()) {
        log_i("Attempting to list files from /");
        File root = SPIFFS.open("/");
        File file = root.openNextFile();

        if (file) {
            while(file) {
                log_i("File: %s", file.name());
                file = root.openNextFile();
            }
        } else {   
            log_i("No files in /");
        }
    }
}

int ConfigParser::ascii2Int(char *ascii, int length) {
    int sign = 1;
    int number = 0;

    for (int i = 0; i < length; i++) {
        char c = *(ascii + i);
        if (i == 0 && c == '-')
        sign = -1;
        else {
        if (c >= '0' && c <= '9')
            number = number * 10 + (c - '0');
        }
    }

    return number * sign;
}

float ConfigParser::ascii2Float(char *ascii, int length) {
    int sign = 1;
    int decimalPlace = 0;
    float number  = 0;
    float decimal = 0;

    for (int i = 0; i < length; i++) {
        char c = *(ascii + i);
        if (i == 0 && c == '-')
        sign = -1;
        else {
        if (c == '.')
            decimalPlace = 1;
        else if (c >= '0' && c <= '9') {
            if (!decimalPlace)
            number = number * 10 + (c - '0');
            else {
            decimal += ((float)(c - '0') / pow(10.0, decimalPlace));
            decimalPlace++;
            }
        }
        }
    }

    return (number + decimal) * sign;
}

string ConfigParser::ascii2String(char *ascii, int length) {
    string str;
    str.reserve(length);
    str = "";

    for (int i = 0; i < length; i++) {
        char c = *(ascii + i);
        str += c;
    }

    return str;
}

bool ConfigParser::copyFile(fs::FS &fs, const char *sourcePath, const char *destPath) {
  File sourceFile = fs.open(sourcePath, "r");
  if (!sourceFile) {
    return false; // Source file not found or failed to open
  }

  File destFile = fs.open(destPath, "w");
  if (!destFile) {
    sourceFile.close();
    return false; // Destination file failed to open
  }

  // Read from source file and write to destination file
  uint8_t buffer[128];
  while (sourceFile.available()) {
    size_t bytesRead = sourceFile.read(buffer, sizeof(buffer));
    destFile.write(buffer, bytesRead);
  }

  sourceFile.close();
  destFile.close();

  return true; // File copied successfully
}

void ConfigParser::setBackUpFilePath(const char * path) {
    ConfigParser::backUpFilePath = path;
}

int ConfigParser::findKey(const __FlashStringHelper * key, char * value) {
    if(!SPIFFS.exists("/.env") == true) {
        if(SPIFFS.exists(ConfigParser::backUpFilePath.c_str()) == true) {
            log_d("Attempting to restore .env from .env.backup"); 
            if (ConfigParser::copyFile(SPIFFS, ConfigParser::backUpFilePath.c_str(), "/.env")) {
                log_d("Restored .env from %s", ConfigParser::backUpFilePath.c_str());
            } else {
                log_e("Failed to restore .env from %s", ConfigParser::backUpFilePath.c_str());
                return 1;
            }
        } else {
            log_e("Failed to open .env, %s not found", "/.env");
            return 1;
        }
    }

    File configFile = SPIFFS.open("/.env", "r", false);
    log_d("Opening %s for reading", "/.env");

    if (!configFile) {
        log_e("Failed to open .env, %s exists but failed to open", "/.env");
        return 1;
    }

    char key_string[KEY_MAX_LENGTH];
    char config_buffer[KEY_MAX_LENGTH + VALUE_MAX_LENGTH + 1]; // 1 is = character
    int key_length = 0;
    int value_length = 0;

    // Flash string to string
    PGM_P keyPoiter;
    keyPoiter = reinterpret_cast<PGM_P>(key);
    byte ch;
    do {
        ch = pgm_read_byte(keyPoiter++);
        if (ch != 0)
        key_string[key_length++] = ch;
    } while (ch != 0);

    // check line by line
    while (configFile.available()) {
        int buffer_length = configFile.readBytesUntil('\n', config_buffer, 100);
        if (config_buffer[buffer_length - 1] == '\r')
        buffer_length--; // trim the \r

        if (buffer_length > (key_length + 1)) { // 1 is = character
            if (memcmp(config_buffer, key_string, key_length) == 0) { // equal
                if (config_buffer[key_length] == '=') {
                value_length = buffer_length - key_length - 1;
                memcpy(value, config_buffer + key_length + 1, value_length);
                break;
                }
            }
        }
    }

    configFile.close();
    return value_length;
}

bool ConfigParser::configAvailable(const __FlashStringHelper * key) {
    char value_string[VALUE_MAX_LENGTH];
    int value_length = ConfigParser::findKey(key, value_string);
    return value_length > 0;
}

int ConfigParser::findInt(const __FlashStringHelper * key) {
    char value_string[VALUE_MAX_LENGTH];
    int value_length = ConfigParser::findKey(key, value_string);
    return ConfigParser::ascii2Int(value_string, value_length);
}

float ConfigParser::findFloat(const __FlashStringHelper * key) {
    char value_string[VALUE_MAX_LENGTH];
    int value_length = ConfigParser::findKey(key, value_string);
    return ConfigParser::ascii2Float(value_string, value_length);
}

string ConfigParser::findString(const __FlashStringHelper * key) {
    char value_string[VALUE_MAX_LENGTH];
    int value_length = ConfigParser::findKey(key, value_string);
    return ConfigParser::ascii2String(value_string, value_length);
}
