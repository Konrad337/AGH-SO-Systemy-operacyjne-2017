
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "carray.h"

struct array createArrayOfStringPtrs(int tabSize, int wordSize) {

    struct array newArr;
    newArr.tabSize = tabSize;
    newArr.wordSize = wordSize;
    newArr.arr = (int**) malloc(tabSize*sizeof(int*));
    newArr.sizeUsed = 0;

    for(int i = 0; i < tabSize; i++) {
    newArr.arr[i] = NULL;
    }

    return newArr;
}

void addintBlock(struct array arr, int* block) {

    if(arr.sizeUsed >= arr.tabSize) {
        arr.arr = (int**) realloc(arr.arr, arr.tabSize * 2 * sizeof(int*));
        for(int i = arr.tabSize; i < arr.tabSize*2; i++) {
        arr.arr[i] = NULL;
        }
        arr.tabSize *= 2;
      }
      arr.arr[arr.sizeUsed] = block;
      arr.sizeUsed += 1;
}

void deinitArray(struct array arr) {
     free(arr.arr);
}

int* findClosestElement(struct array arr) {

int sum = 0;
int bestDiff = INT_MAX;
int index = 0;

  for (int i = 0; i < arr.sizeUsed; i++) {
    for (int j = 0; j < arr.wordSize; j++) {
          sum += arr.arr[i][j];
  }
      if(sum - i < bestDiff) {
          bestDiff = sum - i;
          index = i;
      }
  }
return arr.arr[index];
}
