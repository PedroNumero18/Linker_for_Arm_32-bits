#define __TEST2__

#include "header_Test1_2.h"

extern "C" int Madd(int a, int b);

int main(){
    couple test(420, 40);
    int res = Madd(test.getA(), test.getB());
    return res;
}
