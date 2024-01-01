 /*
 * Arduino Nano
 * First, wooden accordion: v3.1, old bootloader
 * Newer accordions: Nano (e.g. 33 IoT), default boot loader
 *
 * From Dec 2023: we use Native MIDI-over-USB instead of Midi-over-Serial-over-USB
 * 
 *
 * Pin assignments since Aug 2022
 * Analog A0..A7:
 * A0..A5: treble (digital I/O)
 * A6: MojcaMode (analog input, cannot change to digital)
 * A7: free, can only be used as analog input  
 * 
 * Digital D0..D13:
 * D0, D1: reserved for serial communications
 * D2: Direction switch
 * D3, D4: bass input (digital I/O)
 * D5, D6, D7, D8, D9, D10, D11, D12: select bank (digital I/O)
 * D13: onboard LED
 *
 * Banks: high if active
 * Bass side:
 * red lines six banks
 * white lines read signals, connect to inputs
 * 
 * SamplerBox (Raspberry Pi): To mount the boot partition: "sudo mount /dev/mmcblk0p1 /boot"
 * To restart the service: systemctl restart samplerbox.service
 */


 #include "MidiNotes.h"
 #include "InstrumentPatches.h"
 #include "AccordionLeds.h"

 #include "MIDIUSB.h"

 

// if true then a simplistic test program will run
//#define MIDI_MESSAGE_TEST_PROGRAM // repeatedly plays and stops C4 note
//#define SHOW_BUTTONS_TEST_PROGRAM
//#define SINGLE_BANK_TEST_PROGRAM
//#define OUTPUT_TEST_PROGRAM // in MIDI mode

// true to send notes to Raspberry PI, false to send to console, both at 115200


const bool MIDI = true; // set to false for debugging
const bool NATIVE_MIDI = true; // if true then native MIDI is used

bool bassInverted = false;
bool trebleInverted = false; // If true then a "1" means note off, "0" is note on. Cherry treble: false. Optocouplers: true. Auto-detect.

int notePushedCount[256] = {0}; // to handle multiple buttons for one note

#define BANK_COUNT 8
#define TREBLE_INPUT_NR_COUNT 6
#define BASS_INPUT_NR_COUNT 2

#define NOTE_ON_VELOCITY 100
#define NOTE_OFF_VELOCITY 127

#define DIRECTION_BUTTON_PIN 2 // was 10
#define MOJCA_MODE_PIN_A 6 // analog pin A6

#define MAX_LED_DECAY_MS 1000

const int noteON = 144;  // 10010000
const int noteOFF = 128; // 10000000
const int instrumentSelect = 192; // 11000000

int patch = 0; // 0-based!

int loopTime;

// Test optical gates
// Input
// Output: D2..D7 for column 0..5
// Input: A0..A5, D11, D12 for sensor 0..7

static const bool PRINT_ANALOG = false;

int inputPinNr[TREBLE_INPUT_NR_COUNT] = {A0,A1,A2,A3,A4,A5};   // to measure the treble button states of the selected bank
int bassInputPinNr[BASS_INPUT_NR_COUNT] = {3, 4};            // to measure the bass button states of the selected bank
int outputPinNr[BANK_COUNT] = {5,6,7,8,9,10,11,12}; // to select a bank

bool buttonState[BANK_COUNT*TREBLE_INPUT_NR_COUNT];     // bankNr*6+inputNr (the inputNr is the nr within the group of 6 buttons, horizontally aligned)
bool bassButtonState[BANK_COUNT*BASS_INPUT_NR_COUNT]; // bankNr*2+inputNr (which is bass rowNr)

bool pullState;

bool mojcaMode = true;

//int decayTimeMs[4][13]; // duration since last key release
int decayTimeMs[BANK_COUNT*TREBLE_INPUT_NR_COUNT];


//const char *noteNames = "C DbD EbE F GbG AbA BbB ";
const char *noteNames = "C C#D D#E F F#G G#A BbB ";

// bank: 0-7. inputNr: 0-5
// row: 0 = bottom, 3 = top row
// column: 0..11 (most likely every column is L-shaped)
// ledNr = row*12+column

