#pragma once
#include "pebble.h"

typedef struct multi_alarm_data MultiAlarmData;

typedef int err_t;       // 0:Success, -1:Error

typedef size_t index_t;
#define INVALID_INDEX    ((index_t)(-1))

typedef struct multi_alarm_time {
    uint8_t hour;
    uint8_t min;
} MATime;

MultiAlarmData *multi_alarmm_data_create(size_t num_max);
void multi_alarm_data_destory(MultiAlarmData *data);
size_t multi_alarm_data_get_num_max(const MultiAlarmData *data);
size_t multi_alarm_data_get_num_usable(const MultiAlarmData *data);
void multi_alarm_data_sort_by_ascending_order(MultiAlarmData *data);
err_t multi_alarm_data_add(MultiAlarmData *data, index_t *index);
err_t multi_alarm_data_delete(MultiAlarmData *data, index_t index);
bool multi_alarm_data_get_usable(const MultiAlarmData *data, index_t index);
err_t multi_alarm_data_get_time_t_of_today(const MultiAlarmData *data, index_t index, time_t *tim);
err_t multi_alarm_data_get_time_t_of_after24h(const MultiAlarmData *data, index_t index, time_t *tim);
err_t multi_alarm_data_get_MATime(const MultiAlarmData *data, index_t index, MATime *tim);
err_t multi_alarm_data_set_MATime(MultiAlarmData *data, index_t index, MATime tim);
err_t multi_alarm_data_get_alarm_enable(const MultiAlarmData *data, index_t index, bool *enable);
err_t multi_alarm_data_set_alarm_enable(MultiAlarmData *data, index_t index, bool enable);