#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "../FMR_Library/FMR_Lib/FMR_DSP_float.h"
#include "../FMR_Library/FMR_Pedal/IPedal.h"

#include "../FMR_Library/FMR_Pedal/PedalConst.h"

#include <atomic>
#include <memory>
#include <array>

#define ND_TIME_COOKER_NB_CHANNELS 2

enum TeCr_EParams
{
	TeCr_kRate = 0,
	TeCr_kWidth,
	TeCr_kSpeed,
	TeCr_kMix,
	TeCr_kType,
	TeCr_kTremolo,
	TeCr_kMode,
	TeCr_kSync,
	teCr_kFootSw1,
	teCr_kFootSw2,
	TeCr_kNumParams
};

enum TeCr_EControlTags
{
	TeCr_kCtrlLED_1 = 0,
	TeCr_kCtrlLED_2,
	TeCr_kNumLeds
};

template <typename T> class FMR_RateAdapter;

class Chorus;
class Rotary;
class Flanger;
class Phaser;
class Tremolo;

class TimeCookerDSP : public IPedal
{
public:
  struct Parameters
  {
    float rate = 0.f;
	float coeffRate = 0.f;
    unsigned short int width = 2048;
    float speed = 1.0f;
    float MixModDry = 0.5f;
	float MixModWet = 0.5f;
    unsigned short int type = 0;
    float tremolo = 0.25f;
	unsigned char TypeShape = _SINUS_;
    unsigned short int mode = 0;
	unsigned short int sync = 1;
    unsigned short int footSw1 = 1;
    unsigned short int footSw2 = 0;
  };

  TimeCookerDSP();
  ~TimeCookerDSP();

  TimeCookerDSP(const TimeCookerDSP&) = delete;
  TimeCookerDSP& operator=(const TimeCookerDSP&) = delete;

  void Reset(double sampleRate, int blockSize);

  void ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames, int nChans);

  int GetLatency() const;

  // IPedal (utilisé par le board : moteur seul, 48 kHz, en place)
  const char* Id() const override { return "TimeCooker"; }
  
  int NumParams() const override { return TeCr_kNumParams; }
  int NumLeds() const override { return TeCr_kNumLeds; }

  const char* ParamName(int k) const override;
  float ParamDefault(int k) const override;
  void SetParam(int k, float norm) override;
  void ResetEngine() override;
  void Process(iplug::sample** io, int nFrames, int nChans) override
  {
    ProcessEngine(io, io, nFrames, nChans, mParams);
  }

  void SetLed(int led, float color, float value, int decay);
  float GetLedColor(int led);
  float GetLedValue(int led);
  int GetLedDecay(int led);
  
  void setDisplayedLed(bool value) {displayedLed = value;}

private:
  void ProcessEngine(iplug::sample** inputs, iplug::sample** outputs, int nFrames, int nChans, const Parameters& params);
  void levelColorLed(int led, float level, float fullScale, float greenAt, float orangeAt);

  Parameters mParams;
  
  std::unique_ptr<Chorus> mChorus;
  std::unique_ptr<Rotary> mRotary;
  std::unique_ptr<Flanger> mFlanger;
  std::unique_ptr<Phaser> mPhaser;
  std::unique_ptr<Tremolo> mTremolo;

  std::unique_ptr<FMR_RateAdapter<iplug::sample>> mRate;
  
  bool displayedLed = false;

  FilterFloat_t filterRate{};
  FilterFloat_t filterWidth{};
  FilterFloat_t filterMix{};
  FilterFloat_t filterSpeed{};
  FilterFloat_t filterTremolo{};

  float currentRate = 0.0f;
  float currentWidth = 0.0f;
  float currentMix = 0.0f;
  float currentSpeed = 0.0f;
  float currentTremolo = 0.0f;

  LFO_t MylfoMod1;
  LFO_t MylfoMod2;
  
  FilterFloat_t Mylfo1SinFilter;
  FilterFloat_t Mylfo1CosFilter;
  FilterFloat_t Mylfo2SinFilter;
  FilterFloat_t Mylfo2CosFilter;

  LFO_t MylfoVib;
  FilterFloat_t MylfoVibSinFilter_HP;
  FilterFloat_t MylfoVibSinFilter_LP;
  
  struct myLed
  {
      float color = 0.0f;
      float value = 0.0f;
      int decay = 1;
  };
  std::array<myLed, TeCr_kNumLeds> pedalLed;
};
