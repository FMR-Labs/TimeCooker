#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "TimeCookerDSP.h"

using namespace iplug;
using namespace igraphics;

const int kNumPresets = 1;

class TimeCooker final : public Plugin
{
public:
  TimeCooker(const InstanceInfo& info);
  ~TimeCooker() override = default;

  void OnIdle() override;

#if IPLUG_DSP
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;

private:
  TimeCookerDSP mDSP;
#endif
};
