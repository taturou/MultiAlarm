#include <pebble.h>
#include "multi_alarm_data.h"
#include "PDUtils.h"

typedef struct multi_alarm_data_element {
    bool usable;
    bool alarm_enable;
    uint8_t hour;
    uint8_t min;
} MADElement;    
    
typedef struct multi_alarm_data {
    MADElement *elements;
    size_t num_element;
    size_t num_usable;
} MultiAlarmData;

static err_t s_data_get_unusable_index(const MultiAlarmData *data, index_t *index);
static MADElement *s_data_get_element(const MultiAlarmData *data, index_t index);
static void s_element_get_time_t_of_today(const MADElement *element, time_t *now, time_t *tim);
static int s_data_compar(const void *d1, const void *d2);

MultiAlarmData *multi_alarmm_data_create(size_t num_max) {
    MultiAlarmData *data = NULL;
    
    data = calloc(sizeof(MultiAlarmData), 1);
    if (data != NULL) {
        data->elements = calloc(sizeof(MADElement), num_max);
        if (data->elements != NULL) {
            data->num_element = num_max;
            data->num_usable = 0;
        } else {
            multi_alarm_data_destory(data);
            data = NULL;
        }
    }    
    return data;
}

void multi_alarm_data_destory(MultiAlarmData *data) {
    if (data != NULL) {
        if (data->elements != NULL) {
            free(data->elements);
        }
        free(data);
    }
}

size_t multi_alarm_data_get_num_max(const MultiAlarmData *data) {
    return data->num_element;
}

size_t multi_alarm_data_get_num_usable(const MultiAlarmData *data) {
    size_t num = 0;
    
    if (data != NULL) {
        num = data->num_usable;
    }
    return num;
}

void multi_alarm_data_sort_by_ascending_order(MultiAlarmData *data) {
    qsort(data->elements, data->num_element, sizeof(MADElement), s_data_compar);
}

err_t multi_alarm_data_add(MultiAlarmData *data, index_t *index) {
    err_t err = -1;
    
    if (s_data_get_unusable_index(data, index) == 0) {
        data->elements[*index].usable = true;
        data->num_usable++;
        err = 0;
    }
    return err;
}

err_t multi_alarm_data_delete(MultiAlarmData *data, index_t index) {
    MADElement *element = s_data_get_element(data, index);
    if (element != NULL) {
        element->usable = false;
        data->num_usable--;
    }
    return element != NULL ? 0 : -1;
}

bool multi_alarm_data_get_usable(const MultiAlarmData *data, index_t index) {
    return data->elements[index].usable;
}

err_t multi_alarm_data_get_time_t_of_today(const MultiAlarmData *data, index_t index, time_t *tim) {
    MADElement *element = s_data_get_element(data, index);
    if (element != NULL) {
        time_t now = 0;
        s_element_get_time_t_of_today(element, &now, tim);
    }
    return element != NULL ? 0 : -1;
}

err_t multi_alarm_data_get_time_t_of_after24h(const MultiAlarmData *data, index_t index, time_t *tim) {
    MADElement *element = s_data_get_element(data, index);
    if (element != NULL) {
        time_t now = 0;
        s_element_get_time_t_of_today(element, &now, tim);
        if (*tim < now) {
            *tim += 24 * 60 * 60; // second of 1day
        }
    }
    return element != NULL ? 0 : -1;
}

err_t multi_alarm_data_get_MATime(const MultiAlarmData *data, index_t index, MATime *tim) {
    MADElement *element = s_data_get_element(data, index);
    if (element != NULL) {
        tim->hour = element->hour;
        tim->min = element->min;
    }
    return element != NULL ? 0 : -1;
}

err_t multi_alarm_data_set_MATime(MultiAlarmData *data, index_t index, MATime tim) {
    MADElement *element = s_data_get_element(data, index);
    if (element != NULL) {
        element->hour = tim.hour;
        element->min = tim.min;
    }
    return element != NULL ? 0 : -1;
}

err_t multi_alarm_data_get_alarm_enable(const MultiAlarmData *data, index_t index, bool *enable) {
    MADElement *element = s_data_get_element(data, index);
    if (element != NULL) {
        *enable = element->alarm_enable;
    }
    return element != NULL ? 0 : -1;
}

err_t multi_alarm_data_set_alarm_enable(MultiAlarmData *data, index_t index, bool enable) {
    MADElement *element = s_data_get_element(data, index);
    if (element != NULL) {
        element->alarm_enable = enable;
    }
    return element != NULL ? 0 : -1;
}

static err_t s_data_get_unusable_index(const MultiAlarmData *data, index_t *index) {
    err_t err = -1;

    for (index_t i = 0; i < data->num_element; i++) {
        if (data->elements[i].usable == false) {
            *index = i;
            err = 0;
            break;
        }
    }
    return err;
}

static MADElement *s_data_get_element(const MultiAlarmData *data, index_t index) {
    MADElement *element = NULL;
    
    if (index < data->num_element) {
        if (data->elements[index].usable == true) {
            element = &data->elements[index];
        }
    }
    return element;
}

static void s_element_get_time_t_of_today(const MADElement *element, time_t *now, time_t *tim) {

    (void)time(now);
    struct tm now_tm;
    memcpy(&now_tm, localtime(now), sizeof(struct tm));

    now_tm.tm_hour = element->hour;
    now_tm.tm_min = element->min;
    now_tm.tm_sec = 0;

    *tim = p_mktime(&now_tm);
}

static int s_data_compar(const void *d1, const void *d2) {
    const MADElement *e1 = (const MADElement*)d1;
    const MADElement *e2 = (const MADElement*)d2;
    time_t t1, t2, now;

    if (e1->usable == true) {
        s_element_get_time_t_of_today(e1, &now, &t1);
    } else {
        t1 = 0x7fffffffUL;
    }
    if (e2->usable == true) {
        s_element_get_time_t_of_today(e2, &now, &t2);
    } else {
        t2 = 0x7fffffffUL;
    }
    return (int)(t1 - t2);
}