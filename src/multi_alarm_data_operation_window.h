#pragma once
#include "pebble.h"
#include "multi_alarm_data.h"

typedef struct multi_alarm_data_operation_window MultiAlarmDataOperationWindow;

typedef enum multi_alarm_data_operation_element_id {
    MADOE_Enable = 0,
    MADOE_Edit,
    MADOE_Delete,
    MADOE_Add
    // You have to modify 'MAX_MULTI_ALARM_DATA_OPERATION_ELEMENT' value.
} MultiAlarmDataOperationElementID;
#define MAX_MULTI_ALARM_DATA_OPERATION_ELEMENT  ((int)MADOE_Add + 1)

typedef union multi_alarm_data_operation_element {
    // for MADOE_Enable
    struct {
        bool enable;
    } enable;
    // for MADOE_Edit
    struct {
        MATime time;
    } edit;
    // for MADOE_Delete
    struct {
        int dummy;
    } delete;
    // for MADOE_Add
    struct {
        MATime time;
    } add;
} MultiAlarmDataOperationElement;

typedef void (*MultiAlarmDataOperationCallback)(MultiAlarmDataOperationElementID id, MultiAlarmDataOperationElement element, MultiAlarmData *data, index_t index);

MultiAlarmDataOperationWindow *multi_alarm_data_operation_window_create(void);
void multi_alarm_data_operation_window_destroy(MultiAlarmDataOperationWindow *menu);
bool multi_alarm_data_operation_window_show(MultiAlarmDataOperationWindow *menu, MultiAlarmDataOperationCallback callback, MultiAlarmData *data, index_t index);
bool multi_alarm_data_operation_window_hide(MultiAlarmDataOperationWindow *menu);