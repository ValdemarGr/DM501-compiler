//
// Created by MadsG on 11-05-2019.
//

#ifndef HELLO_PARSER_EXTRA_H
#define HELLO_PARSER_EXTRA_H

typedef struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
    char *filename;
} YYLTYPE;

#endif //HELLO_PARSER_EXTRA_H
