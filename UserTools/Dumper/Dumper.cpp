#include "DataModel.h"

#include "Dumper.h"

template <typename Readout>
static void
dump(Readout& readout, std::mutex& mutex, std::ofstream& stream) {
  Readout r;
  {
    std::lock_guard<std::mutex> lock(mutex);
    readout.swap(r);
  };

  for (auto& buffer : r)
    stream.write(
        reinterpret_cast<char*>(buffer.data()),
        buffer.size() * sizeof(buffer.front())
    );
};

void Dumper::dump() {
  ::dump(m_data->v1290_readout, m_data->v1290_mutex, tdc);
  ::dump(m_data->v792_readout,  m_data->v792_mutex,  qdc);
};

void Dumper::dumper_thread(ToolFramework::Thread_args* args) {
  Thread* thread = static_cast<Thread*>(args);
  Dumper& tool = thread->tool;
  try {
    tool.dump();
  } catch (std::exception& e) {
    *tool.m_log << tool.ML(0) << e.what() << std::endl;
    thread->kill = true;
  };
};

void Dumper::open(std::ofstream& stream, const std::string& var) {
  std::string filename;
  if (!m_variables.Get(var, filename)) filename = var + ".out";
  stream.open(filename, std::ios::binary | std::ios::out);
  if (!stream)
    throw std::runtime_error(
        std::string("Cannot open ")
        + var
        + " output file `"
        + filename
        + "': "
        + strerror(errno)
    );
};

bool Dumper::Initialise(std::string configfile, DataModel& data) {
  try {
    if (configfile != "") m_variables.Initialise(configfile);

    m_data = &data;
    m_log  = m_data->Log;

    if (!m_variables.Get("verbose", m_verbose)) m_verbose = 1;

    open(tdc, "tdc");
    open(qdc, "qdc");

    return true;

  } catch (std::exception& e) {
    if (m_log)
      *m_log << ML(0) << e.what() << std::endl;
    else
      fprintf(stderr, "%s\n", e.what());
    return false;
  };
};

bool Dumper::Execute() {
  if (!tdc.is_open() || !qdc.is_open()) return false;
  try {
    thread.reset(new Thread(*this));
    util.CreateThread("Dumper", &dumper_thread, thread.get());
    return true;
  } catch (std::exception& e) {
    *m_log << ML(0) << e.what() << std::endl;
    return false;
  };
};

bool Dumper::Finalise() {
  try {
    if (thread) {
      util.KillThread(thread.get());
      delete thread.release();
    };
    if (tdc) tdc.close();
    if (qdc) qdc.close();

    return true;
  } catch (std::exception& e) {
    *m_log << ML(0) << e.what() << std::endl;
    return false;
  };
};
