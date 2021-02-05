#include "LedControl.hpp"

#include <Mpc.hpp>

#include <controls/Controls.hpp>

#include <sequencer/Sequencer.hpp>

#include <string>

using namespace std;

LedControl::LedControl(mpc::Mpc& _mpc, Image& _ledGreen, Image& _ledRed)
: mpc (_mpc), ledGreen (_ledGreen), ledRed (_ledRed)
{
	int x, y;
	int ledSize = 10;

	x = 874;
	y = 216;

	sixteenLevels = Rectangle<float>(x, y, ledSize, ledSize);
	x = 791;
	y = 298;
	nextSeq = Rectangle<float>(x, y, ledSize, ledSize);
	x = 875;
	y = 298;
	trackMute = Rectangle<float>(x, y, ledSize, ledSize);
	x = 103;
	y = 601;
	after = Rectangle<float>(x, y, ledSize, ledSize);
	x = 226;
	y = 686;
	undoSeq = Rectangle<float>(x, y, ledSize, ledSize);
	x = 214;
	y = 833;
	rec = Rectangle<float>(x, y, ledSize, ledSize);
	x = 294;
	y = 833;
	overDub = Rectangle<float>(x, y, ledSize, ledSize);
	x = 451;
	y = 830;
	play = Rectangle<float>(x, y, ledSize, ledSize);
	x = 791;
	y = 216;
	fullLevel = Rectangle<float>(x, y, ledSize, ledSize);
	x = 958;
	y = 298;
	padBankA = Rectangle<float>(x, y, ledSize, ledSize);
	x = 1041;
	y = 298;
	padBankB = Rectangle<float>(x, y, ledSize, ledSize);
	x = 1124;
	y = 297;
	padBankC = Rectangle<float>(x, y, ledSize, ledSize);
	x = 1206;
	y = 296;
	padBankD = Rectangle<float>(x, y, ledSize, ledSize);

	fullLevelLed = new Led(ledGreen, fullLevel);
	sixteenLevelsLed = new Led(ledGreen, sixteenLevels);
	nextSeqLed = new Led(ledGreen, nextSeq);
	trackMuteLed = new Led(ledGreen, trackMute);
	padBankALed = new Led(ledGreen, padBankA);
	padBankBLed = new Led(ledGreen, padBankB);
	padBankCLed = new Led(ledGreen, padBankC);
	padBankDLed = new Led(ledGreen, padBankD);
	afterLed = new Led(ledGreen, after);
	undoSeqLed = new Led(ledRed, undoSeq);
	recLed = new Led(ledRed, rec);
	overDubLed = new Led(ledRed, overDub);
	playLed = new Led(ledGreen, play);
}

void LedControl::addAndMakeVisible(AudioProcessorEditor* editor) {
	editor->addAndMakeVisible(padBankALed);
	editor->addAndMakeVisible(fullLevelLed);
	editor->addAndMakeVisible(sixteenLevelsLed);
	editor->addAndMakeVisible(nextSeqLed);
	editor->addAndMakeVisible(trackMuteLed);
	editor->addAndMakeVisible(padBankALed);
	editor->addAndMakeVisible(padBankBLed);
	editor->addAndMakeVisible(padBankCLed);
	editor->addAndMakeVisible(padBankDLed);
	editor->addAndMakeVisible(afterLed);
	editor->addAndMakeVisible(undoSeqLed);
	editor->addAndMakeVisible(recLed);
	editor->addAndMakeVisible(overDubLed);
	editor->addAndMakeVisible(playLed);
}

void LedControl::setTransform(AffineTransform transform) {
	fullLevelLed->setTransform(transform);
	sixteenLevelsLed->setTransform(transform);
	nextSeqLed->setTransform(transform);
	trackMuteLed->setTransform(transform);
	padBankALed->setTransform(transform);
	padBankBLed->setTransform(transform);
	padBankCLed->setTransform(transform);
	padBankDLed->setTransform(transform);
	afterLed->setTransform(transform);
	undoSeqLed->setTransform(transform);
	recLed->setTransform(transform);
	overDubLed->setTransform(transform);
	playLed->setTransform(transform);
}

void LedControl::setBounds() {
	fullLevelLed->setBounds();
	sixteenLevelsLed->setBounds();
	nextSeqLed->setBounds();
	trackMuteLed->setBounds();
	padBankALed->setBounds();
	padBankBLed->setBounds();
	padBankCLed->setBounds();
	padBankDLed->setBounds();
	afterLed->setBounds();
	undoSeqLed->setBounds();
	recLed->setBounds();
	overDubLed->setBounds();
	playLed->setBounds();
}


