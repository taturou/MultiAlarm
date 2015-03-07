#pragma once
#include "pebble.h"

typedef struct select_menu SelectMenu;

typedef enum select_menu_element {
    SME_Disable = 0,
    SME_Edit,
    SME_Delete
    // You have to modify 'MAX_SELECT_MENU_ELEMENT' value.
} SelectMenuElement;
#define MAX_SELECT_MENU_ELEMENT  ((int)SME_Delete + 1)

typedef void (*SelectMenuSelectCallback)(SelectMenuElement element);

SelectMenu *select_menu_create(void);
void select_menu_destroy(SelectMenu *menu);
bool select_menu_show(SelectMenu *menu, SelectMenuSelectCallback callback);
bool select_menu_hide(SelectMenu *menu);