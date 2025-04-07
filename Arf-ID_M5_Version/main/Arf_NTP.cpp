#include "Arf_NTP.h"

static constexpr const char *const wd[7] = {"Sun", "Mon", "Tue", "Wed",
                                            "Thr", "Fri", "Sat"};

void arf_ntp_setup()
{
    configTzTime(NTP_TIMEZONE, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);

#if SNTP_ENABLED
    while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED)
    {
        Serial.print('.');
        delay(1000);
    }
#else
    delay(1600);
    struct tm timeInfo;
    while (!getLocalTime(&timeInfo, 1000))
    {
        Serial.print('.');
    };
#endif

    Serial.println("\r\n NTP Connected.");

    time_t t = time(nullptr) + 1; // Advance one second.
    while (t > time(nullptr))
        ; /// Synchronization in seconds
    M5Capsule.Rtc.setDateTime(gmtime(&t));
}

std::string getDateString()
{
    // 2024_11_19_
    auto dt = M5Capsule.Rtc.getDateTime();
    std::string date_string = std::to_string(dt.date.year) + "_" + std::to_string(dt.date.month) + "_" + std::to_string(dt.date.date) + "_";
    return date_string;
}

void arf_ntp_fulltime()
{
    auto dt = M5Capsule.Rtc.getDateTime();
    Serial.printf("RTC   UTC  :%04d/%02d/%02d (%s)  %02d:%02d:%02d\r\n",
                  dt.date.year, dt.date.month, dt.date.date,
                  wd[dt.date.weekDay], dt.time.hours, dt.time.minutes,
                  dt.time.seconds);
    // get the above as a string
    // RTC   UTC  :2024/11/19 (Tue)  03:50:33
    char timeStr[100];
    sprintf(timeStr, "%04d/%02d/%02d %02d:%02d:%02d", dt.date.year, dt.date.month, dt.date.date, dt.time.hours, dt.time.minutes, dt.time.seconds);
}

std::string arf_ntp_reading_time()
{
    auto dt = M5Capsule.Rtc.getDateTime();
    //03:50:33
    std::string time_string = std::to_string(dt.time.hours) + ":" + std::to_string(dt.time.minutes) + ":" + std::to_string(dt.time.seconds);
    return time_string;
}
