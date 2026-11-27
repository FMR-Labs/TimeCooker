
#include "TimeCookerDSP.h"

#include "../FMR_Library/FMR_Modulation/FMR_Chorus.h"
#include "../FMR_Library/FMR_Modulation/FMR_Rotary.h"
#include "../FMR_Library/FMR_Modulation/FMR_Flanger.h"
#include "../FMR_Library/FMR_Modulation/FMR_Phaser.h"
#include "../FMR_Library/FMR_Modulation/FMR_Tremolo.h"

#include "../FMR_Library/FMR_Lib/FMR_Resampler.h"
#include "../FMR_Library/FMR_Lib/FMR_ConvPot.h"

#include <algorithm>
#include <cmath>

using iplug::sample;

namespace
{
  // Ordre = EParams du VST3 autonome. Noms stables : ne jamais les renommer.
  constexpr const char* kParamNames[] = { "rate", "width", "speed", "mix", "type", "tremolo", "mode", "sync", "footSw1", "footSw2"};

  // Valeurs initiales de TimeCooker.cpp, normalisées 0..1.
  // valeurs redondantes avec Parameters dans TimeCookerDPS.h ! Est-ce que l'on peut se passer de l'un ou de l'autres ?
  constexpr float kParamDefaults[] = { 0.5f, 0.5f, 0.25f, 0.5f, 0, 0.25f, 0, 1, 1, 0 };

  inline float Clamp01(float val) { return std::clamp(val, 0.0f, 1.0f); }
  inline int Steps(float val, int n) { return static_cast<int>(std::lround(Clamp01(val) * n)); }
}

TimeCookerDSP::TimeCookerDSP()
{
  InitLFO(&MylfoMod1, 2.0f, 1.0f);
  InitLFO(&MylfoMod2, 2.0f, 1.0f);
  
  InitFilter(&Mylfo1SinFilter, 5.0, LP_1st_order, 1.0);
  InitFilter(&Mylfo1CosFilter, 5.0, LP_1st_order, 1.0);
  InitFilter(&Mylfo2SinFilter, 5.0, LP_1st_order, 1.0);
  InitFilter(&Mylfo2CosFilter, 5.0, LP_1st_order, 1.0);
  
  MylfoVib.type = _SINUS_;
  InitLFO(&MylfoVib, 2.0f, 1.0f);
  InitFilter(&MylfoVibSinFilter_HP,  0.1f, HP_1st_order, 1.0f);
  InitFilter(&MylfoVibSinFilter_LP, 10.0f, LP_1st_order, 1.0f);

  mChorus  = std::make_unique<Chorus>(&MylfoMod1, &MylfoMod2);
  mRotary  = std::make_unique<Rotary>(&MylfoMod1, &MylfoMod2);
  mFlanger = std::make_unique<Flanger>(&MylfoMod1);
  mPhaser  = std::make_unique<Phaser>(&MylfoMod1);
  mTremolo  = std::make_unique<Tremolo>(&MylfoVib);

  mRate = std::make_unique<FMR_RateAdapter<sample>>();
}

TimeCookerDSP::~TimeCookerDSP() = default;

void TimeCookerDSP::Reset(double sampleRate, int blockSize)
{
  // Le moteur tourne à 48 kHz ; le resampler adapte la fréquence du host.
  mRate->Init(sampleRate, std::max(blockSize, 64));
  ResetEngine();
}

void TimeCookerDSP::ResetEngine()
{

}

int TimeCookerDSP::GetLatency() const
{
  return mRate->Latency();
}

void TimeCookerDSP::SetLed(int led, float color, float value, int decay)
{
    pedalLed[led].color = color;
    pedalLed[led].value = value;
    pedalLed[led].decay = decay;
}

float TimeCookerDSP::GetLedColor(int led)
{
    return pedalLed[led].color;
}

float TimeCookerDSP::GetLedValue(int led)
{
    return pedalLed[led].value;
}

int TimeCookerDSP::GetLedDecay(int led)
{
    return pedalLed[led].decay;
}

