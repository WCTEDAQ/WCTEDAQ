#include <boost/date_time/posix_time/posix_time.hpp>

#include "SlaveRunControl.h"
#include "DataModel.h"

bool SlaveRunControl::Initialise(std::string configfile, DataModel& data) {
  InitialiseTool(data);
  InitialiseConfiguration(std::move(configfile));

  if (!m_data->services) {
    *m_log << ML(0) << "SlaveRunControl requires backend services" << std::endl;
    return false;
  };

  if (!m_variables.Get("verbose", m_verbose)) m_verbose = 1;

  m_run_start = m_data->run_start;
  m_run_stop  = m_data->run_stop;

  m_data->services->AlertSubscribe(
      "RunStart",
      [this](const char* alert, const char* payload) {
        ToolFramework::Store store;
        store.JsonParser(payload);
        m_start_time = boost::posix_time::time_from_string(
            "1970-01-01 00:00:00.000"
        ) + boost::posix_time::seconds(store.Get<unsigned long>("Timestamp"));
        m_run_start = true;
      }
  );

  m_data->services->AlertSubscribe(
      "RunStop",
      [this](const char* alert, const char* payload) {
        m_run_stop = true;
      }
  );

  m_data->services->AlertSubscribe(
      "ChangeConfig",
      [this](const char* alert, const char* payload) {
        ToolFramework::Store store;
        store.JsonParser(payload);
        m_data->run_configuration = store.Get<unsigned int>("RunConfig");
        m_data->load_config = true; // reset by the Configuration tool
      }
  );

  ExportConfiguration();
  return true;
};

bool SlaveRunControl::Execute() {
  if (m_run_start) {
    if (m_data->run_start) {
      m_data->run_start = false;
      m_run_start = false;
    } else if (boost::posix_time::microsec_clock::universal_time() >= m_start_time) {
      m_data->start_time = m_start_time;
      m_data->run_start  = true;
    };
  };

  if (m_run_stop) {
    if (m_data->run_stop) {
      m_data->run_stop = false;
      m_run_stop = false;
    } else {
      m_data->run_stop = true;
    };
  };

  return true;
};
