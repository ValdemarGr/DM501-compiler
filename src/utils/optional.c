//
// Created by valde on 4/28/19.
//

#include "optional.h"

Optional *Some(void *value) {
    Optional *optional = (Optional*)(malloc(sizeof(Optional)));

    optional->state = some;
    optional->value = value;
    optional->isDefined = true;

    return optional;
}

Optional *map(Optional *optional, void *(*transformer)(void*)) {
    if (optional->state == some) {
        return Some(transformer(optional->value));
    } else {
        return optional;
    }
}
