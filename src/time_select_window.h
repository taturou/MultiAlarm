#pragma once
#include "pebble.h"
#include "multi_alarm_data.h"

typedef struct timet_select_window TimeSelectWindow;

typedef void (*TimeSelectWindowCallback)(MATime tim, void *data);

TimeSelectWindow *time_select_window_create(TimeSelectWindowCallback callback, MATime initial_tim, void *data);
void time_select_window_destroy(TimeSelectWindow *menu);