// Converts (bank, inputNr) to (row, column)
void sensorToCoordinate(int bank, int inputNr, int& row, int& column)
{
  // Special button: D11 -> B12.  Row B rightmost key (#13) is encoded as row D last key
  if (bank == BANK_COUNT-1 && inputNr == TREBLE_INPUT_NR_COUNT-1) {
    row = 1; // row B
    column = TREBLE_INPUT_NR_COUNT+TREBLE_INPUT_NR_COUNT;
  } else {
    // row 0: bank 0 and 1. row 1: bank 2 and 3. Etc
    row = bank/2;
    column = inputNr + TREBLE_INPUT_NR_COUNT*(bank%2);
  }

  /*
  row = 3-(inputNr/2);
  column = (5-bank)*2+(1-inputNr%2);

  if (row == 3 && column == 0) {
    row = 1;
    column = 0;
  } else if (row == 1) {
    column += 1;
  }
  */
}

// Converts (row, column) to (bank, inputNr)
void coordinateToSensor(int row, int column, int& bank, int& inputNr) {
  if (row == 1 && column == TREBLE_INPUT_NR_COUNT) {
    // Special key, cannot be encoded within bank 3 so it is moved to bank 7
    bank = 7;
    inputNr = TREBLE_INPUT_NR_COUNT-1;
  } else {
    if (column<TREBLE_INPUT_NR_COUNT) {
      bank = row*2;
      inputNr = column;
    } else {
      bank = row*2+1;
      inputNr = column-TREBLE_INPUT_NR_COUNT;
    }
  }
}

// Converts treble row/column to MIDI note number
// Row: 0, 1, 2, 3
// Column: a L shape
int getNoteNumber(bool pull, int row, int column) {
  int noteNumber;
  if (mojcaMode) {
    int noteOffset = trebleNoteOffsetPerPatch[patch];
    if (pull) {
      noteNumber = pullNoteNumber[3-row][column];
    } else {
      noteNumber = pushNoteNumber[3-row][column];
    }
    if (noteNumber > 0) {
      noteNumber += noteOffset;
    }
  } else {
    // vincent mode
    int noteOffset = trebleNoteOffsetPerPatch[patch];
    if (pull) {
      noteNumber = vincentPullNoteNumber[3-row][column];
    } else {
      noteNumber = vincentPushNoteNumber[3-row][column];
    }
    if (noteNumber > 0) {
      noteNumber += noteOffset;
    }
  }
  return noteNumber;
}

// bankNr: 0-7
// inputNr: 0 for first row, 1 for second row
int getSpecialBassNoteNumber(bool pull, int bankNr, int inputNr) {
  if (mojcaMode) {
    if (pull) {
      return specialMojcaBassPullNoteNumber[inputNr][bankNr];
    } else { // push
      return specialMojcaBassPushNoteNumber[inputNr][bankNr];
    }
  } else {
    // vincent mode
    if (pull) {
      return specialVincentBassPullNoteNumber[inputNr][bankNr];
    } else { // push
      return specialVincentBassPushNoteNumber[inputNr][bankNr];
    }
  }
}

// Returns pointer to array with 3 values
// inputNr = rowNr. value 0 is closest to left hand.
int* getBassNoteNumbers(bool pull, int bankNr, int inputNr) {
  static int noteNumbers[3];
  for (int i=0;i<3;i++) {
    if (mojcaMode) {
      int noteOffset = bassNoteOffsetPerPatch[patch];
      int noteNumber;
      if (pull) {
        noteNumber = mojcaBassPullNoteNumber[inputNr][bankNr][i];
      } else { // push
        noteNumber = mojcaBassPushNoteNumber[inputNr][bankNr][i];
      }
      if (noteNumber > 0) {
        noteNumber += noteOffset;
      }
      noteNumbers[i] = noteNumber;
    } else {
      // vincent mode
      int noteOffset = bassNoteOffsetPerPatch[patch];
      int noteNumber;
      if (pull) {
        noteNumber = vincentBassPullNoteNumber[inputNr][bankNr][i];
      } else {
        // push
        noteNumber = vincentBassPushNoteNumber[inputNr][bankNr][i];
      }
      if (noteNumber > 0) {
        noteNumber += noteOffset;
      }
      noteNumbers[i] = noteNumber;
    }
  }
  return noteNumbers;
}

