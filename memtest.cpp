#include "sys/types.h"
#include "sys/sysinfo.h"
#include <iostream>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

int main(){

 struct sysinfo memInfo;
 
 sysinfo (&memInfo);
 
 float mem = (((float) memInfo.totalram - (float)memInfo.freeram -(float)memInfo.sharedram - (float) memInfo.bufferram)/((float)memInfo.totalram)) * 100;
 //mem_unit
 unsigned long total = memInfo.totalram;
 unsigned long free = memInfo.freeram;
 unsigned long shared = memInfo.sharedram;
 unsigned long buffer = memInfo.bufferram;
 unsigned long totalswap = memInfo.totalswap;
 unsigned long freeswap = memInfo.freeswap;
 unsigned long totalhigh = memInfo.totalhigh;
 unsigned long freehigh = memInfo.freehigh;
   
 std::cout<<"total="<<total<<
   " free="<<free<<
   " mem="<<mem<<
   " shared="<<shared<<
   " buffer="<<buffer<<
   " totalswap="<<totalswap<<
   " freeswap="<<freeswap<<
   " totalhigh="<<totalhigh<<
   " freehigh="<<freehigh<<std::endl;
 
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file); 

    std::cout<<"result="<<(result*1000)<<std::endl;
    
 return 0;
 
}
