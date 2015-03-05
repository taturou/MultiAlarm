#pragma once
#include "pebble.h"

typedef struct multi_alarm_data {
    struct {
        uint8_t hour;
        uint8_t min;
    } time;
    bool alarm;
} MultiAlarmData;

typedef struct multi_alarm_pod {
    MultiAlarmData *data;
    size_t num_data;
} MultiAlarmPod;

time_t multi_alarm_data_get_time(MultiAlarmData *data, bool today_only);