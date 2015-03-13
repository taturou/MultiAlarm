#include <pebble.h>
#include "time_select_window.h"

#define MENU_BOUNDS_SIZE_WIDTH  (50)

typedef struct timet_select_window {
    Window *window;
    Layer *layer;
    MATime tim;
    TimeSelectWindowCallback callback;
    void *data;
} TimeSelectWindow;

static void s_window_load(Window *window);
static void s_window_unload(Window *window);

TimeSelectWindow *time_select_window_create(TimeSelectWindowCallback callback, MATime initial_tim, void *data) {
    TimeSelectWindow *menu = NULL;

    menu = calloc(sizeof(TimeSelectWindow), 1);
    if (menu != NULL) {
        Window *window = window_create();
        if (window != NULL) {
            menu->window = window;
            menu->callback = callback;
            menu->tim = initial_tim;
            menu->data = data;

            // init window
            window_set_background_color(window, GColorWhite);
            window_set_fullscreen(window, true);
            window_set_user_data(window, (void*)menu);
            window_set_window_handlers(window, (WindowHandlers) {
                .load = s_window_load,
                .unload = s_window_unload,
            });
            window_stack_push(window, true /* Animated */);
        } else {
            time_select_window_destroy(menu);
            menu = NULL;
        }
    }
    return menu;
}

void time_select_window_destroy(TimeSelectWindow *menu) {
    if (menu != NULL) {
        if (menu->window != NULL) {
            window_stack_pop(false);
            window_destroy(menu->window);
        }
        free(menu);
    }
}

static void s_window_load(Window *window) {
    TimeSelectWindow *menu = (TimeSelectWindow*)window_get_user_data(window);
    
    Layer *window_layer = window_get_root_layer(window);
    GRect window_frame = layer_get_frame(window_layer);

    menu->layer = layer_create(
        (GRect){
            .origin = {
                MENU_BOUNDS_SIZE_WIDTH + 10,
                10
            },
            .size = {
                window_frame.size.w - (MENU_BOUNDS_SIZE_WIDTH + 10) + 5,
                window_frame.size.h - (10 * 2)
            }
        });
    
    layer_add_child(window_layer, menu->layer);
}

static void s_window_unload(Window *window) {
    TimeSelectWindow *menu = (TimeSelectWindow*)window_get_user_data(window);

    layer_destroy(menu->layer);
    menu->layer = NULL;
}
