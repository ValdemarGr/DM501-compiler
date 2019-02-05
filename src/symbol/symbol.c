//
// Created by valde on 2/1/19.
//

#include "symbol.h"

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
    htable->next = t;
    return htable;
}

SYMBOL *putSymbol(SymbolTable *t, char *name, int value) {
    //Error stuff
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
            fprintf(stderr, "Error: variable %s already declared in scope", name);
            exit(-1);
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

    //We have our current node we are looking at
    SymbolTable *current_table = t;

    int tableNum = 0;
    //While the table we are looking at is allocated (aka it exists)
    while (current_table != NULL) {
        printf("We are looking at table %i:\n", tableNum);
        //We print our while whole table bucket wise
        for (int i = 0; i < HashSize; i++) {
            //This is the i'th symbol root node
            SYMBOL *current_symbol = (current_table->table)[i];

            if (current_symbol != NULL) {
                printf("At bucket %i we have the values:\n", i);
            }

            //We keep printing the contents of the linked list elements as long as they exist
            while (current_symbol != NULL) {
                //We print the contents of this node
                printf("name: %s, value: %i\n", current_symbol->name, current_symbol->value);

                //We prepare the next level
                current_symbol = current_symbol->next;
            }
        }

        //We proceed to the next table
        current_table = current_table->next;
        tableNum++;
    }
}

