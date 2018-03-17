
#include <stdio.h>
#include "../task1/libstat.h"
#include "data.c"

int main( int argc, const char* argv[] )
{
    int w1[3] = {1,2,3}, w2[3] = {6,2,3}, w3[3] = {3,2,3};

    // 0 [size][p*] --> [Char][Char][Char][Char][Char]
    // 2 [size][p*] --> [Char][Char][Char]
    // 4 [size][p*] --> [Char][Char][Char][Char][Char]
    // 6 [size][p*] --> [Char]
    // 8 [0][NaN]
    // 10 [0][NaN]


    int *tab = createArrayOfStringPtrs(10);
    addCharBlock(tab, 10, w1, 3);

    printf("%i", tab[0]);

}
