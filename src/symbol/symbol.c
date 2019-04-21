//
// Created by valde on 2/1/19.
//

#include "symbol.h"
#include "../ast/tree.h"

int Hash(char *str){
    //If garbage is given
    if (str == NULL) {
        return -1;
    }

    size_t len = strlen(str);

    //If no chars, no need to run though
    if (len == 0) {
        return -1;
    }

    int sum = (int)str[0];

    //Start at 1, we initial the sum for first character above
    for (int i = 1; i < len; i++) {
        //Shift
        sum = sum << 1;
        //Add to sum
        sum = sum + (int)str[i];
    }

    return sum % HashSize;
}

SymbolTable *initSymbolTable() {
    //Create the initial table
    SymbolTable *table = NEW(SymbolTable);

    //Set next to null
    table->next = NULL;

    table->nextSymbolId = 0;
    table->distanceFromRoot = 0;

    //Set all entries to NULL
    for (int i = 0; i < HashSize; i++) {
        (table->table)[i] = NULL;
    }

    return table;
}

SymbolTable *scopeSymbolTable(SymbolTable *t) {
    //Error handling
    if (t == NULL) {
        return NULL;
    }

    //New table and put old table in new tables next
    SymbolTable* htable = initSymbolTable();
    htable->distanceFromRoot = t->distanceFromRoot + 1;
    htable->next = t;

    if (maxDistFromRoot < htable->distanceFromRoot) {
        maxDistFromRoot = htable->distanceFromRoot;
    }

    return htable;
}

/*SYMBOL *putSymbol(SymbolTable *t, char *name, int value) {
    //Error decorateFunction
    if (t == NULL || name == NULL) {
        return NULL;
    }

    //First we find the hash position
    int pos = Hash(name);

    //Then we find table root node of our desired item hash
    SYMBOL* current_node = t->table[pos];
    SYMBOL* parent_node = NULL;

    //Traverse until we find empty next
    while (current_node != NULL) {
        //Abort if we encounter a duplicate
        if (strcmp(name, current_node->name) == 0) {
            fprintf(stderr, "\nVariable declared in same scope %s same scope\n", name);
            fflush(stderr);
            return NULL;
        }
        
        parent_node = current_node;
        current_node = current_node->next;
    }

    //Node has been found, init it
    current_node = NEW(SYMBOL);
    current_node->next = NULL;

    //Allocate new space for the name (the given name is an R-value
    current_node->name = (char*)malloc(sizeof(char) * strlen(name));
    strcpy(current_node->name, name);

    current_node->value = value;
    
    //If parent node is not NULL we have to adjust the parent's next
    if (parent_node != NULL) {
        parent_node->next = current_node;
    } else {
        t->table[pos] = current_node;
    }

    //Node created and inited, we are done
    return current_node;
}*/

SYMBOL *putSymbol(SymbolTable *t, char *name, struct Value *value, int symbol_stmDeclNum, bool isConst) {
    //Error decorateFunction
    if (t == NULL || name == NULL) {
        return NULL;
    }

    //First we find the hash position
    int pos = Hash(name);

    //Then we find table root node of our desired item hash
    SYMBOL* current_node = t->table[pos];
    SYMBOL* parent_node = NULL;

    //Traverse until we find empty next
    while (current_node != NULL) {
        //Abort if we encounter a duplicate
        if (strcmp(name, current_node->name) == 0) {
            fprintf(stderr, "\nVariable declared in same scope %s\n", name);
            fflush(stderr);
            return NULL;
        }

        parent_node = current_node;
        current_node = current_node->next;
    }

    //Node has been found, init it
    current_node = NEW(SYMBOL);
    current_node->next = NULL;

    //Allocate new space for the name (the given name is an R-value
    current_node->name = (char*)malloc(sizeof(char) * strlen(name));
    strcpy(current_node->name, name);

    current_node->value = value;
    current_node->symbol_stmDeclNum = symbol_stmDeclNum;
    current_node->uniqueIdForScope = t->nextSymbolId;
    t->nextSymbolId++;
    current_node->distanceFromRoot = t->distanceFromRoot;
    current_node->isConst = isConst;

    //If parent node is not NULL we have to adjust the parent's next
    if (parent_node != NULL) {
        parent_node->next = current_node;
    } else {
        t->table[pos] = current_node;
    }

    //Node created and inited, we are done
    return current_node;
}

