 /*
 * Arduino Nano
 * First, wooden accordion: v3.1, old bootloader
 * Newer accordions: Nano, default boot loader
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

// if true then a simplistic test program will run
//#define MIDI_MESSAGE_TEST_PROGRAM // repeatedly plays and stops C4 note
//#define SHOW_BUTTONS_TEST_PROGRAM
//#define SINGLE_BANK_TEST_PROGRAM
//#define OUTPUT_TEST_PROGRAM // in MIDI mode

// true to send notes to Raspberry PI, false to send to console, both at 115200
const bool MIDI = true;

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

const int noteON = 144;  // 10010000
const int noteOFF = 128; // 10000000
const int instrumentSelect = 192; // 11000000

int patch = 0; // 0-based!

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


//const char *noteNames = "C DbD EbE F GbG AbA BbB ";
const char *noteNames = "C C#D D#E F F#G G#A BbB ";

// bank: 0-7. inputNr: 0-5
// row: 0 = bottom, 3 = top row
void sensorToCoordinate(int bank, int inputNr, int& row, int& column)
{
  // Special button: D11 -> B12.  Row B rightmost key (#13) is encoded as row D last key
  if (bank == BANK_COUNT-1 && inputNr == TREBLE_INPUT_NR_COUNT-1) {
    row = 1; // row B
    column = TREBLE_INPUT_NR_COUNT;
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

void coordinateToSensor(int row, int column, int& bank, int& inputNr) {
  if (row == 1 && column == TREBLE_INPUT_NR_COUNT) {
    // Special key, cannot be encoded within bank 3 so it is moved to bank 7
    bank = 7;
    inputNr = TREBLE_INPUT_NR_COUNT;
  } else {
    if (column<TREBLE_INPUT_NR_COUNT) {
      bank = row*2;
      inputNr = column;
    } else {
      bank = row*2+1;
      inputNr = column-TREBLE_INPUT_NR_COUNT;
    }
  }

  /*
  int ph;
  if (row == 1) {
    bank = 6 - ((column+1)/2);
    ph = column%2;
  } else {
    bank = 5 - (column/2);
    ph = (column+1)%2;
  }
  inputNr = 2*(3-row)+ph;

  if (bank == 6 && inputNr == 4) {
      bank = 5;
      inputNr = 1;
  }
  */
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
  Serial.write(command); //send note on or note off command
  Serial.write(MIDInote); //send pitch data
  Serial.write(MIDIvelocity); //send velocity data
#endif
      } else if (command == noteOFF) {
        notePushedCount[MIDInote]--;
        if (notePushedCount[MIDInote] < 0) {
          notePushedCount[MIDInote] = 0;
        }
        if (notePushedCount[MIDInote] == 0) {
          Serial.write(command); //send note on or note off command
          Serial.write(MIDInote); //send pitch data
          Serial.write(MIDIvelocity); //send velocity data
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
void MIDImessage2(int command, int data) {
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

/* Send generic MIDI command consisting of three values: command and two data values. Could be anything as long as it follows the MIDI protocol.
 * While it could be used for noteON/noteOFF commands, it is better to use the MIDImessage function for noteON/noteOFF as it then keeps track of the 
 * number of buttons playing the same note.
 */
void MIDImessage3(int command, int data, int data2) {
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
  MIDImessage2(instrumentSelect,patch);
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
  MIDImessage(noteOn?noteON:noteOFF, noteNumber, noteOn?NOTE_ON_VELOCITY:NOTE_OFF_VELOCITY);
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
    MIDImessage(noteOn?noteON:noteOFF, noteNumber, noteOn?NOTE_ON_VELOCITY:NOTE_OFF_VELOCITY);
  } else {
    // normal instruments
    int* noteNumbers = getBassNoteNumbers(myPullState,bankNr,inputNr);
    for (int noteSubNr=0;noteSubNr<3;noteSubNr++) {
      if (noteNumbers[noteSubNr]>0) {
        MIDImessage(noteOn?noteON:noteOFF, noteNumbers[noteSubNr], noteOn?NOTE_ON_VELOCITY:NOTE_OFF_VELOCITY);
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
          Serial.println(digitalValue);
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
  Serial.begin(115200);
  pinMode(DIRECTION_BUTTON_PIN,INPUT_PULLUP); // direction pushbutton

#ifdef MIDI_MESSAGE_TEST_PROGRAM
  playMidiMessageTestProgram(); // this won't return
#endif

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

/*
void showAnalog() {
  digitalWrite(outputPinNr[2],HIGH);
  for (int inputNr=0;inputNr<8;inputNr++) {
    int analogValue = analogRead(inputNr);
    int digitalValue = !digitalRead(inputPinNr[inputNr]);
    Serial.print(analogValue);
    Serial.print('/');
    Serial.print(digitalValue);
    Serial.print(' ');
  }
  Serial.println();
}
*/

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
