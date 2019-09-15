/*
   Test script for bass buttons

*/

#include "BassBoard.h"

//#define PIN_DEBUG

BassBoard bassBoard(BassBoard::TREBLE_TESTING); // BASS_TESTING
void setup() {
  Serial.begin(115200);
  Serial.println("BassBoard initialization");
  //bassBoard.setAnalogReading(true);
  bassBoard.init(); // also sets pins  
#ifdef PIN_DEBUG
  bassBoard.activateColumn(4);
#endif
}


void loop() {
#ifndef PIN_DEBUG
  bassBoard.readButtons();
  bassBoard.printStateIfChanged();
#else
  Serial.println(bassBoard.readRow(4));
#endif
  delay(50);
}
