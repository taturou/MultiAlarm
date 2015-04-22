#include <pebble.h>
#include "multi_alarm_window.h"

#define MENU_BOUNDS_SIZE_WIDTH  (50) 

typedef struct multi_alarm_windows {
    Window *window;
    Layer *root_layer;
} MultiAlarmWindow;

static void s_window_load(Window *window);
static void s_window_unload(Window *window);
static void s_root_layer_update_proc(struct Layer *layer, GContext *ctx);

MultiAlarmWindow *multi_alarm_window_create(void) {
    MultiAlarmWindow *mawindow = NULL;
    
    mawindow = calloc(sizeof(MultiAlarmWindow), 1);
    if (mawindow != NULL) {
        Window *window = window_create();
        if (window != NULL) {
            mawindow->window = window;

            // init window
            {
                window_set_background_color(window, GColorClear);
                window_set_fullscreen(window, true);
                window_set_user_data(window, (void*)mawindow);
                window_set_window_handlers(window, (WindowHandlers) {
                    .load = s_window_load,
                    .unload = s_window_unload,
                });
            }

            // init root-layer
            {
                Layer *window_layer = window_get_root_layer(window);
                GRect window_frame = layer_get_frame(window_layer);

                mawindow->root_layer = layer_create(
                    GRect(MENU_BOUNDS_SIZE_WIDTH, 
                          0,
                          window_frame.size.w - MENU_BOUNDS_SIZE_WIDTH,
                          window_frame.size.h));
                layer_set_update_proc(mawindow->root_layer, s_root_layer_update_proc);
            }
        }
    }
    return mawindow;
}

void multi_alarm_window_destroy(MultiAlarmWindow *mawindow, bool animated) {
    if (mawindow != NULL) {
        multi_alarm_window_hide(mawindow, animated);
        layer_destroy(mawindow->root_layer);
        window_destroy(mawindow->window);
        free(mawindow);
    }
}

void multi_alarm_window_show(MultiAlarmWindow *mawindow, bool animated) {
    window_stack_push(mawindow->window, animated);
}

void multi_alarm_window_hide(MultiAlarmWindow *mawindow, bool animated) {
    (void)mawindow;
    window_stack_pop(animated);
}

GRect multi_alarm_window_get_root_layer_frame(MultiAlarmWindow *mawindow) {
    GRect frame = layer_get_frame(mawindow->root_layer);
    return GRect(0, 0, frame.size.w, frame.size.h);
}

void multi_alarm_window_root_layer_add_child(MultiAlarmWindow *mawindow, Layer *child) {
    layer_add_child(mawindow->root_layer, child);
}

void multi_alarm_window_root_layer_remove_from_parent(MultiAlarmWindow *mawindow, Layer *child) {
    (void)mawindow;
    layer_remove_from_parent(child);
}

static void s_window_load(Window *window) {
    MultiAlarmWindow *mawindow = (MultiAlarmWindow*)window_get_user_data(window);
    
    layer_add_child(window_get_root_layer(window), mawindow->root_layer);
}

static void s_window_unload(Window *window) {
    (void)window;
}

static void s_root_layer_update_proc(struct Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    
    graphics_context_set_stroke_color(ctx, GColorWhite);
    for (int y = 0; y < bounds.size.h; y += 2) {
        graphics_draw_line(ctx, GPoint(0, y), GPoint(bounds.size.w, y));
    }
}