const char* TimeCookerDSP::ParamName(int k) const
{
  return (k >= 0 && k < TeCr_kNumParams) ? kParamNames[k] : "";
}

float TimeCookerDSP::ParamDefault(int k) const
{
  return (k >= 0 && k < TeCr_kNumParams) ? kParamDefaults[k] : 0.5f;
}

void TimeCookerDSP::SetParam(int k, float val)
{
  unsigned short int value;
  switch (k)
  {
    case TeCr_kRate:	mParams.rate = 0.1f + 5.9f * PotCurve_0_30[static_cast<unsigned short int>(Steps(val, 4095))];
						mParams.coeffRate = 0.2f + 0.8f * PotCurve_0_10[ 4095 - static_cast<unsigned short int>(Steps(val, 4095)) ]; 	break;
    case TeCr_kWidth:	mParams.width = Steps(val, 4095); break;
    case TeCr_kSpeed:	value = static_cast<unsigned short int>(Steps(val, 4095));
						if ( value < 2048)
						{
							value <<= 1;
							mParams.speed = 0.2f + 9.8f * PotCurve_0_10[ 4095 - value ];
							mParams.TypeShape = _SINUS_;
						}
						else
						{
							value -= 2048;
							value <<= 1;
							mParams.speed = 0.2f + 9.8f * PotCurve_0_10[ value ];
							mParams.TypeShape = _SQUARE_;
						}
						break;
						
    case TeCr_kMix:		mParams.MixModDry = 1.0f - PotCurve_0_30[ static_cast<unsigned short int>(Steps(val, 4095)) ];
						mParams.MixModWet = 1.0f - 0.9f * PotCurve_0_30[ 4095 - static_cast<unsigned short int>(Steps(val, 4095)) ];	break;
    case TeCr_kType:	mParams.type = Steps(val, 3); break;
    case TeCr_kTremolo:	mParams.tremolo = Steps(val, 4095); break;

    case TeCr_kMode:	mParams.mode = Steps(val, 2); break;
    case TeCr_kSync:	mParams.sync = Steps(val, 2); break;
	
    case teCr_kFootSw1:	mParams.footSw1 = Steps(val, 1); break;
    case teCr_kFootSw2:	mParams.footSw2 = Steps(val, 1); break;
    default: break;
  }
}
	
void TimeCookerDSP::ProcessBlock( sample** inputs, sample** outputs, int nFrames, int nChans )
{
  if (mRate->Active())
  {
    // Host -> 48 kHz -> moteur -> host, par tranches <= MaxBlock().
    for (int done = 0; done < nFrames; )
    {
      const int n = std::min(nFrames - done, mRate->MaxBlock());

      sample* in[ND_TIME_COOKER_NB_CHANNELS];
      sample* out[ND_TIME_COOKER_NB_CHANNELS];

      for (int c = 0; c < nChans; c++)
      {
        in[c] = inputs[c] + done;
        out[c] = outputs[c] + done;
      }

      const int n48 = mRate->ToEngine(in, n, nChans);
      ProcessEngine(mRate->Planar(), mRate->Planar(), n48, nChans, mParams);
      mRate->FromEngine(n48, out, n, nChans);

      done += n;
    }
  }
  else
  {
    ProcessEngine(inputs, outputs, nFrames, nChans, mParams);
  }
}

void TimeCookerDSP::levelColorLed(int led, float level, float fullScale, float greenAt, float orangeAt)
{
  
  const float normalized = std::clamp(level / fullScale, 0.0f, 1.0f);
  
  if (normalized < 0.005f)	pedalLed[led].value = 0.0f;	// éteinte
  else					    pedalLed[led].value = 1.0f;	// allumée

  if (normalized < greenAt)			pedalLed[led].color = LedVerte;
  else if (normalized < orangeAt)	pedalLed[led].color = LedOrange;
  else								pedalLed[led].color = LedRouge;

  if( displayedLed )	pedalLed[led].decay = LedDecay;
  else  				pedalLed[led].decay = 0;
}

