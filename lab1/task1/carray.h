#ifndef CARRAY_H_   /* Include guard */
#define CARRAY_H_


struct array {
    int tabSize;
    int wordSize;
    int sizeUsed;
    int** arr;
};

struct array createArrayOfStringPtrs(int tabSize, int wordSize);

void addCharBlock(struct array arr, int* block);

void deinitArray(struct array arr);

int* findClosestElement(struct array arr);

void removeCharBlock(struct array arr);

#endif
