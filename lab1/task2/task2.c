
#include <stdio.h>
#include "../task1/carray.h"
#include "data.h"

int main( int argc, const char* argv[] )
{
    int w1[3] = {1,2,3}, w2[3] = {6,2,3}, w3[3] = {3,2,3};

    // 0 [size][p*] --> [Char][Char][Char][Char][Char]
    // 2 [size][p*] --> [Char][Char][Char]
    // 4 [size][p*] --> [Char][Char][Char][Char][Char]
    // 6 [size][p*] --> [Char]
    // 8 [0][NaN]
    // 10 [0][NaN]


    struct array tab = createArrayOfStringPtrs(10, 3);
    addCharBlock(tab, w1);


    printf("%i", data[0][0]);

}
