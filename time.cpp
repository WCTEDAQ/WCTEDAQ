#include <iostream>
#include <time.h>
#include <unistd.h>

int main(){

  time_t timer;
  //struct tm y2k = {0};
  //double seconds;

  //y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
  //y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

  
  time(&timer);  /* get current time; same as: timer = time(NULL)  */
  while( true){
    unsigned long tmp= time(nullptr);
    unsigned long tmp2= time(nullptr) * 1000;
    unsigned long tmp3= tmp2/1000;
    
    std::cout<< tmp<<std::endl;
    std::cout<< tmp2<<std::endl;
    std::cout<< tmp3<<std::endl;
    
    sleep(1);
  }
  
    //  seconds = difftime(timer,mktime(&y2k));

    //printf ("%.f seconds since January 1, 2000 in the current timezone", seconds);


  return 0;


}
