#include "LED.h"

LED_args::LED_args():Thread_args(){}

LED_args::~LED_args(){}


LED::LED():Tool(){}


bool LED::Initialise(std::string configfile, DataModel &data){

  m_configfile=configfile;
  InitialiseTool(data);
  InitialiseConfiguration(configfile);
  //m_variables.Print();

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

  m_util=new Utilities();
  //args=new LED_args();
  
  //  m_util->CreateThread("test", &Thread, args);

  LoadConfig();
  
  ExportConfiguration();

  
  return true;
}


bool LED::Execute(){

  if(m_data->change_config){
    InitialiseConfiguration(m_configfile);
    LoadConfig();
    ExportConfiguration();    
  }
  
  lapse = period - (boost::posix_time::microsec_clock::universal_time() - last);
  if(lapse.is_negative()){
    last= boost::posix_time::microsec_clock::universal_time();
    
    std::string json="";

    firing_sequence.at(sequence_num).Set("start_time", m_data->current_coarse_counter + 15000000000U );
    firing_sequence.at(sequence_num)>>json;
    printf("Sendinf LED\n");    
    m_data->services->AlertSend("LEDTrigger", json);
    sequence_num++;
  }
  
   

  
  
  return true;
}


bool LED::Finalise(){

  m_util->KillThread(args);

  delete args;
  args=0;

  delete m_util;
  m_util=0;

  return true;
}

void LED::Thread(Thread_args* arg){

  LED_args* args=reinterpret_cast<LED_args*>(arg);

}

bool LED::LoadSequence(int version){

  firing_sequence.clear();
  // m_data->services->GetDeviceConfig(config_json, -1, "MCCAssignments");
  
  std::string test_sequence = "{\"sequence\":[{\"MPMTID\":77,\"LED\":1,\"DACSetting\":750},{\"MPMTID\":20,\"LED\":2,\"DACSetting\":750}, {\"MPMTID\":26,\"LED\":1,\"DACSetting\":750},{\"MPMTID\":104,\"LED\":2,\"DACSetting\":750}, {\"MPMTID\":13,\"LED\":2,\"DACSetting\":750},{\"MPMTID\":85,\"LED\":1,\"DACSetting\":750}, {\"MPMTID\":46,\"LED\":1,\"DACSetting\":750},{\"MPMTID\":41,\"LED\":1,\"DACSetting\":750},{\"MPMTID\":84,\"LED\":1,\"DACSetting\":750}, {\"MPMTID\":3,\"LED\":1,\"DACSetting\":750},{\"MPMTID\":45,\"LED\":1,\"DACSetting\":750}, {\"MPMTID\":14,\"LED\":2,\"DACSetting\":750},{\"MPMTID\":34,\"LED\":2,\"DACSetting\":750}, {\"MPMTID\":52,\"LED\":1,\"DACSetting\":750}]}";
  
  Store sequence_store;
  sequence_store.JsonParser(test_sequence);
  
  std::vector<std::string> triggers;
  
  sequence_store.Get("sequence", triggers);
  
  Store led_trigger;
  
  unsigned int mpmt_ID = 45;
  unsigned int gain= 1;
  unsigned int dac_setting=750;
  unsigned int type = 0;
  unsigned int num_flashes = 10000;
  unsigned int flash_interval = 10000;
  unsigned int start_time = 62500;
  unsigned int led;
  
  led_trigger.Set("MPMTID", mpmt_ID);
  led_trigger.Set("Gain", gain);
  led_trigger.Set("DACSetting", dac_setting);
  led_trigger.Set("Type", type);
  led_trigger.Set("NumFlashes", num_flashes);
  led_trigger.Set("FashInterval", flash_interval);
  led_trigger.Set("StartTime", start_time);
  led_trigger.Set("LED", led);
  
  for(int i=0; i<triggers.size(); i++){
    
    led_trigger.JsonParser(triggers.at(i));
    firing_sequence.push_back(led_trigger);
  }
  
  
  
  return true;
}

void LED::LoadConfig(){

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
  period=boost::posix_time::seconds(300);
  last= boost::posix_time::microsec_clock::universal_time();
  
  LoadSequence();
  sequence_num=0;
  
}