void LedControl::setPadBankA(bool b)
{
    padBankALed->setOn(b);
}

void LedControl::setPadBankB(bool b)
{
    padBankBLed->setOn(b);
}

void LedControl::setPadBankC(bool b)
{
    padBankCLed->setOn(b);
}

void LedControl::setPadBankD(bool b)
{
    padBankDLed->setOn(b);
}

void LedControl::setFullLevel(bool b)
{
    fullLevelLed->setOn(b);
}

void LedControl::setSixteenLevels(bool b)
{
    sixteenLevelsLed->setOn(b);
}

void LedControl::setNextSeq(bool b)
{
    nextSeqLed->setOn(b);
}

void LedControl::setTrackMute(bool b)
{
    trackMuteLed->setOn(b);
}

void LedControl::setAfter(bool b)
{
    afterLed->setOn(b);
}

void LedControl::setRec(bool b)
{
    recLed->setOn(b);
}

void LedControl::setOverDub(bool b)
{
    overDubLed->setOn(b);
}

void LedControl::setPlay(bool b)
{
    playLed->setOn(b);
}

void LedControl::setUndoSeq(bool b)
{
    undoSeqLed->setOn(b);
}

void LedControl::timerCallback()
{
    setOverDub(mpc.getControls().lock()->isOverDubPressed() || mpc.getSequencer().lock()->isOverDubbing());
    setRec(mpc.getControls().lock()->isRecPressed() || mpc.getSequencer().lock()->isRecording());
}

void LedControl::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s.compare("full-level-on") == 0) {
		setFullLevel(true);
	}
	else if(s.compare("full-level-off") == 0) {
		setFullLevel(false);
	}
	else if (s.compare("sixteen-levels-on") == 0) {
		setSixteenLevels(true);
	}
	else if (s.compare("sixteen-levels-off") == 0) {
		setSixteenLevels(false);
	}
	else if (s.compare("next-seq-on") == 0) {
		setNextSeq(true);
	}
	else if (s.compare("next-seq-off") == 0) {
		setNextSeq(false);
	}
	else if (s.compare("track-mute-on") == 0) {
		setTrackMute(true);
	}
	else if (s.compare("track-mute-off") == 0) {
		setTrackMute(false);
	}
	else if (s.compare("pad-bank-a-on") == 0) {
		setPadBankA(true);
	}
	else if (s.compare("pad-bank-a-off") == 0) {
		setPadBankA(false);
	}
	else if (s.compare("pad-bank-b-on") == 0) {
		setPadBankB(true);
	}
	else if (s.compare("pad-bank-b-off") == 0) {
		setPadBankB(false);
	}
	else if (s.compare("pad-bank-c-on") == 0) {
		setPadBankC(true);
	}
	else if (s.compare("pad-bank-c-off") == 0) {
		setPadBankC(false);
	}
	else if (s.compare("pad-bank-d-on") == 0) {
		setPadBankD(true);
	}
	else if (s.compare("pad-bank-d-off") == 0) {
		setPadBankD(false);
	}
	else if (s.compare("after-on") == 0) {
		setAfter(true);
	}
	else if (s.compare("after-off") == 0) {
		setAfter(false);
	}
	else if (s.compare("undo-seq-on") == 0) {
		setUndoSeq(true);
	}
	else if (s.compare("undo-seq-off") == 0) {
		setUndoSeq(false);
	}
	else if (s.compare("rec-on") == 0) {
		setRec(true);
	}
	else if (s.compare("rec-off") == 0) {
		setRec(false);
	}
	else if (s.compare("overdub-on") == 0) {
		setOverDub(true);
	}
	else if (s.compare("overdub-off") == 0) {
		setOverDub(false);
	}
	else if (s.compare("play-on") == 0) {
		setPlay(true);
	}
	else if (s.compare("play-off") == 0) {
		setPlay(false);
	}
}

LedControl::~LedControl() {
	delete fullLevelLed;
	delete sixteenLevelsLed;
	delete nextSeqLed;
	delete trackMuteLed;
	delete padBankALed;
	delete padBankBLed;
	delete padBankCLed;
	delete padBankDLed;
	delete afterLed;
	delete undoSeqLed;
	delete recLed;
	delete overDubLed;
	delete playLed;
}
