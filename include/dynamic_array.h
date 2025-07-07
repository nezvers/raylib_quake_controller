#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "stdlib.h"

#define DynamicArray(type)  \
    struct {                \
        type *data;         \
        int count;          \
        int capacity;       \
    }

#define da_append(_da, _item){                                                  \
    if ((_da).count >= (_da).capacity){                                         \
        unsigned new_capacity = ((_da).capacity == 0) ? 4 : (_da).capacity * 2; \
        (typeof(*(_da).data)*) new_data = realloc((_da).data, (_da).capacity * sizeof(*(_da).data));\
        if (new_data != NULL){(_da).data = new_data; (_da).capacity = new_capacity;((typeof(*(_da).data)*)((_da).data))[(_da).count++] = _item;}\
    }                                                                           \
    else{((typeof(*(_da).data)*)((_da).data))[(_da).count++] = _item;}\
                    \
}

#define da_free(_da){       \
    free((_da).data);       \
    (_da).count = 0;        \
    (_da).capacity = 0;     \
}


/*
int main(){
    DynamicArray(int) int_array = {};
    append(int_array, 1);
    append(int_array, 2);
    append(int_array, 3);
    for (int i = 0; i < int_array.count; i++){
        printf("%d ", int_array.data[i]);
    }
    return 0;
}
*/

#endif // DYNAMIC_ARRAY_H