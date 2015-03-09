#pragma once
#include "pebble.h"
#include "multi_alarm_data.h"

typedef struct select_menu SelectMenu;

typedef enum select_menu_element {
    SME_Enable = 0,
    SME_Edit,
    SME_Delete,
    SME_Add
    // You have to modify 'MAX_SELECT_MENU_ELEMENT' value.
} SelectMenuElementID;
#define MAX_SELECT_MENU_ELEMENT  ((int)SME_Add + 1)

typedef union select_menu_data {
    struct {
        bool enable;
    } enable;
    struct {
        MATime time;
    } edit;
    struct {
        int dummy;
    } delete;
    struct {
        MATime time;
    } add;
} SelectMenuElement;

typedef void (*SelectMenuSelectCallback)(SelectMenuElementID id, SelectMenuElement element, MultiAlarmData *data, index_t index);

SelectMenu *select_menu_create(void);
void select_menu_destroy(SelectMenu *menu);
bool select_menu_show(SelectMenu *menu, SelectMenuSelectCallback callback, MultiAlarmData *data, index_t index);
bool select_menu_hide(SelectMenu *menu);