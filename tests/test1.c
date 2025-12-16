#define __TEST1__

#include "header_Test1_2.h"

extern int Madd(int a, int b);

int main(void){
    int a = 420, b = 40;
    int res = Madd(a, b); 
    return res;
}