int getLedNr(int row, int column) {
  if (row == 0) {
    return column;
  } else if (row == 1) {
    return 12 + column;
  } else if (row == 2) {
    return 12 + 13 + column;
  } else if (row == 3) {
    return 12 + 13 + 12 + column;
  } else {
    return 0;
  }
}

// for native midi
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}


/*
 * Sends MIDI message fo three values: command, note and velocity.
 * command can be noteON or noteOFF.
 */
void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  if (MIDInote>0) {
    if (MIDI) {
      if (command == noteON) {
        notePushedCount[MIDInote]++;
#ifdef OUTPUT_TEST_PROGRAM
  Serial.println("MIDI message!");
#else
        if (NATIVE_MIDI) {
          noteOn(0, MIDInote, MIDIvelocity);
        } else {
          Serial.write(command); //send note on or note off command
          Serial.write(MIDInote); //send pitch data
          Serial.write(MIDIvelocity); //send velocity data
        }
#endif
      } else if (command == noteOFF) {
        notePushedCount[MIDInote]--;
        if (notePushedCount[MIDInote] < 0) {
          notePushedCount[MIDInote] = 0;
        }
        if (notePushedCount[MIDInote] == 0) {
          if (NATIVE_MIDI) {
            noteOff(0, MIDInote, MIDIvelocity);
          } else {
            Serial.write(command); //send note on or note off command
            Serial.write(MIDInote); //send pitch data
            Serial.write(MIDIvelocity); //send velocity data
          }
        }
      }
    } else {
      Serial.print("Command: ");
      Serial.print(command);
      Serial.print(" MIDInote: ");
      Serial.print(MIDInote);
      Serial.print(" MIDIvelocity: ");
      Serial.print(MIDIvelocity);
  
      int noteNr=MIDInote%12;
      int noteOctave=MIDInote/12 -1;
      Serial.print(" ");
      Serial.print(noteNames[noteNr*2]);
      if (noteNames[noteNr*2+1] != ' ') {
        Serial.print(noteNames[noteNr*2+1]);
      }
      Serial.print(noteOctave);
  
      if (command == 144) {
        Serial.println(" on");
      } else if (command == 128) {
        Serial.println(" off");
      }
    }
  }
}

/* Send generic MIDI command consisting of two values: command and data. Could be anything as long as it follows the MIDI protocol.
 */
void serialMIDImessage2(int command, int data) {
  if (NATIVE_MIDI) {
    Serial.println("serialMIDImessage2 not implemented for native midi!");
  } else {
    if (MIDI) {
      Serial.write(command);
      Serial.write(data);
    } else {
      Serial.print("Command: ");
      Serial.print(command);
      Serial.print(" Data: ");
      Serial.println(data);
    }
  }
}

/* Send generic MIDI command consisting of three values: command and two data values. Could be anything as long as it follows the MIDI protocol.
 * While it could be used for noteON/noteOFF commands, it is better to use the MIDImessage function for noteON/noteOFF as it then keeps track of the 
 * number of buttons playing the same note.
 */
void serialMIDImessage3(int command, int data, int data2) {
  if (NATIVE_MIDI) {
    Serial.println("serialMIDImessage3 not implemented for native midi!");
  } else {
    if (MIDI) {
      Serial.write(command);
      Serial.write(data);
      Serial.write(data2);
    } else {
      Serial.print("Command: ");
      Serial.print(command);
      Serial.print(" Data: ");
      Serial.print(data);
      Serial.print(" Data2: ");
      Serial.println(data2);
    }
  }
}


/* 
 * In Vincent mode row 0 is not playing any notes. So this row is repurposed to selecting the patch.
 * Patch is binary encoded using the leftmost eight buttons of row 0.
 */
