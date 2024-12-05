#include "DataModel.h"

DataModel::DataModel():DAQDataModelBase(){}

/*
TTree* DataModel::GetTTree(std::string name){

  return m_trees[name];

}


void DataModel::AddTTree(std::string name,TTree *tree){

  m_trees[name]=tree;

}


void DataModel::DeleteTTree(std::string name,TTree *tree){

  m_trees.erase(name);

}

*/

void*
DataModel::AlertSubscribe(
    const std::string&           alert,
    ToolFramework::AlertFunction function
) {
  auto list = alerts.find(alert);
  if (list == alerts.end()) {
    if (!services)
      throw std::runtime_error("DataModel::AlertSubscribe: services not found");

    auto broker = [this](const char* alert, const char* payload) {
      // XXX: a function can be marked for erasure in another thread
      auto a = alerts.find(alert);
      if (a == alerts.end()) {
        return;
      };
      auto& functions = a->second;
      auto i = functions.begin();
      while (i != functions.end()) {
        auto& f = *i;
        if (f) {
          f(alert, payload);
          ++i;
        } else
          functions.erase(i++);
      };
    };

    if (!services->AlertSubscribe(alert, std::move(broker)))
      throw std::runtime_error(
          "DataModel::AlertSubscribe: could not subscribe to " + alert
      );

    list = alerts.emplace(
        alert,
        std::list<ToolFramework::AlertFunction> {}
    ).first;
  };

  return &*list->second.insert(list->second.end(), std::move(function));
};

void DataModel::AlertUnsubscribe(const std::string& alert, void* handle) {
  // mark for erasure. The entry will be erased when the alert is received.
  *reinterpret_cast<ToolFramework::AlertFunction*>(handle)
    = ToolFramework::AlertFunction();
};
