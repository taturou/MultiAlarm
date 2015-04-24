#include "pebble.h"
#include "PDUtils.h"
#include "multi_alarm_layer.h"
#include "multi_alarm_data.h"
#if 0 // not use multi_alarm_data_operation_window
#include "multi_alarm_data_operation_window.h"
#include "time_select_window.h"
#endif
#include "multi_alarm_window.h"

static Window *s_window;
static MultiAlarmLayer *s_malayer;
static MultiAlarmData *s_madata;
#if 0 // not use multi_alarm_data_operation_window
static MultiAlarmDataOperationWindow *s_malarm_ope_window;
static TimeSelectWindow *s_tselect_window;
#endif
    
#define MAX_DATA    (50)

#if 0 // not use multi_alarm_data_operation_window
static void s_select_menu_select_callback(MultiAlarmDataOperationElementID id, MultiAlarmDataOperationElement element, MultiAlarmData *data, index_t index) {
    switch (id) {
    case MADOE_Enable:
        (void)multi_alarm_data_set_alarm_enable(data, index, element.enable.enable);
        multi_alarm_layer_mark_dirty(s_malayer);
        break;
    case MADOE_Edit:
        {
            index_t exist_index = multi_alarm_data_get_index_to_MATime(data, element.edit.time);
            if (exist_index == MA_INDEX_INVALID) {
                (void)multi_alarm_data_set_MATime(data, index, element.edit.time);
                multi_alarm_data_sort_by_ascending_order(s_madata);
                multi_alarm_layer_set_data_pointer(s_malayer, s_madata);
                index = multi_alarm_data_get_index_to_MATime(data, element.edit.time);
                multi_alarm_layer_set_data_index(s_malayer, index);
            } else {
                vibes_short_pulse();
            }
        }
        break;
    case MADOE_Add:
        {
            index_t exist_index = multi_alarm_data_get_index_to_MATime(data, element.add.time);
            if (exist_index == MA_INDEX_INVALID) {
                if (multi_alarm_data_add(s_madata, &index) == 0) {
                    (void)multi_alarm_data_set_MATime(s_madata, index, element.add.time);
                    multi_alarm_data_sort_by_ascending_order(s_madata);
                    multi_alarm_layer_set_data_pointer(s_malayer, s_madata);
                    index = multi_alarm_data_get_index_to_MATime(data, element.edit.time);
                    multi_alarm_layer_set_data_index(s_malayer, index);
                }
            } else {
                vibes_short_pulse();
            }
        }
        break;
    case MADOE_Delete:
        multi_alarm_data_delete(data, index);
        multi_alarm_data_sort_by_ascending_order(s_madata);
        multi_alarm_layer_set_data_pointer(s_malayer, s_madata);
        multi_alarm_layer_set_data_index(s_malayer, index == 0 ? 0 : (index - 1));
        break;
    }
}

static void s_time_select_window_callback(MATime tim, void *data) {
    MultiAlarmDataOperationElement element;
    element.add.time = tim;
    s_select_menu_select_callback(MADOE_Add, element, s_madata, MA_INDEX_INVALID);
}
#endif

static void s_multi_alarm_select_callback(MultiAlarmData *data, index_t index) {
    if (index != MA_INDEX_INVALID) {
        bool enable = false;
        if (multi_alarm_data_get_alarm_enable(data, index, &enable) == 0) {
            (void)multi_alarm_data_set_alarm_enable(data, index, enable == true ? false : true);
            multi_alarm_layer_mark_dirty(s_malayer);
        }
    } else {
#if 0 // not use multi_alarm_data_operation_window
        MATime tim = {0, 0};
        s_tselect_window = time_select_window_create(s_time_select_window_callback, tim, NULL);
#endif
    }
}

static void s_multi_alarm_long_select_callback(MultiAlarmData *data, index_t index) {
#if 0 // not use multi_alarm_data_operation_window
    if (index != MA_INDEX_INVALID) {
        multi_alarm_data_operation_window_show(s_malarm_ope_window, s_select_menu_select_callback, data, index);
    } else {
        MATime tim = {0, 0};
        s_tselect_window = time_select_window_create(s_time_select_window_callback, tim, NULL);
    }
#endif
    MultiAlarmWindow *s_mawindow = multi_alarm_window_create(s_window);
    multi_alarm_window_show(s_mawindow);
}

static void s_malarm_update(struct tm *tick_time, TimeUnits units_changed) {
    multi_alarm_layer_update_abouttime(s_malayer);
}

void s_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect window_frame = layer_get_frame(window_layer);

    s_madata = multi_alarmm_data_create(MAX_DATA);
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
            (void)multi_alarm_data_add(s_madata, &index);
            (void)multi_alarm_data_set_MATime(s_madata, index, data[i]);
        }
        multi_alarm_data_sort_by_ascending_order(s_madata);
    }

    s_malayer = multi_alarm_layer_create(window_frame);
    multi_alarm_layer_set_click_config_onto_window(s_malayer, window);
    multi_alarm_layer_add_child_to_layer(s_malayer, window_layer);
    multi_alarm_layer_select_click_subscribe(s_malayer, s_multi_alarm_select_callback);
    multi_alarm_layer_select_long_click_subscribe(s_malayer, s_multi_alarm_long_select_callback);
    multi_alarm_layer_set_data_pointer(s_malayer, s_madata);
    multi_alarm_layer_set_data_index(s_malayer, MA_INDEX_NEAR_NOW_TIME);

#if 0 // not use multi_alarm_data_operation_window
    s_malarm_ope_window = multi_alarm_data_operation_window_create();
#endif

    tick_timer_service_subscribe(SECOND_UNIT, s_malarm_update);
}

void s_window_unload(Window *window) {
#if 0 // not use multi_alarm_data_operation_window
    multi_alarm_data_operation_window_destroy(s_malarm_ope_window);
#endif
    multi_alarm_layer_destroy(s_malayer);
    multi_alarm_data_destory(s_madata);
}

int main(void) {
    s_window = window_create();
    window_set_fullscreen(s_window, false);

    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = s_window_load,
        .unload = s_window_unload,
    });

    window_stack_push(s_window, true /* Animated */);
    app_event_loop();
    window_destroy(s_window);
}