#include <iostream>
#include <mutex>
#include <BinaryStream.h>
#include <Utilities.h>
#include <WCTERawData.h>

using namespace ToolFramework;

int main(){

  BinaryStream bs;
  bs.Bopen("/mnt/nfs/WebServer/html-Detector/Data/dataR631S0P0.dat", READ, UNCOMPRESSED);

  WCTERawData tmp;

  bs >>tmp;

  tmp.Print();

  
  bs.Bclose();

  

}
