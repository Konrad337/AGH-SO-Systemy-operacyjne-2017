struct array {
    int tabSize;
    int wordSize;
    int sizeUsed;
    int** arr;
};

struct array createArrayOfStringPtrs(int tabSize, int wordSize);

void addintBlock(struct array arr, int* block);

void deinitArray(struct array arr);

int* findClosestElement(struct array arr);
