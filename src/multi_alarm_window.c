#include <pebble.h>
#include "multi_alarm_window.h"

#define MENU_BOUNDS_SIZE_WIDTH       (50)

#define ROOT_LAYER_SLIDE_DURATION    (200)

typedef struct multi_alarm_windows {
    Window *window;
    Layer *root_layer;
} MultiAlarmWindow;

static void s_window_load(Window *window);
static void s_window_appear(Window *window);
static void s_window_unload(Window *window);
static void s_animation_stopped(struct Animation *animation, bool finished, void *context);
static void s_root_layer_update_proc(struct Layer *layer, GContext *ctx);

MultiAlarmWindow *multi_alarm_window_create(void) {
    MultiAlarmWindow *mawindow = NULL;
    
    mawindow = calloc(sizeof(MultiAlarmWindow), 1);
    if (mawindow != NULL) {
        Window *window = window_create();
        if (window != NULL) {
            mawindow->window = window;

            window_set_user_data(window, (void*)mawindow);
            window_set_window_handlers(window, (WindowHandlers) {
                .load = s_window_load,
                .appear = s_window_appear,
                .unload = s_window_unload,
            });
        }
    }
    return mawindow;
}

void multi_alarm_window_destroy(MultiAlarmWindow *mawindow) {
    if (mawindow != NULL) {
        window_destroy(mawindow->window);
    }
}

void multi_alarm_window_show(MultiAlarmWindow *mawindow) {
    window_stack_push(mawindow->window, false);
}

void multi_alarm_window_hide(MultiAlarmWindow *mawindow) {
    window_stack_pop(false);
}

GRect multi_alarm_window_get_root_layer_frame(MultiAlarmWindow *mawindow) {
    return layer_get_frame(mawindow->root_layer);
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

    window_set_background_color(window, GColorClear);
    window_set_fullscreen(window, false);

    // create root_layer
    mawindow->root_layer = layer_create(layer_get_frame(window_get_root_layer(window)));
    layer_set_update_proc(mawindow->root_layer, s_root_layer_update_proc);
}

static void s_window_appear(Window *window) {
    MultiAlarmWindow *mawindow = (MultiAlarmWindow*)window_get_user_data(window);
    
    Layer *window_layer = window_get_root_layer(window);
    GRect window_frame = layer_get_frame(window_layer);
    GRect to_frame = GRect(MENU_BOUNDS_SIZE_WIDTH, 
                           0,
                           window_frame.size.w - MENU_BOUNDS_SIZE_WIDTH,
                           window_frame.size.h);
    GRect from_frame = GRect(window_frame.size.w, 0, 0, window_frame.size.h);

    layer_set_frame(mawindow->root_layer, from_frame);
    layer_add_child(window_layer, mawindow->root_layer);

    PropertyAnimation *anime = property_animation_create_layer_frame(mawindow->root_layer, &from_frame, &to_frame);
    animation_set_duration((Animation*)anime, ROOT_LAYER_SLIDE_DURATION);
    animation_set_handlers((Animation*)anime,
                           (AnimationHandlers){
                               .stopped = s_animation_stopped,
                           },
                           (void*)mawindow);
    animation_schedule((Animation*)anime);
}

static void s_window_unload(Window *window) {
    MultiAlarmWindow *mawindow = (MultiAlarmWindow*)window_get_user_data(window);

    window_set_user_data(window, NULL);
    layer_destroy(mawindow->root_layer);
    mawindow->root_layer = NULL;
    free(mawindow);
}

static void s_animation_stopped(struct Animation *animation, bool finished, void *context) {
    animation_destroy(animation);
}

static void s_root_layer_update_proc(struct Layer *layer, GContext *ctx) {
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}