void setPatchAccordingToButtonState() {
  int bankNr;
  int inputNr;

  // bottom row
  int newPatchNr = 0;
  for (int i=0;i<8;i++) {
    coordinateToSensor(0,i,bankNr,inputNr);
    if (buttonState[bankNr*8+inputNr]) {
      newPatchNr += 1<<i;
    }
  }
  patch=newPatchNr;
  if (NATIVE_MIDI) {
    controlChange(instrumentSelect,patch,0);
  } else {
    serialMIDImessage2(instrumentSelect,patch);
  }
}

// Play or stop the note of a specific treble button number, given pull state. Keeps internally track of number of times a specific note is pressed.
// Returns noteNumber. Is 0 if no note is assigned to the specific button, given the current mojcaMode and myPullState.
int turnTrebleButton(bool noteOn, int buttonNr, int myPullState) {
  // buttonNr: 0..47
  int bankNr = buttonNr/TREBLE_INPUT_NR_COUNT;
  int inputNr = buttonNr%TREBLE_INPUT_NR_COUNT;
  int row, column;
  sensorToCoordinate(bankNr, inputNr, row, column);

  int noteNumber = getNoteNumber(myPullState,row,column);
  MIDImessage(noteOn?noteON:noteOFF, noteNumber, noteOn?NOTE_ON_VELOCITY:NOTE_OFF_VELOCITY); // is native MIDI compatible

  if (noteOn) {
    decayTimeMs[buttonNr] = 0;
  }
  return noteNumber;
}

// Play or stop the note of a specific bass button number, given pull state. Keeps internally track of number of times a specific note is pressed.
// buttonNr: bankNr*BASS_INPUT_NR_COUNT+inputNr
void turnBassButton(bool noteOn, int buttonNr, int myPullState) {
  // send note off on oldPullState and note on on pull
  int bankNr = buttonNr/BASS_INPUT_NR_COUNT;
  int inputNr = buttonNr%BASS_INPUT_NR_COUNT;
  if (1) {
    // 2nd row is shifted by one
    bankNr -= inputNr;
    if (bankNr < 0) {
      // should not happen
      return;
    }
  }

  if (isSpecialInstrument[patch]) {
    // special instrument with single note for chords
    int noteNumber = getSpecialBassNoteNumber(myPullState,bankNr,inputNr);
    MIDImessage(noteOn?noteON:noteOFF, noteNumber, noteOn?NOTE_ON_VELOCITY:NOTE_OFF_VELOCITY); // is native MIDI compatible
  } else {
    // normal instruments
    int* noteNumbers = getBassNoteNumbers(myPullState,bankNr,inputNr);
    for (int noteSubNr=0;noteSubNr<3;noteSubNr++) {
      if (noteNumbers[noteSubNr]>0) {
        MIDImessage(noteOn?noteON:noteOFF, noteNumbers[noteSubNr], noteOn?NOTE_ON_VELOCITY:NOTE_OFF_VELOCITY); // is native MIDI compatible
      }
    }
  }
}


// Turns all buttons off of the current mojcaMode and pullState
void turnAllTrebleButtonsOff() {
  for (int buttonNr=0;buttonNr<48;buttonNr++) {
    turnTrebleButton(false, buttonNr, pullState);
  }
}

// Turns all buttons off of the current mojcaMode and pullState
void turnAllBassButtonsOff() {
  for (int buttonNr=0;buttonNr<16;buttonNr++) {
    turnBassButton(false, buttonNr, pullState);
  }
}

