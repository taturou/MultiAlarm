#include "pebble.h"
#include "PDUtils.h"

static Window *window;
static MenuLayer *menu_layer;

typedef struct hhmm {
    int hour;
    int min;
} HHMM;

typedef struct timer {
    HHMM time;
    bool enable;
} Timer;

static Timer timer[] = {
    {{6, 0}, false},
    {{7, 37}, false},
    {{7, 46}, false},
    {{7, 52}, false},
    {{12, 0}, false},
    {{15, 0}, false},
    {{17, 30}, false},
    {{23, 30}, false},
    {{0, 0}, false},
};

#define NUM_ROWS_OF_SECTION_1       (sizeof(timer) / sizeof(Timer))
#define NUM_ROWS_OF_SECTION_2       (1)

#define CELL_HEIGHT_OF_SECTION_1    (40)
#define CELL_HEIGHT_OF_SECTION_2    (30)

#define VLINE_ORIGIN_X              (57) 

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
    return 2;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    switch (section_index) {
    case 0:
        return NUM_ROWS_OF_SECTION_1;
    case 1:
        return NUM_ROWS_OF_SECTION_2;
    default:
        return 0;
    }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t menu_get_cell_hight_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    uint16_t height = 10;

    switch (cell_index->section) {
    case 0:
        height = CELL_HEIGHT_OF_SECTION_1;
        break;
    case 1:
        height = CELL_HEIGHT_OF_SECTION_2;
    }
    return height;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
    switch (section_index) {
    case 0:
        menu_cell_basic_header_draw(ctx, cell_layer, "Time          About...");
        GRect bounds = layer_get_bounds(cell_layer);
        graphics_draw_line(ctx, (GPoint){VLINE_ORIGIN_X, 0}, (GPoint){VLINE_ORIGIN_X, bounds.size.h});
        break;
    case 1:
        menu_cell_basic_header_draw(ctx, cell_layer, "Setting");
        break;
    }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    char str[16];

    switch (cell_index->section) {
    case 0:
        // enable
        {
            graphics_draw_circle(
                ctx,
                (GPoint){
                    6 + 3,
                    CELL_HEIGHT_OF_SECTION_1 / 2
                },
                5);
            if (timer[cell_index->row].enable == true) {
                graphics_context_set_fill_color(ctx, GColorBlack);
                graphics_fill_circle(
                    ctx,
                    (GPoint){
                        6 + 3,
                        CELL_HEIGHT_OF_SECTION_1 / 2
                    },
                    3);
            }
        }
        
        // time
        {
            snprintf(str, 8, "%2d:%02d", timer[cell_index->row].time.hour, timer[cell_index->row].time.min);
            graphics_context_set_text_color(ctx, GColorBlack);
            graphics_draw_text(
                ctx,
                str,
                fonts_get_system_font(FONT_KEY_GOTHIC_18),
                (GRect){
                    .origin = {
                        0,
                        (CELL_HEIGHT_OF_SECTION_1 / 2) - (18 / 2) - 3,
                    },
                        .size = {
                        VLINE_ORIGIN_X - 5,
                        18
                    }
                },
                GTextOverflowModeWordWrap,
                GTextAlignmentRight,
                NULL);
        }
        
        // about
        {
            // calc
            time_t now_time = time(NULL);
            struct tm ltm;
            
            memcpy(&ltm, localtime(&now_time), sizeof(struct tm));

            ltm.tm_hour = timer[cell_index->row].time.hour;
            ltm.tm_min = timer[cell_index->row].time.min;
            ltm.tm_sec = 0;

             time_t timer_time = p_mktime(&ltm);

            if (timer_time < now_time) {
                timer_time += 60 * 60 * 24;
            }
            
            uint32_t diff_time = (uint32_t)(timer_time - now_time);
            
            int hour = diff_time / (60 * 60);
            int min = (diff_time / 60) % 60;
            int sec = diff_time % 60;

            // display
            snprintf(str, 16, "%2d:%02d:%02d", hour, min, sec);
            graphics_context_set_text_color(ctx, GColorBlack);
            graphics_draw_text(
                ctx,
                str,
                fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
                (GRect){
                    .origin = {
                        VLINE_ORIGIN_X,
                        (CELL_HEIGHT_OF_SECTION_1 / 2) - (28 / 2) - 5,
                    },
                        .size = {
                        layer_get_bounds(cell_layer).size.w - (VLINE_ORIGIN_X + 4),
                        28
                    }
                },
                GTextOverflowModeWordWrap,
                GTextAlignmentRight,
                NULL);
        }            

        // v-line
        {
            GRect bounds = layer_get_bounds(cell_layer);
            graphics_draw_line(ctx, (GPoint){VLINE_ORIGIN_X, 0}, (GPoint){VLINE_ORIGIN_X, bounds.size.h});
        }
        break;
    case 1:
        menu_cell_title_draw(ctx, cell_layer, "New Timer");
        break;
    }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    switch (cell_index->section) {
    case 0:
        timer[cell_index->row].enable = timer[cell_index->row].enable == true ? false : true;
        layer_mark_dirty(menu_layer_get_layer(menu_layer));
        break;
    default:
        break;
    }
}

static void menu_long_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    layer_mark_dirty(menu_layer_get_layer(menu_layer));
}
    
void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);

    menu_layer = menu_layer_create(bounds);
    menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
        .get_num_sections = menu_get_num_sections_callback,
        .get_num_rows = menu_get_num_rows_callback,
        .get_cell_height = menu_get_cell_hight_callback,
        .get_header_height = menu_get_header_height_callback,
        .draw_header = menu_draw_header_callback,
        .draw_row = menu_draw_row_callback,
        .select_click = menu_select_callback,
        .select_long_click = menu_long_select_callback,
    });
    menu_layer_set_click_config_onto_window(menu_layer, window);
    layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
    
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

void window_unload(Window *window) {
    tick_timer_service_unsubscribe();
    menu_layer_destroy(menu_layer);
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