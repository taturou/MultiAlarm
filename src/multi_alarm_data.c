#include <pebble.h>
#include "multi_alarm_data.h"
#include "PDUtils.h"

time_t multi_alarm_data_get_time(MultiAlarmData *data, bool today_only) {
    time_t now_time = time(NULL);
    struct tm now_tm;
    memcpy(&now_tm, localtime(&now_time), sizeof(struct tm));

    now_tm.tm_hour = data->time.hour;
    now_tm.tm_min = data->time.min;
    now_tm.tm_sec = 0;

    time_t data_time = p_mktime(&now_tm);
    if (today_only == false) {
        if (data_time < now_time) {
            data_time += 24 * 60 * 60; // second of 1day
        }
    }
    return data_time;
}