// Switch from pull to push (or vice-versa) detected. pullState is the new pull state, !pullState the old one.
void manageSwitchPull() {
  // pressed treble notes off
  for (int buttonNr=0; buttonNr<48; buttonNr++) {
    if (buttonState[buttonNr]) {
      // send note off on oldPullState and note on on pull
      turnTrebleButton(false,buttonNr,!pullState);
    }
    decayTimeMs[buttonNr] = MAX_LED_DECAY_MS;
  }

  // pressed treble notes on
  for (int buttonNr=0; buttonNr<48; buttonNr++) {
    if (buttonState[buttonNr]) {
      turnTrebleButton(true,buttonNr,pullState);
    }
  }

  // pressed bass notes off
  for (int buttonNr=0; buttonNr<16; buttonNr++) {
    if (bassButtonState[buttonNr]) {
      turnBassButton(false,buttonNr,!pullState);
    }
  }

  // pressed bass notes on
  for (int buttonNr=0; buttonNr<16; buttonNr++) {
    if (bassButtonState[buttonNr]) {
      // send note off on oldPullState and note on on pull
      turnBassButton(true, buttonNr, pullState);
    }
  }
}

/*
 * Assumes that bankNr is currently activated. Reads the six sensor lines and processes these.
 * sendMidiMessages: if false then no midi messages are generated and button state is not saved, e.g. to auto-detect trebleInverted.
 */
void manageTrebleButtons(int bankNr, bool sendMidiMessages) {
  for (int inputNr=0;inputNr<TREBLE_INPUT_NR_COUNT;inputNr++) {
    int digitalValue = digitalRead(inputPinNr[inputNr]) ^ trebleInverted;
    int buttonNr = bankNr*TREBLE_INPUT_NR_COUNT+inputNr;
    if (digitalValue != buttonState[buttonNr]) {
      buttonState[buttonNr] = digitalValue;
      if (sendMidiMessages) {
        if (!MIDI) {
          Serial.print("Treble note change detected! Bank=");
          Serial.print(bankNr);
          Serial.print(" inputNr=");
          Serial.print(inputNr);
          Serial.print(" buttonNr=");
          Serial.print(buttonNr);
          Serial.print(" noteOn=");
          Serial.print(buttonState[buttonNr]);
          Serial.print(" decayTimeMs=");
          Serial.print(decayTimeMs[buttonNr]);
          Serial.println();
        }
        int noteNumber = turnTrebleButton(digitalValue,buttonNr,pullState);
        if ((noteNumber == 0) && digitalValue) {
          // possibly special keys to change patch
          int row, column;
          sensorToCoordinate(bankNr, inputNr, row, column);
          if (!mojcaMode && row == 3 && column == 7) {
            if (!MIDI) {
              Serial.println("Patch change, resetting all buttons...");
            }
            setPatchAccordingToButtonState();
            resetAllButtons();
          }
        }
      }
    }
  }
}

void manageBassButtons(int bankNr, bool sendMidiMessages) {
  // bass
  for (int inputNr=0;inputNr<BASS_INPUT_NR_COUNT;inputNr++) {
    int noteOn = digitalRead(bassInputPinNr[inputNr]) ^ bassInverted;
    int buttonNr = bankNr*BASS_INPUT_NR_COUNT+inputNr;
    if (noteOn != bassButtonState[buttonNr]) {
      bassButtonState[buttonNr]=noteOn;
      if (sendMidiMessages) {
        if (!MIDI) {
          Serial.print("Bass note change detected! Bank=");
          Serial.print(bankNr);
          Serial.print(" inputNr=");
          Serial.print(inputNr);
          Serial.print(" buttonNr=");
          Serial.print(buttonNr);
          Serial.print(" noteOn=");
          Serial.println(noteOn);
        }
        turnBassButton(noteOn,buttonNr,pullState);
      }
    }
  }
}

/*
 * Iterates over all banks
 */
void readButtons(bool sendMidiMessages) {
  for (int bankNr=0;bankNr<BANK_COUNT;bankNr++) {
    digitalWrite(outputPinNr[bankNr],HIGH);
    delayMicroseconds(500); // to give electronics time to activated
    manageTrebleButtons(bankNr,sendMidiMessages);
    manageBassButtons(bankNr,sendMidiMessages);
    digitalWrite(outputPinNr[bankNr],LOW);
  }
}


void resetAllButtons() {
  if (!MIDI) {
      Serial.println("Resetting all buttons...");
  }

  turnAllTrebleButtonsOff();  
  turnAllBassButtonsOff();  
}

