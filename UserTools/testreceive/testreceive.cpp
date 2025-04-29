#include "testreceive.h"

testreceive::testreceive():Tool(){}


bool testreceive::Initialise(std::string configfile, DataModel &data){
  
  InitialiseTool(data);
  InitialiseConfiguration(configfile);
  //m_variables.Print();

  //your code here


  ExportConfiguration();

  sleep(10);

  utils=new DAQUtilities(m_data->context);

  sock=new zmq::socket_t(*(m_data->context), ZMQ_ROUTER);
  utils->UpdateConnections("XMPMT", sock, connections, "4444");
  utils->UpdateConnections("XMPMT", sock, connections, "4445");
  utils->UpdateConnections("XMPMT", sock, connections, "4446");
  utils->UpdateConnections("XMPMT", sock, connections, "4447");


  for( std::map<std::string,Store*>::iterator it=connections.begin(); it!=connections.end(); it++){

    std::cout<<it->first<<std::endl;

  } 
  /*
  for( std::map<std::string,Store*>::iterator it=connections2.begin(); it!=connections2.end(); it++){
    
    std::cout<<it->first<<std::endl;
    
  }
  */
  return true;
}


bool testreceive::Execute(){

  unsigned long counter=0;
  boost::posix_time::ptime time=boost::posix_time::microsec_clock::universal_time();
  boost::posix_time::time_duration duration;
  //  boost::posix_time::microsec_clock::universal_time();
  
  while(true){
    if( counter % 10000 ==0 ){
      duration= (boost::posix_time::microsec_clock::universal_time()-time);
      std::cout<<duration<<std::endl;
      time=boost::posix_time::microsec_clock::universal_time();
    }
    zmq::message_t msg;
    sock->recv(&msg);
    counter++;
  }
  
  return true;
}


bool testreceive::Finalise(){

  return true;
}
