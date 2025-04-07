#ifndef arf_ntp_h
#define arf_ntp_h

#include <M5Capsule.h>
#define NTP_TIMEZONE  "UTC-8"
#define NTP_SERVER1   "0.pool.ntp.org"
#define NTP_SERVER2   "1.pool.ntp.org"
#define NTP_SERVER3   "2.pool.ntp.org"
#include <WiFi.h>
#include <String>


// Different versions of the framework have different SNTP header file names and
// availability.
#if __has_include(<esp_sntp.h>)
#include <esp_sntp.h>
#define SNTP_ENABLED 1
#elif __has_include(<sntp.h>)
#include <sntp.h>
#define SNTP_ENABLED 1
#endif

void arf_ntp_setup();
void arf_ntp_fulltime();
std::string getDateString();
std::string arf_ntp_reading_time();



#endif
