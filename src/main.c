#include "pebble.h"
#include "PDUtils.h"
#include "multi_alarm_layer.h"

static Window *window;
static MultiAlarmLayer *s_malarm_layer;

static MultiAlarmData s_malarm_data[] = {
    {{6, 0}, false},
    {{7, 37}, false},
    {{7, 46}, false},
    {{7, 52}, false},
    {{8, 0}, false},
    {{9, 0}, false},
    {{10, 0}, false},
    {{11, 0}, false},
    {{12, 0}, false},
    {{13, 0}, false},
    {{14, 0}, false},
    {{15, 0}, false},
    {{15, 0}, false},
    {{16, 0}, false},
    {{17, 0}, false},
    {{17, 30}, false},
    {{18, 0}, false},
    {{19, 0}, false},
    {{20, 0}, false},
    {{21, 0}, false},
    {{22, 0}, false},
    {{23, 0}, false},
    {{23, 30}, false},
    {{0, 0}, false},
    {{1, 0}, false},
    {{2, 0}, false},
    {{3, 0}, false},
    {{4, 0}, false},
    {{5, 0}, false},
};
static MultiAlarmPod s_malarm_pod = {s_malarm_data, sizeof(s_malarm_data)/sizeof(MultiAlarmData)};

static void s_malarm_update(struct tm *tick_time, TimeUnits units_changed) {
    multi_alarm_layer_update_abouttime(s_malarm_layer);
}

void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect window_frame = layer_get_frame(window_layer);

    s_malarm_layer = multi_alarm_layer_create(window_frame);
    multi_alarm_layer_set_click_config_onto_window(s_malarm_layer, window);
    multi_alarm_layer_add_child_to_layer(s_malarm_layer, window_layer);
    multi_alarm_layer_set_data_pointer(s_malarm_layer, &s_malarm_pod);
    
    tick_timer_service_subscribe(SECOND_UNIT, s_malarm_update);
}

void window_unload(Window *window) {
    multi_alarm_layer_destroy(s_malarm_layer);
}

int main(void) {
    window = window_create();

    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });

    window_stack_push(window, true /* Animated */);
    app_event_loop();
    window_destroy(window);
}