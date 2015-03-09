#include "pebble.h"
#include "PDUtils.h"
#include "multi_alarm_layer.h"
#include "multi_alarm_data.h"
#include "select_menu.h"
#include "time_select_window.h"

static Window *s_window;
static MultiAlarmLayer *s_malarm_layer;
static MultiAlarmData *s_malarm_data;
static SelectMenu *s_select_menu;
static TimeSelectWindow *s_tselect_window;

#define MAX_DATA    (50)

static void s_select_menu_select_callback(SelectMenuElementID id, SelectMenuElement element, MultiAlarmData *data, index_t index) {
    switch (id) {
    case SME_Enable:
        (void)multi_alarm_data_set_alarm_enable(data, index, element.enable.enable);
        multi_alarm_layer_mark_dirty(s_malarm_layer);
        break;
    case SME_Edit:
        {
            index_t exist_index = multi_alarm_data_get_index_to_MATime(data, element.edit.time);
            if (exist_index == MA_INDEX_INVALID) {
                (void)multi_alarm_data_set_MATime(data, index, element.edit.time);
                multi_alarm_data_sort_by_ascending_order(s_malarm_data);
                multi_alarm_layer_set_data_pointer(s_malarm_layer, s_malarm_data);
                index = multi_alarm_data_get_index_to_MATime(data, element.edit.time);
                multi_alarm_layer_set_data_index(s_malarm_layer, index);
            } else {
                vibes_short_pulse();
            }
        }
        break;
    case SME_Add:
        {
            index_t exist_index = multi_alarm_data_get_index_to_MATime(data, element.add.time);
            if (exist_index == MA_INDEX_INVALID) {
                if (multi_alarm_data_add(s_malarm_data, &index) == 0) {
                    (void)multi_alarm_data_set_MATime(s_malarm_data, index, element.add.time);
                    multi_alarm_data_sort_by_ascending_order(s_malarm_data);
                    multi_alarm_layer_set_data_pointer(s_malarm_layer, s_malarm_data);
                    index = multi_alarm_data_get_index_to_MATime(data, element.edit.time);
                    multi_alarm_layer_set_data_index(s_malarm_layer, index);
                }
            } else {
                vibes_short_pulse();
            }
        }
        break;
    case SME_Delete:
        multi_alarm_data_delete(data, index);
        multi_alarm_data_sort_by_ascending_order(s_malarm_data);
        multi_alarm_layer_set_data_pointer(s_malarm_layer, s_malarm_data);
        multi_alarm_layer_set_data_index(s_malarm_layer, index == 0 ? 0 : (index - 1));
        break;
    }
}

static void s_time_select_window_callback(MATime tim) {
    SelectMenuElement element;
    element.add.time = tim;
    s_select_menu_select_callback(SME_Add, element, s_malarm_data, MA_INDEX_INVALID);
}

static void s_multi_alarm_select_callback(MultiAlarmData *data, index_t index) {
    if (index != MA_INDEX_INVALID) {
        bool enable = false;
        if (multi_alarm_data_get_alarm_enable(data, index, &enable) == 0) {
            (void)multi_alarm_data_set_alarm_enable(data, index, enable == true ? false : true);
            multi_alarm_layer_mark_dirty(s_malarm_layer);
        }
    } else {
        MATime tim = {0, 0};
        time_select_window_show(s_tselect_window, s_time_select_window_callback, tim);
    }
}

static void s_multi_alarm_long_select_callback(MultiAlarmData *data, index_t index) {
    if (index != MA_INDEX_INVALID) {
        select_menu_show(s_select_menu, s_select_menu_select_callback, data, index);
    } else {
        MATime tim = {0, 0};
        time_select_window_show(s_tselect_window, s_time_select_window_callback, tim);
    }
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
    multi_alarm_layer_select_click_subscribe(s_malarm_layer, s_multi_alarm_select_callback);
    multi_alarm_layer_select_long_click_subscribe(s_malarm_layer, s_multi_alarm_long_select_callback);
    multi_alarm_layer_set_data_pointer(s_malarm_layer, s_malarm_data);
    multi_alarm_layer_set_data_index(s_malarm_layer, MA_INDEX_NEAR_NOW_TIME);

    s_select_menu = select_menu_create();
    s_tselect_window = time_select_window_create();

    tick_timer_service_subscribe(SECOND_UNIT, s_malarm_update);
}

void window_unload(Window *window) {
    time_select_window_destroy(s_tselect_window);
    select_menu_destroy(s_select_menu);
    multi_alarm_layer_destroy(s_malarm_layer);
    multi_alarm_data_destory(s_malarm_data);
}

int main(void) {
    s_window = window_create();
    window_set_fullscreen(s_window, true);

    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });

    window_stack_push(s_window, true /* Animated */);
    app_event_loop();
    window_destroy(s_window);
}