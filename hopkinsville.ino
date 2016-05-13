/*
Hopkinsville A midi toy for controlling the DSI Prophet 08 

@author Richard Hoar <richard.hoar@streeme.com>
@see https://github.com/chaffneue/hopkinsville

License
---

The MIT License (MIT)

Copyright (c) 2015 Richard Hoar <richard.hoar@streeme.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */
#define ARDUINO_AVR_NANO 1
#include <DirectIO.h>

#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>

#define _TASK_MICRO_RES
#include <TaskScheduler.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include "LcdNavigator.h"
#include "Hopkinsville.h"

Task downbeatFlash(1, 1, &downbeatFlashCallback, &scheduler);
Task downbeatFlashComplete(DOWNBEAT_LAMP_FLASH_OFF, 2, &downbeatFlashCompleteCallback, &scheduler);
Task encoderDebounce(ENCODER_DEBOUNCE_TIME, 2, &encoderDebounceCallback, &scheduler, true);
Task updateEditable(UPDATE_EDITABLE_TIME, 2, &updateEditableCallback, &scheduler, false);
Task clearEditable(UPDATE_EDITABLE_TIME, 2, &clearEditableCallback, &scheduler, true);
Task nextEditable(NAV_POLL_TIME, -1, &nextEditableCallback, &scheduler, true);
Task nextEditableDebounce(NAV_POLL_TIME, -1, &nextEditableDebounceCallback, &scheduler, false);
Task previousEditable(NAV_POLL_TIME, -1, &previousEditableCallback, &scheduler, true);
Task previousEditableDebounce(NAV_POLL_TIME, -1, &previousEditableDebounceCallback, &scheduler, false);
Task updateNoteList(NOTE_LIST_REDRAW_TIME, 8, &updateNoteListCallback, &scheduler, true);

NavigationItem root(0, 0, 11, 0, clearTwoSpaces, &navigation, &notePrinterCallback);
NavigationItem rootRange(0, 2, 9, 5, clearOneSpace, &navigation, &rangePrinterCallback);
NavigationItem rootMode(0, 3, 6, 0, clearOneSpace, &navigation, &modePrinterCallback);
NavigationItem arpeggiatorModeStep1(0, 4, 4, 0, clearOneSpace, &navigation, &arpeggiatorModePrinterCallback);
NavigationItem arpeggiatorDegreeStep1(0, 5, 19, 1, clearTwoSpaces, &navigation, &modeDegreePrinterCallback);
NavigationItem arpeggiatorModeStep2(0, 7, 4, 0, clearOneSpace, &navigation, &arpeggiatorModePrinterCallback);
NavigationItem arpeggiatorDegreeStep2(0, 8, 19, 0, clearTwoSpaces, &navigation, &modeDegreePrinterCallback);
NavigationItem arpeggiatorModeStep3(0, 10, 4, 0, clearOneSpace, &navigation, &arpeggiatorModePrinterCallback);
NavigationItem arpeggiatorDegreeStep3(0, 11, 19, 0, clearTwoSpaces, &navigation, &modeDegreePrinterCallback);
NavigationItem arpeggiatorModeStep4(0, 13, 4, 0, clearOneSpace, &navigation, &arpeggiatorModePrinterCallback);
NavigationItem arpeggiatorDegreeStep4(0, 14, 19, 0, clearTwoSpaces, &navigation, &modeDegreePrinterCallback);

/** Print a note name given a value
 *  @param value - the current value
 */
void notePrinterCallback(uint8_t value) {
  lcd.print(noteName[value]);
  rootNote = value;
  updateNoteList.restart();
}

/** Print a mode name given a value
 *  @param value - the current value
 */
void modePrinterCallback(uint8_t value) {
  lcd.print(modeName[value]);
  currentMode = value;
  updateNoteList.restart();
}

/** Print an arpeggiator mode
 *  @param value - the current value
 */
void arpeggiatorModePrinterCallback(uint8_t value) {
  lcd.write(value);
}

/** Print an arpeggiator mode
 *  @param value - the current value
 */
