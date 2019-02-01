//
// Created by valde on 2/1/19.
//

#include "symbol.h"

int Hash(char *str){
    //If garbage is given
    if (str == NULL) {
        return -1;
    }

    int len = strlen(str);

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
    SymbolTable* htable = NEW(SymbolTable);
    htable->next = t;
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
        parent_node = current_node;
        current_node = current_node->next;
    }

    //Node has been found, init it
    current_node = NEW(SYMBOL);
    current_node->next = NULL;
    current_node->name = name;
    current_node->value = value;
    
    //If parent node is not NULL we have to adjust the parent's next
    if (parent_node != NULL) {
        parent_node = current_node;
    }

    //Node created and inited, we are done
    return current_node;
}

SYMBOL *getSymbol(SymbolTable *t, char *name) {
    //Error checking
    if (t == NULL || name == NULL) {
        return NULL;
    }

    //Which table we are currently looking at
    SymbolTable *current_table = t;

    //We hash name to find a matching bucket
    int bucketIndex = Hash(name);

    //We use a found variable for code clearness
    short found = 0;

    while (found == 0) {
        //Get the bucket
        SYMBOL *bucket = (current_table->table)[bucketIndex];

        //We traverse the linked list for a match
        while (strcmp(name, bucket->name) != 0) {
            //If the bucket is unallocated, we cannot find what we seek in this scope
            //Thus we must continue in the next scope
            if (bucket == NULL) {
                break;
            }

            //If there is not a match we continue looking in the linked list
            bucket = bucket->next;
        }

        //Here we actually check if we found an entry
        if (bucket == NULL) {
            //We also check if the next table even exists
            if (current_table->next == NULL) {
                //If it doesn't exist, we are done. No result found
                return NULL;
            } else {
                //There is a next table, so we continue in the next scope.
                current_table = current_table->next;
                continue;
            }
        } else {
            //We actually found a bucket, so we return it!
            return bucket;
        }
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
            printf("At bucket %i we have the values:\n", i);

            //This is the i'th symbol root node
            SYMBOL *current_symbol = (current_table->table)[i];

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

