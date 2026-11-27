
#include "TimeCooker.h"

#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

#include "TimeCookerGUI.h"
#include "../FMR_Library/FMR_Lib/FMR_ConvPot.h"

#include <algorithm>

// utiliser struct Parameters et TimeCookerDSP::SetParam pour ne pas dupliquer les infos ?
TimeCooker::TimeCooker(const InstanceInfo& info) : iplug::Plugin(info, MakeConfig(TeCr_kNumParams, kNumPresets))
{
  GetParam(TeCr_kRate)->InitInt("Rate", 2048, 0, 4095);
  GetParam(TeCr_kWidth)->InitInt("Width", 2048, 0, 4095);
  GetParam(TeCr_kSpeed)->InitInt("Speed", 1024, 0, 4095);
  
  GetParam(TeCr_kMix)->InitInt("Mix", 2048, 0, 4095);
  GetParam(TeCr_kType)->InitInt("Type", 0, 0, 3);
  GetParam(TeCr_kTremolo)->InitInt("Tremolo", 1024, 0, 4095);
  
  GetParam(TeCr_kMode)->InitInt("Mode", 0, 0, 2);
  GetParam(TeCr_kSync)->InitInt("Sync", 1, 0, 2);

  GetParam(teCr_kFootSw1)->InitInt("Foot Switch 1", 1, 0, 1);
  GetParam(teCr_kFootSw2)->InitInt("Foot Switch 2", 0, 0, 1);
  
#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]()
  {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };

  mLayoutFunc = [&](IGraphics* pGraphics)
  {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    TimeCookerGUI::Build(pGraphics, 0.f, 0.f, 0, true);
	mDSP.setDisplayedLed(true);
  };
#endif
}

void TimeCooker::OnIdle()
{
#if IPLUG_EDITOR
  if (!GetUI()) return;
  
  for(int l = 0; l < TeCr_kNumLeds ; l++ )
  {
    if ( auto* led = dynamic_cast<ILEDControl*>( GetUI()->GetControlWithTag( kNumStartLED + l) ) )
	{
      float value = GetBypassed() ? 0.0f : mDSP.GetLedValue(l);
      led->SetValue(value);
      if (value != 0.0f)
      {
          led->TriggerWithDecay(mDSP.GetLedDecay(l));
      }
      led->SetHue(mDSP.GetLedColor(l));
      led->SetDirty(true);
    }
  }

#endif
}

#if IPLUG_DSP

void TimeCooker::OnReset()
{
  mDSP.Reset(GetSampleRate(), std::max(GetBlockSize(), 64));
  SetLatency(mDSP.GetLatency());
}

void TimeCooker::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = std::min(NOutChansConnected(),ND_TIME_COOKER_NB_CHANNELS);

  // Conversion unique : TimeCookerDSP::SetParam (partagée avec le board).
  // lecture de tous les knobs et conversion (mise à l'échelle)
  for (int k = 0; k < mDSP.NumParams(); k++)
    mDSP.SetParam(k, static_cast<float>(GetParam(k)->GetNormalized()));

  mDSP.ProcessBlock(inputs, outputs, nFrames, nChans);  
}

#endif
