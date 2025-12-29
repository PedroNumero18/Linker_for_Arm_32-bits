#ifndef __HEADER_TEST1_2_H_
#define __HEADER_TEST1_2_H_

#ifdef __TEST1__
    typedef struct {
        int a, b
    }couple;
    
    couple initCouple(int num1, int num2){
        couple test1;
        test1.a = num1;
        test1.b = num2;
        return test1;

    }
#endif

#ifdef __TEST2__
    class couple{
        private:
            int a, b
        public:
            couple(int num1, int num2);
            int getA();
            int getB();
    };

    couple::couple(int num1, int num2){
        a = num1;
        b = num2;
    }
    int couple::getA(){
        return a;
    }
    int couple::getB(){
        return b;
    
#endif
#endif