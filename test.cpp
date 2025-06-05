#include <iostream>
#include <vector>
#include <map>
#include "Store.h"

using namespace ToolFramework;

int main(){


  std::map<unsigned int, unsigned int> my_map;
  std::vector<unsigned int> my_vec;
  std::cout<<my_map.max_size()<<std::endl;
  std::cout<<my_vec.max_size()<<std::endl;

 std::string test_sequence = "{\"sequence\":[{\"card_id\":77,\"led_mask\":1,\"dac\":750},{\"card_id\":20,\"led_mask\":2,\"dac\":750}, {\"card_id\":26,\"led_mask\":1,\"dac\":750},{\"card_id\":104,\"led_mask\":2,\"dac\":750}, {\"card_id\":13,\"led_mask\":2,\"dac\":750},{\"card_i\
d\":85,\"led_mask\":1,\"dac\":750}, {\"card_id\":46,\"led_mask\":1,\"dac\":750},{\"card_id\":41,\"led_mask\":1,\"dac\":750},{\"card_id\":84,\"led_mask\":1,\"dac\":750}, {\"card_id\":3,\"led_mask\":1,\"dac\":750},{\"card_id\":45,\"led_mask\":1,\"dac\":750}, {\"card_id\":14,\"led_mask\":2,\"dac\":750},{\"card_id\":34,\"led_mask\":2,\"dac\":750}, {\"card_id\":52,\"led_mask\":1,\"dac\":750}]}";

 Store test;
 test.JsonParser(test_sequence);

 test.Print();
 std::vector<std::string> bob;
  test.Get("sequence", bob);
  for(unsigned int i=0; i<bob.size(); i++){
    printf("%u: %s\n", i , bob.at(i).c_str());
    //      bob.at(i).Print();
    
  }
  
  
  return 0;
}
