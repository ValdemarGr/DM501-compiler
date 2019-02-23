//
// Created by valde on 2/23/19.
//
#include "error.h"

int writeError(Error *e) {
    if (e == NULL) {
        return;
    }

    switch (e->error) {
        case WEED_FUNC_HAS_NO_END:
            printf("Function \"%s\" has no end. ", e->val.WEED_FUNC_HAS_NO_END_S.headId);
            printf("Did you mean %s at line %i?\n",
                   e->val.WEED_FUNC_HAS_NO_END_S.headId,
                   e->val.WEED_FUNC_HAS_NO_END_S.lineno);
            break;
        case WEED_FUNC_HAS_NO_RETURN:
            printf("Function \"%s\" has no return statement\n", e->val.WEED_FUNC_HAS_NO_RETURN_S.tailId);
            break;
        default:
            break;
    }

    return e->error;
}