SYMBOL *getSymbol(SymbolTable *t, char *name) {
    //Error checking
    if (t == NULL || name == NULL) {
        return NULL;
    }

    //We hash name to find a matching bucket
    int bucketIndex = Hash(name);

    //Get the bucket
    SYMBOL *bucket = (t->table)[bucketIndex];

    //If bucket is NULL, we continue looking in next table
    if (bucket == NULL) {
        //We check if this is the root node, if yes we return NULL
        if (t->next == NULL) {
            return NULL;
        } else {
            //Recurse if we still have parents to look in
            return getSymbol(t->next, name);
        }
    }

    //We traverse the linked list for a match if a bucket is found
    while (strcmp(name, bucket->name) != 0) {
        //If the bucket is unallocated, we cannot find what we seek in this scope
        //Thus we must continue in the next scope
        if (bucket->next == NULL) {
            break;
        }

        //If there is not a match we continue looking in the linked list
        bucket = bucket->next;
    }

    //A bit of boilerplate, but if the name is not found in this scope, we look upwards again
    //We check if this is the root node, if yes we return NULL
    if (strcmp(name, bucket->name) == 0) {
        return bucket;
    } else if (t->next == NULL) {
        return NULL;
    } else {
        //Recurse if we still have parents to look in
        return getSymbol(t->next, name);
    }
}

