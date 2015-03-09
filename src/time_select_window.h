#pragma once
#include "pebble.h"
#include "multi_alarm_data.h"

typedef struct timet_select_window TimeSelectWindow;

typedef void (*TimeSelectWindowCallback)(MATime tim);

TimeSelectWindow *time_select_window_create(void);
void time_select_window_destroy(TimeSelectWindow *window);
bool time_select_window_show(TimeSelectWindow *window, TimeSelectWindowCallback callback, MATime initial_tim);
bool time_select_window_hide(TimeSelectWindow *window);