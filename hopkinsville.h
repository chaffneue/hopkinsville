/*
Hopkinsville A midi toy for controlling the DSI Prophet 08 

@author Richard Hoar <richard.hoar@streeme.com>
@see https://github.com/chaffneue/hopkinsville
@see hopkinsville.ino for changelog

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
#ifndef Hopkinsville_h
  #define Hopkinsville_h
  #define calulateQuarterNoteTime(bpm) (floor((1/((float)bpm / 60) * 4000000)/4))
  #define calulateMidiClockTime(quarterNoteTime) (floor(quarterNoteTime/24));
  #define ENCODER_CLK_PIN 3
  #define ENCODER_DT_PIN 4
  #define ENCODER_SW_PIN 5
  #define AUDITION_LAMP_PIN 8 
  #define WRITE_LAMP_PIN 9
  #define NEXT_PIN 7
  #define PREVIOUS_PIN 6
  #define WRITE_PIN 11
  #define AUDITION_PIN 10
  #define DOWNBEAT_LAMP_FLASH_OFF 10000
  #define DISPLAY_POLL_TIME 150000
  #define ENCODER_DEBOUNCE_TIME 9000
  #define UPDATE_EDITABLE_TIME 140000
  #define NAV_POLL_TIME 30000
  #define NOTE_LIST_REDRAW_TIME 20000
  #define DEFAULT_NOTE_NUMBER 69

  #define LCD_CHAR_ROOT_INVERTED 0
  #define LCD_CHAR_DRONE_INVERTED 1
  #define LCD_CHAR_UP_ARROW_INVERTED 2
  #define LCD_CHAR_DOWN_ARROW_INVERTED 3
  #define LCD_CHAR_UP_DOWN_ARROW_INVERTED 4
  #define LCD_CHAR_OCTAVE 5
  #define LCD_CHAR_BANK 6  
  #define NOTE_A 69
  #define NOTE_Bb 70
  #define NOTE_B 71
  #define NOTE_C 72
  #define NOTE_Db 73
  #define NOTE_D 74
  #define NOTE_Eb 75
  #define NOTE_E 76
  #define NOTE_F 77
  #define NOTE_Gb 78
  #define NOTE_G 79
  #define NOTE_Ab 80

  //Channel for program NRPN data
  #define PROGRAM_CHANNEL_NRPN 1

  //Arpeggiator Direction messages
  #define DIRECTION_NRPN_A 97
  #define DIRECTION_NRPN_B 297
  #define DIRECTION_UP 0
  #define DIRECTION_DOWN 1
  #define DIRECTION_UPDOWN 2

  //Apreggiator On/Off
  #define TOGGLE_ARPEGGIATOR_NRPN_A 100
  #define TOGGLE_ARPEGGIATOR_NRPN_B 300
  #define ARPEGGIATOR_OFF 0
  #define ARPEGGIATOR_ON 1

  //Gated Sequencer On/Off
  #define TOGGLE_GATED_SEQUENCER_NRPN_A 101
  #define TOGGLE_GATED_SEQUENCER_NRPN_B 301
  #define GATED_SEQUENCER_OFF 0
  #define GATED_SEQUENCER_ON 1

  //Clock Mode NRPN
  #define CLOCK_MODE_NRPN 388
  #define CLOCK_CHANNEL_NRPN 6
  #define CLOCK_EXTERNAL 1
  #define CLOCK_INTERNAL 3

  //Note Names
  #define NOTE_CHAR_A "A"
  #define NOTE_CHAR_Bb "Bb"
  #define NOTE_CHAR_B "B"
  #define NOTE_CHAR_C "C"
  #define NOTE_CHAR_Db "Db"
  #define NOTE_CHAR_D "D"
  #define NOTE_CHAR_Eb "Eb"
  #define NOTE_CHAR_E "E"
  #define NOTE_CHAR_F "F"
  #define NOTE_CHAR_Gb "Gb"
  #define NOTE_CHAR_G "G"
  #define NOTE_CHAR_Ab "Ab"

  //Mode Names
  #define MODE_CHAR_MAJOR "M"
  #define MODE_CHAR_MINOR "m"
  #define MODE_CHAR_DORIAN "D"
  #define MODE_CHAR_PHRYGIAN "P"
  #define MODE_CHAR_LYDIAN "L"
  #define MODE_CHAR_MIXOLYDIAN "X"
  #define MODE_CHAR_LOCRIAN "O"  

  Scheduler scheduler;
  Navigation navigation;
  MIDI_CREATE_INSTANCE(HardwareSerial, Serial,  midi1);
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  Input<ENCODER_CLK_PIN> encoderClkPin(false);
  Input<ENCODER_DT_PIN> encoderDtPin(false);
  Input<ENCODER_SW_PIN> encoderSwPin;
  Output<AUDITION_LAMP_PIN> auditionLampPin;
  Output<WRITE_LAMP_PIN> writeLampPin;
  Input<NEXT_PIN> nextPin;
  Input<PREVIOUS_PIN> previousPin;
  Input<WRITE_PIN> writePin;
  Input<AUDITION_PIN> auditionPin;
  int interruptPin = digitalPinToInterrupt(ENCODER_CLK_PIN);
  
  //forward declarations
  void handleMidiClock();
  void downbeatFlashCallback();
  void downbeatFlashCompleteCallback();
  void updateDisplayCallback();
  void encoderDebounceCallback();
  void updateEditableCallback();
  void clearEditableCallback();
  void nextEditableCallback();
  void nextEditableDebounceCallback();
  void previousEditableCallback();
  void previousEditableDebounceCallback();
  void updateNoteListCallback();

  void notePrinterCallback(int value);
  void modePrinterCallback(int value);
  void arpeggiatorModePrinterCallback(int value);
  void rangePrinterCallback(int value);
  void modeDegreePrinterCallback(int value);
  
  int clocks = 1;
  int quarterNotes = 0;
  char clearOneSpace[] = " "; 
  char clearTwoSpaces[] = "  ";
  const char* noteName[] = {NOTE_CHAR_C, NOTE_CHAR_Db, NOTE_CHAR_D, NOTE_CHAR_Eb, NOTE_CHAR_E, NOTE_CHAR_F, NOTE_CHAR_Gb, NOTE_CHAR_G, NOTE_CHAR_Ab, NOTE_CHAR_A, NOTE_CHAR_Bb, NOTE_CHAR_B};
  const char* modeName[] = {MODE_CHAR_MAJOR, MODE_CHAR_DORIAN, MODE_CHAR_PHRYGIAN, MODE_CHAR_LYDIAN, MODE_CHAR_MIXOLYDIAN, MODE_CHAR_MINOR, MODE_CHAR_LOCRIAN};
  const int modeFormula[] = {0, 2, 4, 5, 7, 9, 11};

  int i = 0;
  int currentNote = 0;
  int currentMode = 0;
  
  //LCD Icons
  byte rootInverted[8] = {
    B11111,
    B10011,
    B10101,
    B10101,
    B10011,
    B10101,
    B10101,
    B11111
  };
  byte droneInverted[8] = {
    B11111,
    B10011,
    B10101,
    B10101,
    B10101,
    B10101,
    B10011,
    B11111
  };
  byte upArrowInverted[8] = {
    B11111,
    B11011,
    B10001,
    B01010,
    B11011,
    B11011,
    B11011,
    B11111
  };
  byte downArrowInverted[8] = {
    B11111,
    B11011,
    B11011,
    B11011,
    B01010,
    B10001,
    B11011,
    B11111
  };
  byte upDownArrowInverted[8] = {
    B11111,
    B10111,
    B10111,
    B01010,
    B01010,
    B11101,
    B11101,
    B11111
  };
  byte octave[8] = {
    B11111,
    B10001,
    B10101,
    B10101,
    B10101,
    B10101,
    B10001,
    B11111
  };
  byte bank[8] = {
    B11111,
    B10101,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111    
  };
  /** Midi Start Handler
   */
  void handleMidiStart() {
    quarterNotes = 0;
    clocks = 1;
    midi1.sendRealTime(midi::Start);
  }
  
  /** Midi Stop Handler
   */
  void handleMidiStop() {
    midi1.sendRealTime(midi::Stop);
  }
  
  /** Midi Continue Handler
   */
  void handleMidiContinue() {
    midi1.sendRealTime(midi::Continue);
  }

  /** Start the MIDI interfaces
   */
  void initMidi() {
    midi1.begin(1);
    midi1.turnThruOff();
    midi1.setHandleClock(&handleMidiClock);
    midi1.setHandleStart(&handleMidiStart);
    midi1.setHandleStop(&handleMidiStop);
    midi1.setHandleContinue(&handleMidiContinue);
  }

  /** Initialize the i2c display
   */
  void initLcd() {
    lcd.begin();
    lcd.backlight();
    lcd.createChar(LCD_CHAR_ROOT_INVERTED, rootInverted);
    lcd.createChar(LCD_CHAR_DRONE_INVERTED, droneInverted);
    lcd.createChar(LCD_CHAR_UP_ARROW_INVERTED, upArrowInverted);
    lcd.createChar(LCD_CHAR_DOWN_ARROW_INVERTED, downArrowInverted);
    lcd.createChar(LCD_CHAR_UP_DOWN_ARROW_INVERTED, upDownArrowInverted);
    lcd.createChar(LCD_CHAR_OCTAVE, octave);
    lcd.createChar(LCD_CHAR_BANK, bank);
    lcd.home();
  }
#endif