void loop() {
  static uint32_t lastTime = millis();
  uint32_t tm = millis();
  loopTime = tm - lastTime;
  lastTime = tm;

  bool newMojcaMode = analogRead(MOJCA_MODE_PIN_A)<512;
  if (newMojcaMode != mojcaMode) {
    if (!MIDI) {
      Serial.print("Mojca mode changed! New value: ");
      Serial.println(newMojcaMode);
    }
    resetAllButtons();
    mojcaMode = newMojcaMode;
  }

  bool newPullState = digitalRead(DIRECTION_BUTTON_PIN);
  if (newPullState != pullState) {
    if (!MIDI) {
      Serial.print("Pull state changed! New value: ");
      Serial.println(newPullState);
    }
    pullState = newPullState;
    manageSwitchPull();
    //Serial.println("Finished managing pull state.");
  }

  readButtons(true);

  updateKeyDecay();

  setLedsToActiveColor();

  static int q;
  if (q++ >= 100) {
    q = 0;
    /*
    Serial.print("ButtonState[18]=");
    Serial.print(buttonState[18]);
    Serial.print(" decayTimeMs[18]=");
    Serial.print(decayTimeMs[18]);
    Serial.println();
    */
  }
}

void playMidiMessageTestProgram() {
  pinMode(13,OUTPUT);
  bool led=true;

  int myPause=1000;
  while (1) {
    digitalWrite(13,led);
    led=!led;

    MIDImessage(noteON, 72, NOTE_ON_VELOCITY); // C4 on
    delay(myPause);
  
    MIDImessage(noteOFF, 72, NOTE_OFF_VELOCITY); // C4 off  
    delay(myPause);
  }
}


void playShowAllButtonsTestProgram() {
  while(1) {
    showAllButtons();
    delay(200);
  }
}

void playSingleBankTestProgram() {
  int bankNr = 0;
  digitalWrite(outputPinNr[bankNr],HIGH);

  while(1) {
    readPrintInputs();
    Serial.println();
    delay(200);
  }      
}


void setup() {
  pinMode(DIRECTION_BUTTON_PIN,INPUT_PULLUP); // direction pushbutton

#ifdef MIDI_MESSAGE_TEST_PROGRAM
  playMidiMessageTestProgram(); // this won't return
#endif

  setupLeds();

  for (int keyNr=0;keyNr<48;keyNr++) {
      decayTimeMs[keyNr]=0;
  }

  // Bank pins
  for (int i=0; i<BANK_COUNT; i++) {
    pinMode(outputPinNr[i],OUTPUT);
  }
  // Treble input pins
  for (int i=0;i<TREBLE_INPUT_NR_COUNT;i++) {
    pinMode(inputPinNr[i],INPUT);
  }
  // Bass input pins
  for (int i=0;i<BASS_INPUT_NR_COUNT;i++) {
    pinMode(bassInputPinNr[i],INPUT);
  }

#ifdef SHOW_BUTTONS_TEST_PROGRAM
  playShowAllButtonsTestProgram();
#endif
#ifdef SINGLE_BANK_TEST_PROGRAM
  playSingleBankTestProgram();
#endif

  mojcaMode = analogRead(MOJCA_MODE_PIN_A)<512;

  for (int i=0;i<48;i++) {
    buttonState[i]=0;
  }
  for (int i=0;i<16;i++) {
    bassButtonState[i]=0;
  }
  pullState = 0;

  if (!MIDI) {
    if (mojcaMode) {
      Serial.println("Four row");
    } else {
      Serial.println("Two row, CG");
    }
  }

  autoDetectTrebleBassInverted();

  //updateLeds();
  //testLeds();
  setLedsToNoteColor();
}

