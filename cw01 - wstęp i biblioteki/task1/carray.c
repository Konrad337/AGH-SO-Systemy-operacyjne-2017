
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "carray.h"
#include <stdio.h>

#define ARRAY_SIZE 1000
#define BLOCK_SIZE 10


#ifdef STATIC


void addCharBlock(struct array* arr, int* block) {

    if(arr->sizeUsed >= arr->tabSize) {
        return;
      }

      for(int i = 0; i < BLOCK_SIZE; i++) {
          arr->arr[arr->sizeUsed][i] = block[i];
      }

      arr->sizeUsed += 1;
}

void removeCharBlock(struct array* arr) {
      if(arr->sizeUsed <= 0)
          return;
      arr->sizeUsed -= 1;

}

struct array createArrayOfStringPtrs(int tabSize, int wordSize) {

    //printf("STATIC!\n\n");

    struct array arr;
    arr.tabSize = ARRAY_SIZE;
    arr.wordSize = BLOCK_SIZE;
    arr.sizeUsed = 0;

    return arr;
}

void deinitArray(struct array* arr) {
//nope
}


#else


struct array createArrayOfStringPtrs(int tabSize, int wordSize) {

    //printf("DYNAMIC!\n");


    if(tabSize < 1) tabSize = 1;
    struct array newArr;
    newArr.tabSize = tabSize;
    newArr.wordSize = wordSize;
    newArr.arr = (int**) malloc(tabSize*sizeof(int*));
    newArr.sizeUsed = 0;

/*
    for(int i = 0; i < tabSize; i++) {
    newArr.arr[i] = (int*)malloc(wordSize*sizeof(int));
    }
*/
    return newArr;
}

void addCharBlock(struct array* arr, int* block) {

    if(arr->sizeUsed >= arr->tabSize - 1) {
        arr->arr = (int**) realloc(arr->arr, arr->tabSize * 2 * sizeof(int*));
        /*
        for(int i = arr->tabSize; i < arr->tabSize*2; i++) {
        arr->arr[i] = NULL;
        }
        */
        arr->tabSize *= 2;
      }
      arr->arr[arr->sizeUsed] = block;
      arr->sizeUsed += 1;
}

void deinitArray(struct array* arr) {
     free(arr->arr);
}

void removeCharBlock(struct array* arr) {

    if(arr->sizeUsed <= 0)
        return;
      free(arr->arr[arr->sizeUsed]);
      arr->sizeUsed -= 1;

}

#endif



int* findClosestElement(struct array* arr) {

int sum = 0;
int bestDiff = INT_MAX;
int index = 0;

  for (int i = 0; i < arr->sizeUsed; i++) {
    for (int j = 0; j < arr->wordSize; j++) {
          sum += arr->arr[i][j];
  }
      if(sum - i < bestDiff) {
          bestDiff = sum - i;
          index = i;
      }
  }
return arr->arr[index];
}

int* findClosestElementWithValue(struct array* arr, int value) {
    int sum = 0;
    int bestDiff = INT_MAX;
    int index = 0;

      for (int i = 0; i < arr->sizeUsed; i++) {
        for (int j = 0; j < arr->wordSize; j++) {
              sum += arr->arr[i][j];
      }
          if(sum - value < bestDiff) {
              bestDiff = sum - value;
              index = i;
          }
      }
    return arr->arr[index];
}
