
#include "TimeCookerGUI.h"
#include "IControls.h"
#include "../FMR_Library/FMR_Pedal/PedalFactory.h"

void TimeCookerGUI::Build(iplug::igraphics::IGraphics* pGraphics, float x, float y, int slot, bool displayLed)
{
	using namespace iplug::igraphics;

//	const IBitmap knobBitmap = pGraphics->LoadBitmap(KNOB_FN, 101);
	const IBitmap knobBitmap   = pGraphics->LoadBitmap(KNOB_NUM_FN, 101);
	const IBitmap rotaryBitmap = pGraphics->LoadBitmap(ROTARY_4_FN, 4);
	const IBitmap switchBitmap = pGraphics->LoadBitmap(SWITCH_H_FN, 3);
	const IBitmap footswBitmap = pGraphics->LoadBitmap(FOOTSW_FN, 2);
	const IBitmap boitierBitmap = pGraphics->LoadBitmap(BOITIER_FN, 1);

	const IBitmap fondBitmap = pGraphics->LoadBitmap(TIMECOOKER_FN, 1);

	auto at = [&](const IRECT& r) { return r.GetTranslated(x, y); };

	//  x /  y     
	// 42 / 81		126 / 81	210 / 81
	// 42 / 200		126 / 200	210 / 200
	//				126 / 337
	//				126 / 415

	// 49 / 129		133 / 129	217 / 129
	// 49 / 248		133 / 248	217 / 248
	//				133 / 385
	//				133 / 463

	const IRECT boitier = at(IRECT(0, 0, 266, 535));
	const IRECT Led1 = at(IRECT(0, 320, 92, 418));
	const IRECT Led2 = at(IRECT(174, 320, 266, 418));
	const IRECT faceAvant = at(IRECT(7, 48, 259, 528));

	const IRECT rateKnob = at(IRECT(6, 68, 86, 148));
	const IRECT widthKnob = at(IRECT(93, 68, 173, 148));
	const IRECT speedKnob = at(IRECT(180, 68, 260, 148));

	const IRECT mixKnob = at(IRECT(6, 190, 86, 270));
	const IRECT typeKnob = at(IRECT(93, 190, 173, 270));
	const IRECT tremKnob = at(IRECT(180, 190, 260, 270));

	const IRECT modeSwitch = at(IRECT(93, 277, 173, 357));
	const IRECT syncSwitch = at(IRECT(180, 277, 260, 357));

	const IRECT footSw1 = at(IRECT(0, 414, 92, 512));
	const IRECT footSw2 = at(IRECT(174, 414, 266, 512));
  
	pGraphics->AttachControl(new IBitmapControl(boitier, boitierBitmap));
	pGraphics->AttachControl(new ILEDControl(Led1, LedRouge), slot + TeCr_kCtrlLED_1 + kNumStartLED);
	pGraphics->AttachControl(new ILEDControl(Led2, LedRouge), slot + TeCr_kCtrlLED_2 + kNumStartLED);
	pGraphics->AttachControl(new IBitmapControl(faceAvant, fondBitmap));

	pGraphics->AttachControl(new IBKnobControl(rateKnob, knobBitmap, slot + TeCr_kRate));
	pGraphics->AttachControl(new IBKnobControl(widthKnob, knobBitmap, slot + TeCr_kWidth));
	pGraphics->AttachControl(new IBKnobControl(speedKnob, knobBitmap, slot + TeCr_kSpeed));

	pGraphics->AttachControl(new IBKnobControl(mixKnob, knobBitmap, slot + TeCr_kMix));
	pGraphics->AttachControl(new IBKnobControl(typeKnob, rotaryBitmap, slot + TeCr_kType));
	pGraphics->AttachControl(new IBKnobControl(tremKnob, knobBitmap, slot + TeCr_kTremolo));

	pGraphics->AttachControl(new IBKnobControl(modeSwitch, switchBitmap, slot + TeCr_kMode, EDirection::Horizontal));
	pGraphics->AttachControl(new IBKnobControl(syncSwitch, switchBitmap, slot + TeCr_kSync, EDirection::Horizontal));

	pGraphics->AttachControl(new IBSwitchControl(footSw1, footswBitmap, slot + teCr_kFootSw1));
	pGraphics->AttachControl(new IBSwitchControl(footSw2, footswBitmap, slot + teCr_kFootSw2));
}

PEDAL_FACTORY(TimeCooker, TimeCookerDSP, TimeCookerGUI)