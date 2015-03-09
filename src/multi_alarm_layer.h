#pragma once
#include "pebble.h"
#include "multi_alarm_data.h"

typedef struct multi_alarm_layer MultiAlarmLayer;

typedef void (*MultiAlarmLayerClickCallback)(MultiAlarmData *data, index_t index);

MultiAlarmLayer *multi_alarm_layer_create(GRect frame);
void multi_alarm_layer_destroy(MultiAlarmLayer *malarm);
void multi_alarm_layer_set_click_config_onto_window(MultiAlarmLayer *malarm, Window *window);
void multi_alarm_layer_add_child_to_layer(MultiAlarmLayer *malarm, Layer *layer);
void multi_alarm_layer_mark_dirty(MultiAlarmLayer *malarm);
void multi_alarm_layer_set_data_pointer(MultiAlarmLayer *malarm, MultiAlarmData *data);
void multi_alarm_layer_set_data_index(MultiAlarmLayer *malarm, index_t index);
void multi_alarm_layer_set_data_index_correspond_to_time(MultiAlarmLayer *malarm, MATime tim);
void multi_alarm_layer_update_abouttime(MultiAlarmLayer *malarm);
void multi_alarm_layer_select_click_subscribe(MultiAlarmLayer *malarm, MultiAlarmLayerClickCallback handler);
void multi_alarm_layer_select_long_click_subscribe(MultiAlarmLayer *malarm, MultiAlarmLayerClickCallback handler);