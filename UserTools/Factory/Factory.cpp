#include "Factory.h"
#include "Unity.h"

Tool* Factory(std::string tool) {
Tool* ret=0;

// if (tool=="Type") tool=new Type;
if (tool=="Configuration") ret=new Configuration;
if (tool=="DummyTool") ret=new DummyTool;
if (tool=="Dumper") ret=new Dumper;
if (tool=="FileWriter2") ret=new FileWriter2;
if (tool=="FileWriter") ret=new FileWriter;
if (tool=="HKMPMT") ret=new HKMPMT;
if (tool=="JobManager") ret=new JobManager;
if (tool=="Monitoring") ret=new Monitoring;
if (tool=="MPMT2") ret=new MPMT2;
if (tool=="MPMT3") ret=new MPMT3;
if (tool=="MPMT5") ret=new MPMT5;
if (tool=="MPMTfakeTrigger") ret=new MPMTfakeTrigger;
if (tool=="MPMT") ret=new MPMT;
if (tool=="Nhits") ret=new Nhits;
if (tool=="ProcessData") ret=new ProcessData;
if (tool=="RunControl") ret=new RunControl;
if (tool=="SlaveRunControl") ret=new SlaveRunControl;
if (tool=="Sorting2") ret=new Sorting2;
if (tool=="Trigger2") ret=new Trigger2;
if (tool=="V1290") ret=new V1290;
if (tool=="V1495") ret=new V1495;
if (tool=="V792") ret=new V792;
if (tool=="V812") ret=new V812;
if (tool=="VMEReceive") ret=new VMEReceive;
if (tool=="VMESend") ret=new VMESend;
if (tool=="VMETest") ret=new VMETest;
if (tool=="WindowBuilder2") ret=new WindowBuilder2;
if (tool=="WindowBuilder3") ret=new WindowBuilder3;
  if (tool=="GenerateDummyData") ret=new GenerateDummyData;
return ret;
}