void autoDetectTrebleBassInverted() {
  trebleInverted = false;
  bassInverted = false;
  readButtons(false); // read buttons without sending MIDI messages
  int buttonPressedCount = 0;
  for (int i=0;i<BANK_COUNT*TREBLE_INPUT_NR_COUNT; i++) {
    if (buttonState[i]) {
      buttonPressedCount++;
    }
  }
  if (buttonPressedCount > BANK_COUNT*TREBLE_INPUT_NR_COUNT/2) {
    trebleInverted = true;
    for (int i=0;i<BANK_COUNT*TREBLE_INPUT_NR_COUNT; i++) {
      buttonState[i] = false;
    }
  }

  buttonPressedCount = 0;
  for (int i=0;i<BANK_COUNT*BASS_INPUT_NR_COUNT; i++) {
    if (bassButtonState[i]) {
      buttonPressedCount++;
    }
  }
  if (buttonPressedCount > BANK_COUNT*TREBLE_INPUT_NR_COUNT/2) {
    bassInverted = true;
    for (int i=0;i<BANK_COUNT*BASS_INPUT_NR_COUNT; i++) {
      bassButtonState[i] = false;
    }
  }
}


void readPrintInputs() {
  for (int inputNr=0;inputNr<TREBLE_INPUT_NR_COUNT;inputNr++) {
    int digitalValue = digitalRead(inputPinNr[inputNr]);
    Serial.print(digitalValue);
  }
  Serial.print(",");
  for (int inputNr=0;inputNr<BASS_INPUT_NR_COUNT;inputNr++) {
    int digitalValue = digitalRead(bassInputPinNr[inputNr]);
    Serial.print(digitalValue);
  }
}

void showAllButtons() {
  for (int bankNr=0;bankNr<BANK_COUNT;bankNr++) {
    Serial.print('#');
    Serial.print(bankNr);
    Serial.print(':');
    digitalWrite(outputPinNr[bankNr],HIGH);
    delayMicroseconds(500);
    readPrintInputs();
    digitalWrite(outputPinNr[bankNr],LOW);
    Serial.print(' ');
  }
  Serial.println();
}

void setLedsToNoteColor() {
  for (int keyNr = 0; keyNr < BANK_COUNT * TREBLE_INPUT_NR_COUNT; keyNr ++) {
    int bank = keyNr / TREBLE_INPUT_NR_COUNT;
    int inputNr = keyNr % TREBLE_INPUT_NR_COUNT;
    int row;
    int column;
    sensorToCoordinate(bank, inputNr, row, column);
    int midiNoteNumber = getNoteNumber(pullState, row, column);
    int ledNr = getLedNr(row, column);
    if (midiNoteNumber > 0) {
      strip.setPixelColor(ledNr, strip.ColorHSV(noteHue[midiNoteNumber%12],255,30));
    } else {
      strip.setPixelColor(ledNr, 0);
    }
  }
  strip.show();
}


void setLedsToActiveColor() {
  for (int keyNr = 0; keyNr < BANK_COUNT * TREBLE_INPUT_NR_COUNT; keyNr++) {
    int bank = keyNr / TREBLE_INPUT_NR_COUNT;
    int inputNr = keyNr % TREBLE_INPUT_NR_COUNT;
    int row;
    int column;
    sensorToCoordinate(bank, inputNr, row, column);
    int midiNoteNumber = getNoteNumber(pullState, row, column);
    int ledNr = getLedNr(row, column);
    if (midiNoteNumber > 0) {
      uint32_t color;
      if (buttonState[keyNr]) {
        color = strip.ColorHSV(noteHue[midiNoteNumber%12],255,255);
      } else if (decayTimeMs[keyNr] < MAX_LED_DECAY_MS) {
        color = strip.ColorHSV(noteHue[midiNoteNumber%12],255,map(decayTimeMs[keyNr],0,MAX_LED_DECAY_MS,127,10));
      } else {
        color = strip.ColorHSV(noteHue[midiNoteNumber%12],255,10);
      }
      strip.setPixelColor(ledNr, color);
    } else {
      strip.setPixelColor(ledNr, 0);
    }
  }
  strip.show();
}

void updateKeyDecay() {
  for (int keyNr=0;keyNr<48;keyNr++) {
    if (!buttonState[keyNr]) {
      if (decayTimeMs[keyNr] < MAX_LED_DECAY_MS) {
        decayTimeMs[keyNr] += loopTime;
      }
    }
  }
}

