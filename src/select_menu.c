#include <pebble.h>
#include "select_menu.h"

#define NUM_MENU_SECTIONS       (1)
#define NUM_MENU_SECTION1_ROWS  (MAX_SELECT_MENU_ELEMENT)

#define MENU_BOUNDS_SIZE_WIDTH  (50) 

typedef struct select_menu {
    Window *window;
    MenuLayer *layer;
    SelectMenuSelectCallback callback;
    MultiAlarmData *data;
    index_t data_index;
} SelectMenu;

static void s_window_load(Window *window);
static void s_window_unload(Window *window);

SelectMenu *select_menu_create(void) {
    SelectMenu *menu = NULL;

    menu = calloc(sizeof(SelectMenu), 1);
    return menu;
}

void select_menu_destroy(SelectMenu *menu) {
    if (menu != NULL) {
        select_menu_hide(menu);
        free(menu);
    }
}

bool select_menu_show(SelectMenu *menu, SelectMenuSelectCallback callback, MultiAlarmData *data, index_t index) {
    bool ret = false;

    if (menu != NULL) {
        Window *window = window_create();
        if (window != NULL) {
            menu->window = window;
            menu->callback = callback;
            menu->data = data;
            menu->data_index = index;

            // init window
            window_set_background_color(window, GColorClear);
            window_set_fullscreen(window, true);
            window_set_user_data(window, (void*)menu);
            window_set_window_handlers(window, (WindowHandlers) {
                .load = s_window_load,
                .unload = s_window_unload,
            });
            window_stack_push(window, false /* Animated */);
            ret = true;
        }
    }
    return ret;
}

bool select_menu_hide(SelectMenu *menu) {
    bool ret = false;

    if (menu != NULL) {
        if (menu->window != NULL) {
            window_stack_pop(false);
            window_destroy(menu->window);
            menu->window = NULL;
            ret = true;
        }
    }
    return ret;
}

static uint16_t s_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
    return NUM_MENU_SECTIONS;
}

static uint16_t s_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    const uint16_t num_rows[NUM_MENU_SECTIONS] = {
        NUM_MENU_SECTION1_ROWS,
    };
    return num_rows[section_index];
}

static int16_t s_menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    return layer_get_bounds(menu_layer_get_layer(menu_layer)).size.h / NUM_MENU_SECTION1_ROWS;
}

static int16_t s_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return 0;
}

static int16_t s_menu_get_separator_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    return 0;
}

static void s_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    SelectMenu *menu = (SelectMenu*)data;
    struct {
        char *title;
        char *subtitle;
        GBitmap *icon;
    } cell = {NULL, NULL, NULL};
   
    switch ((SelectMenuElementID)cell_index->row) {
    case SME_Enable:
        {
            bool enable = false;
            if (multi_alarm_data_get_alarm_enable(menu->data, menu->data_index, &enable) == 0) {
                if (enable == true) {
                    cell.title = "Disable";
                } else {
                    cell.title = "Enable";                    
                }
            }
        }
        break;
    case SME_Edit:
        cell.title = "Edit";
        break;
    case SME_Delete:
        cell.title = "Delete";
        break;
    default:
        break;
    }
    
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, layer_get_bounds(cell_layer), 0, GCornerNone);
    graphics_draw_rect(ctx, layer_get_bounds(cell_layer));
    menu_cell_basic_draw(ctx, cell_layer, cell.title, cell.subtitle, cell.icon);
}
    
static void s_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    (void)menu_layer;
    SelectMenu *menu = (SelectMenu*)data;
    SelectMenuElement element;

    switch ((SelectMenuElementID)cell_index->row) {
    case SME_Enable:
        if (multi_alarm_data_get_alarm_enable(menu->data, menu->data_index, &element.enable.enable) == 0) {
            element.enable.enable = element.enable.enable == true ? false : true;
        }
        break;
    case SME_Edit:
        element.edit.time.hour = 12;
        element.edit.time.min = 34;
        break;
    case SME_Delete:
        element.delete.dummy = 0;
        break;
    default:
        break;
    }

    (*menu->callback)((SelectMenuElementID)cell_index->row, element, menu->data, menu->data_index);
    select_menu_hide(menu);
}

static void s_window_load(Window *window) {
    SelectMenu *menu = window_get_user_data(window);
    
    Layer *window_layer = window_get_root_layer(window);
    GRect window_frame = layer_get_frame(window_layer);

    menu->layer = menu_layer_create(
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
    menu_layer_set_callbacks(menu->layer, (void*)menu, (MenuLayerCallbacks){
        .get_num_sections = s_menu_get_num_sections_callback,
        .get_num_rows = s_menu_get_num_rows_callback,
        .get_cell_height = s_menu_get_cell_height_callback,
        .get_header_height = s_menu_get_header_height_callback,
        .get_separator_height = s_menu_get_separator_height_callback,
        .draw_row = s_menu_draw_row_callback,
        .select_click = s_menu_select_callback
    });
    scroll_layer_set_shadow_hidden(menu_layer_get_scroll_layer(menu->layer), true);
    menu_layer_set_click_config_onto_window(menu->layer, window);
    layer_add_child(window_layer, menu_layer_get_layer(menu->layer));
}

static void s_window_unload(Window *window) {
    SelectMenu *menu = window_get_user_data(window);

    menu_layer_destroy(menu->layer);
    menu->layer = NULL;
}