void dumpSymbolTable(SymbolTable *t) {
    //Error check
    if (t == NULL) {
        return;
    }

    printf("Dumping symbol table\n");

    //We have our current node we are looking at
    SymbolTable *current_table = t;

    int tableNum = 0;
    //While the table we are looking at is allocated (aka it exists)
    while (current_table != NULL) {
        //For formatting purposes we find the longest name and type (since this can be user defined)
        size_t longestName = 0;
        size_t longestType = 0;

        for (int i = 0; i < HashSize; i++) {
            SYMBOL *current_symbol = (current_table->table)[i];

            //We keep printing the contents of the linked list elements as long as they exist
            while (current_symbol != NULL) {

                size_t tmpNameLen = strlen(current_symbol->name);
                size_t tmptypeLen = 0;

                switch (current_symbol->value->kind) {
                    case typeK:
                        tmptypeLen = strlen(typeToString(current_symbol->value->val.typeD.tpe));
                        break;
                    case typeFunctionK:
                        tmptypeLen = strlen(typeToString(current_symbol->value->val.typeFunctionD.returnType));
                        break;
                    default:
                        break;
                }

                tmptypeLen = tmpNameLen;

                //For generic
                if (current_symbol->value->kind == typeK) {
                    if (current_symbol->value->val.typeD.tpe->kind == typeGenericK) {
                        if (current_symbol->value->val.typeD.tpe->val.typeGeneric.subType != NULL) {
                            tmptypeLen = tmpNameLen + strlen(current_symbol->value->val.typeD.tpe->val.typeGeneric.subType) + strlen(": ");
                        }
                    }
                }

                if (tmpNameLen > longestName) longestName = tmpNameLen;
                if (tmptypeLen > longestType) longestType = tmptypeLen;

                //We prepare the next level
                current_symbol = current_symbol->next;
            }
        }

        size_t nameTabs = longestName / 8 + 1;
        size_t typeTabs = longestType / 8 + 1;

        printf("We are looking at table level %i:\n", tableNum);
        printf("\tname");

        for (int j = 0; j < nameTabs; j++) {
            printf("\t");
        }

        printf("mapsBy\ttypeId");

        for (int j = 0; j < typeTabs; j++) {
            printf("\t");
        }

        printf("bucket\targuments");

        printf("\n");

        //We print our while whole table bucket wise
        for (int i = 0; i < HashSize; i++) {
            //This is the i'th symbol root node
            SYMBOL *current_symbol = (current_table->table)[i];

            //We keep printing the contents of the linked list elements as long as they exist
            while (current_symbol != NULL) {

                size_t nameLen;
                size_t nameTabsBetween;

                size_t typeLen;
                size_t typeTabsBetween;
                //We print the contents of this node
                switch (current_symbol->value->kind) {
                    case typeK:
                        nameLen = strlen(current_symbol->name);

                        nameTabsBetween = nameTabs - nameLen / 8;

                        typeLen = strlen(typeToString(current_symbol->value->val.typeD.tpe));

                        typeTabsBetween = typeTabs - typeLen / 8;

                        printf("\t\033[0;31m%s", current_symbol->name);

                        for (int j = 0; j < nameTabsBetween; j++) {
                            printf("\t");
                        }

                        //For the arrow
                        printf(" : \t");
                        printf("\033[0;36m");

                        printf("%s", typeToString(current_symbol->value->val.typeD.tpe));

                        if (current_symbol->value->val.typeD.tpe->kind == typeGenericK) {
                            if (current_symbol->value->val.typeD.tpe->val.typeGeneric.subType != NULL) {
                                printf(": %s", current_symbol->value->val.typeD.tpe->val.typeGeneric.subType);
                                typeTabsBetween = typeTabsBetween -
                                        (strlen(current_symbol->value->val.typeD.tpe->val.typeGeneric.subType) +
                                         strlen("generic: ")) / 8;
                            } else {
                                printf(": %s", current_symbol->value->val.typeD.tpe->val.typeGeneric.genericName);
                                typeTabsBetween = typeTabsBetween -
                                                  (strlen(current_symbol->value->val.typeD.tpe->val.typeGeneric.genericName) +
                                                  strlen("generic: ")) / 8;
                            }
                        }

                        for (int j = 0; j < typeTabsBetween; j++) {
                            printf("\t");
                        }

                        printf("\033[0;32m%i\033[0m\n", i);

                        break;
                    case typeFunctionK:
                        nameLen = strlen(current_symbol->name);

                        nameTabsBetween = nameTabs - nameLen / 8;

                        typeLen = strlen(typeToString(current_symbol->value->val.typeFunctionD.returnType));

                        typeTabsBetween = typeTabs - typeLen / 8;

                        printf("\t\033[0;31m%s", current_symbol->name);

                        for (int j = 0; j < nameTabsBetween; j++) {
                            printf("\t");
                        }

                        //For the arrow
                        printf(" -> \t");
                        printf("\033[0;36m%s", typeToString(current_symbol->value->val.typeFunctionD.returnType));

                        for (int j = 0; j < typeTabsBetween; j++) {
                            printf("\t");
                        }

                        printf("\033[0;32m%i\t", i);

                        //Arguments
                        VarDelList *vdl = current_symbol->value->val.typeFunctionD.tpe;

                        if (vdl != NULL) {
                            printf("(");

                            while (vdl != NULL) {
                                printf("\033[0;36m%s", typeToString(vdl->type));

                                if (vdl->next != NULL) {
                                    printf("\033[0m, ");
                                }

                                vdl = vdl->next;
                            }

                            printf("\033[0m)");
                        }

                        printf("\033[0m\n");
                        break;
                    case symTypeClassK:
                        nameLen = strlen(current_symbol->name);

                        nameTabsBetween = nameTabs - nameLen / 8;

                        typeLen = strlen("class");

                        typeTabsBetween = typeTabs - typeLen / 8;

                        printf("\t\033[0;31m%s", current_symbol->name);

                        for (int j = 0; j < nameTabsBetween; j++) {
                            printf("\t");
                        }

                        //For the arrow
                        printf(" {} \t");
                        printf("\033[0;36m%s", "class");

                        for (int j = 0; j < typeTabsBetween; j++) {
                            printf("\t");
                        }

                        printf("\033[0;32m%i\t", i);

                        printf("\033[0m\n");
                        break;
                    default:
                        printf("ERROR IN DUMP\n");
                        break;
                }

                //We prepare the next level
                current_symbol = current_symbol->next;
            }
        }

        //We proceed to the next table
        current_table = current_table->next;
        tableNum++;
    }
}

