//
// Created by Mads Grau Kristensen on 2019-03-25.
//

#ifndef HELLO_AMS_CODE_GEN_H
#define HELLO_AMS_CODE_GEN_H

#include "consts.h"

char *getUidLocation(int identifier);

void generateLoadIdentifier(FILE *out, int identifier);

void generateAdd(FILE *out, Add add);

void generateValue(FILE *out, Value *value);


#endif //HELLO_AMS_CODE_GEN_H
