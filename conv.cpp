#include <iostream>


int main(){

  unsigned int a = 134217728U;
  unsigned int b;
  unsigned long test= ((unsigned long)(a & 4294901760U))<<16;
  //  test= test<<16;
  std::cout<< test<<std::endl;
  
  return 0;

}