void TimeCookerDSP::ProcessEngine(sample** inputs, sample** outputs, int nFrames, int nChans, const Parameters& params)
{
  float VoutTremolo, VoutModulation;
  float output;

  for (int s = 0; s < nFrames; s++)
  {
    // Read LFO modulation
    ReadLFO(&MylfoMod1);
    ReadLFO(&MylfoMod2);
    // Filtering LFO Modulation
    MylfoMod1.Sin = Filter1st(&Mylfo1SinFilter, MylfoMod1.Sin);
    MylfoMod1.Cos = Filter1st(&Mylfo1CosFilter, MylfoMod1.Cos);
    MylfoMod2.Sin = Filter1st(&Mylfo2SinFilter, MylfoMod2.Sin);
    MylfoMod2.Cos = Filter1st(&Mylfo2CosFilter, MylfoMod2.Cos);
    // Updating LFO frequencies
    FreqLFO( &MylfoMod1, mParams.rate );
    FreqLFO( &MylfoMod2, mParams.rate * 0.98f );	// LFO2 2% moins vite que LFO1

	switch( mParams.sync )
	{
		case 0 : MylfoVib.Sin =  MylfoMod1.Sin; break;
		default :
		case 1 :
			// Updating LFO frequencies
			FreqLFO( &MylfoVib, mParams.speed );
			// Read LFO Vibrato
			ReadLFO(&MylfoVib);
			break;
		case 2 : MylfoVib.Sin = -MylfoMod1.Sin; break;
	}
	// Filter LFO Vib
	MylfoVib.Sin = Filter1st(&MylfoVibSinFilter_HP, MylfoVib.Sin);
	if ( mParams.TypeShape == _SQUARE_ )
	{
		MylfoVib.Sin *= 30.0f;
		if ( MylfoVib.Sin > 1.0f )  MylfoVib.Sin =  1.0f;
		if ( MylfoVib.Sin < -1.0f ) MylfoVib.Sin = -1.0f;
	}
	// Filter LFO
	MylfoVib.Sin = Filter1st(&MylfoVibSinFilter_LP, MylfoVib.Sin);
	
    for (int c = 0; c < nChans; c++)
    {
      const float input = inputs[c][s];
	  //if( c > 1 )	output = input;
	  //else

      if( params.footSw2 == 1 )
	  {
	    VoutTremolo = mTremolo->Process(c, input, mParams.tremolo);
		SetLed(TeCr_kCtrlLED_2, LedRose, 0.5f + 0.5f * MylfoVib.Sin, 0);
	  }
	  else
	  {
	    VoutTremolo = input;
        SetLed(TeCr_kCtrlLED_2, LedRouge, 0.0f, 0);
      }
	  
	  switch( mParams.type )
	  {
		  case 0 :	// Chorus
			VoutModulation = mChorus->Process(c, VoutTremolo, mParams.width, mParams.coeffRate, mParams.MixModDry, mParams.MixModWet, params.mode);
			break;
		  case 1 :	// Leslie
			VoutModulation = mRotary->Process(c, VoutTremolo, mParams.width, mParams.coeffRate, mParams.MixModDry, mParams.MixModWet, params.mode);
			break;
		  case 2 :	// Flanger
			VoutModulation = mFlanger->Process(c, VoutTremolo, mParams.width, mParams.coeffRate, mParams.MixModDry, mParams.MixModWet, params.mode);
			break;
		  case 3 :	// Phaser
			VoutModulation = mPhaser->Process(c, VoutTremolo, mParams.width, mParams.coeffRate, mParams.MixModDry, mParams.MixModWet, params.mode);
			break;
		  default :
			VoutModulation = VoutTremolo;
			break;
	  }
	  
      if( params.footSw1 == 1 )
	  {
		SetLed(TeCr_kCtrlLED_1, LedRose, 0.5f + 0.5f * MylfoMod1.Sin, 0);
		output = VoutModulation;
	  }
      else
      {
        VoutModulation = VoutTremolo;
        SetLed(TeCr_kCtrlLED_1, LedRouge, 0.0f, 0);
 	  }
	  
	  outputs[c][s] = VoutModulation;

    }
  }
}
