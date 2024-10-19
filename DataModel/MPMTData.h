#ifndef MPMT_DATA_H
#define MPMT_DATA_H

#include <WCTEMPMTHit.h>
#include <WCTEMPMTLED.h>
#include <WCTEMPMTWaveform.h>
#include <WCTEMPMTPPS.h>
#include <HKMPMTHit.h>
#include <HKMPMTPPS.h>

class TriggerInfo;

struct MPMTData{

  unsigned int coarse_counter; //last 6 bits are zero
  std::vector<WCTEMPMTHit> mpmt_hits;
  std::vector<WCTEMPMTLED> mpmt_leds;
  std::vector<WCTEMPMTWaveform> mpmt_waveforms;
  std::vector<WCTEMPMTPPS> mpmt_pps;
  std::vector<WCTEMPMTHit> mpmt_triggers;
  std::vector<HKMPMTHit> hkmpmt_hits;
  std::vector<HKMPMTPPS> hkmpmt_pps;
  std::vector<WCTEMPMTHit> extra_hits;
  std::vector<WCTEMPMTWaveform> extra_waveforms;
  unsigned int cumulative_sum[4194303U];
  std::mutex unmerged_triggers_mtx;
  std::vector<TriggerInfo> unmerged_triggers;
  

};


#endif