void rangePrinterCallback(uint8_t value) {
  lcd.print(value);
}


void modeDegreePrinterCallback(uint8_t value) {
  if(value == 0) {
    lcd.print("--");
  } else if(value < 8) {
    lcd.print(value);
  } else if(value >= 8  && value < 16){
    lcd.print(value - 8);
    lcd.print(7);
  } else { 
    lcd.print(noteName[value - 16]);
  }
}

/** Indicate that clocks are being received by flashing on the downbeat
 */
void downbeatFlashCallback() {
  auditionLampPin = HIGH;
  downbeatFlashComplete.restart();
}

/** Clear the downbeat lamp
 */
void downbeatFlashCompleteCallback() {
  if(!downbeatFlashComplete.isFirstIteration()) {
    auditionLampPin = LOW;
  }
}

/** Debounce the encoder
 */
void encoderDebounceCallback() {
  if(!encoderDebounce.isFirstIteration()) {
    attachInterrupt(interruptPin, handleEncoder, FALLING);
  }
}

/** Update the current editable area
 */
void updateEditableCallback() {
  if(!updateEditable.isFirstIteration()) {
    navigation.updateEditable();
    clearEditable.restart();
    updateEditable.disable();
  }
}

/** Clear the current editable area
 */
void clearEditableCallback() {
  if(!clearEditable.isFirstIteration()) {
    navigation.clearEditable();
    updateEditable.restart();
    clearEditable.disable();
  }
}

/** Go to the next nav item 
 */
void nextEditableCallback() {
  if(nextPin == LOW) {
    updateEditable.disable();
    clearEditable.disable();
    navigation.updateEditable();
    navigation.selectNextEditable();
    updateEditable.restart();
    nextEditableDebounce.restart();
    nextEditable.disable();
  }
}

/** Debounce the next nav item button
 */
void nextEditableDebounceCallback() {
  if(nextPin == HIGH) {
    nextEditable.restart();
    nextEditableDebounce.disable();
  }
}

/** Go to the previous nav item 
 */
void previousEditableCallback() {
  if(previousPin == LOW) {
    updateEditable.disable();
    clearEditable.disable();
    navigation.updateEditable();
    navigation.selectPreviousEditable();
    updateEditable.restart();
    previousEditableDebounce.restart();
    previousEditable.disable();
  }
}

/** Debounce the previous nav item button
 */
void previousEditableDebounceCallback() {
  if(previousPin == HIGH) {
    previousEditable.restart();
    previousEditableDebounce.disable();
  }
}

/** Update the valid note list on the editor screen
 */
void updateNoteListCallback() {
  relativePitch = 0;
  
  if(!updateNoteList.isFirstIteration()) { 
    for (i = 0; i < updateNoteList.getRunCounter() - 1; i++) {
      relativePitch +=  modeFormula[(i + currentMode) % 7];
    }
  }
  
  lcd.setCursor((updateNoteList.getRunCounter()) * 2, 1);
  lcd.print(clearTwoSpaces);
  lcd.setCursor((updateNoteList.getRunCounter()) * 2, 1);
  lcd.print(noteName[(rootNote + relativePitch) % 12]);
}

/** Clock handler
 */
void handleMidiClock() {
  midi1.sendRealTime(midi::Clock);
  if(clocks == 24) {
    clocks = 1;
  } else {
    if(clocks == 1){
      downbeatFlash.restart();
      quarterNotes++;
    }
    
    clocks++;
  }

  if(quarterNotes == 32) {
    quarterNotes = 0;
  }
}

/** Set a listener for the encoder
 */
void handleEncoder() {
  if(encoderClkPin == LOW) {
    if(encoderDtPin == LOW) {
      navigation.incrementEditableValue();
    } else {
      navigation.decrementEditableValue();
    }
    detachInterrupt(interruptPin);
    encoderDebounce.restart();
  }
}

void setup() {
  initMidi();
  initLcd();
  navigation.addLcdInstance(lcd);
  navigation.refresh();
  scheduler.startNow();
}

void loop() {
  midi1.read();
  scheduler.execute();
}
