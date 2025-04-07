#ifndef sdcard_h
#define sdcard_h

#include "FS.h"
#include "SPI.h"
#include "SD.h"
#include <SPI.h>
#include <vector>

using namespace std;
static constexpr const gpio_num_t SDCARD_CSPIN = GPIO_NUM_11;

std::vector<std::string>  listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
std::string SD_setup(std::string current_date);
void saveDataSD(string filename, string epc, string reading_time);


#endif