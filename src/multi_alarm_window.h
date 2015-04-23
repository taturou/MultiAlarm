#pragma once
#include "pebble.h"

typedef struct multi_alarm_windows MultiAlarmWindow;

MultiAlarmWindow *multi_alarm_window_create(void);
void multi_alarm_window_destroy(MultiAlarmWindow *mawindow);
void multi_alarm_window_show(MultiAlarmWindow *mawindow);
void multi_alarm_window_hide(MultiAlarmWindow *mawindow);
GRect multi_alarm_window_get_root_layer_frame(MultiAlarmWindow *mawindow);
void multi_alarm_window_root_layer_add_child(MultiAlarmWindow *mawindow, Layer *child);
void multi_alarm_window_root_layer_remove_from_parent(MultiAlarmWindow *mawindow, Layer *child);