#ifndef HELLO_MEMORY_H
#define HELLO_MEMORY_H

void *Malloc(unsigned n);

#define NEW(type) (type *)Malloc(sizeof(type))

#endif