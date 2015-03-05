#pragma once
#include "pebble.h"
#include "multi_alarm_data.h"

typedef struct multi_alarm_layer MultiAlarmLayer;

MultiAlarmLayer *multi_alarm_layer_create(GRect frame);
void multi_alarm_layer_destroy(MultiAlarmLayer *malarm);
void multi_alarm_layer_set_click_config_onto_window(MultiAlarmLayer *malarm, Window *window);
void multi_alarm_layer_add_child_to_layer(MultiAlarmLayer *malarm, Layer *layer);
void multi_alarm_layer_set_data_pointer(MultiAlarmLayer *malarm, MultiAlarmData *data);
void multi_alarm_layer_update_abouttime(MultiAlarmLayer *malarm);