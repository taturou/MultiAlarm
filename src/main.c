#include "pebble.h"
#include "PDUtils.h"
#include "multi_alarm_layer.h"
#include "multi_alarm_data.h"

static Window *window;
static MultiAlarmLayer *s_malarm_layer;
static MultiAlarmData *s_malarm_data;
#define MAX_DATA    (50)

static void s_multi_alarm_select_callback(MultiAlarmData *data, index_t index) {
    vibes_short_pulse();
}

static void s_malarm_update(struct tm *tick_time, TimeUnits units_changed) {
    multi_alarm_layer_update_abouttime(s_malarm_layer);
}

void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect window_frame = layer_get_frame(window_layer);

    s_malarm_data = multi_alarmm_data_create(MAX_DATA);
    {
        index_t index = 0;
        MATime data[] = {
            {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0},
            {6, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0},
            {13, 0}, {14, 0}, {15, 0}, {16, 0}, {17, 0}, {18, 0},
            {19, 0}, {20, 0}, {21, 0}, {22, 0}, {23, 0},
            {7, 37}, {7, 46}, {7, 52}, {17, 30}, {23, 30}, {23, 35}
        };

        for (index_t i = 0; i < (sizeof(data)/sizeof(MATime)); i++) {
            (void)multi_alarm_data_add(s_malarm_data, &index);
            (void)multi_alarm_data_set_MATime(s_malarm_data, index, data[i]);
        }
        multi_alarm_data_sort_by_ascending_order(s_malarm_data);
    }

    s_malarm_layer = multi_alarm_layer_create(window_frame);
    multi_alarm_layer_set_click_config_onto_window(s_malarm_layer, window);
    multi_alarm_layer_add_child_to_layer(s_malarm_layer, window_layer);
    multi_alarm_layer_set_data_pointer(s_malarm_layer, s_malarm_data);
    multi_alarm_layer_select_long_click_subscribe(s_malarm_layer, s_multi_alarm_select_callback);
    
    tick_timer_service_subscribe(SECOND_UNIT, s_malarm_update);
}

void window_unload(Window *window) {
    multi_alarm_layer_destroy(s_malarm_layer);
    multi_alarm_data_destory(s_malarm_data);
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