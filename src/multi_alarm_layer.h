#pragma once
#include "pebble.h"

typedef struct multi_alarm_layer MultiAlarmLayer;

typedef struct multi_alarm_data {
    struct {
        uint8_t hour;
        uint8_t min;
    } time;
    bool alarm;
} MultiAlarmData;

MultiAlarmLayer *multi_alarm_layer_create(GRect frame);
void multi_alarm_layer_destroy(MultiAlarmLayer *malarm);
void multi_alarm_layer_set_click_config_onto_window(MultiAlarmLayer *malarm, Window *window);
void multi_alarm_layer_add_child_to_layer(MultiAlarmLayer *malarm, Layer *layer);
// The module require data that is sorted in ascending order. 
void multi_alarm_layer_set_data_pointer(MultiAlarmLayer *malarm, MultiAlarmData *data, uint8_t size);
void multi_alarm_layer_update_abouttime(MultiAlarmLayer *malarm);

time_t multi_alarm_data_get_time(MultiAlarmData *data);