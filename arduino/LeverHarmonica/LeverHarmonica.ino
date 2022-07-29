 /*
 * Arduino Nano v3.1, Old bootloader
 * 
 * MIDI note number 69 is A4 concert pitch = 440 Hz
 * 
 * Note number 43 is G2
 * Note number 31 is G1
 * Note number 19 is G0 chord
 * 
 * pin assignment:
 * Inputs: A0, A1, A2, A3, A4, A5, 11, 12,    8?, 10?
 * Outputs: 2, 3, 4, 5, 6, 7
 * 8 tilt
 * 9 direction
 * 10 free: direction led
 * 6 mojcaMode
 * 7 free: button pushed
 * 0,1 serial
 *
 * Banks: high if active
 * Bass side:
 * red lines six banks
 * white lines read signals, connect to inputs
 * 
 * SamplerBox (Raspberry Pi): To mount the boot partition: "sudo mount /dev/mmcblk0p1 /boot"
 * To restart the service: systemctl restart samplerbox.service
 */

/*
 * Patches:
 * 0. Special Accordion recorded by Mojca&Vincent on July 27, 2022
 * 1. Accordion
 * 2. MKII Flute
 * 3. Lately Bass
 * 4. Saw
 * 5. Square
 * 6. Piano
 * 7. Airy Keys
 * 8. Chrous Keys
 * 9. Melanchosia
 * 10. Music box
 * 11. Octave bass
 * 12. Organ
 * 13. Rezo bass
 * 14. Simple bass
 *
 */

// if true then a simplistic test program will run
//#define TEST_PROGRAM

// true to send notes to Raspberry PI, false to send to console, both at 115200
const bool MIDI = true;

const int PATCH_COUNT = 15;
int isSpecialInstrument[PATCH_COUNT] = {true,false,false,false,false,false,false,false,false,false,false,false,false,false}; // 0-based
int trebleNoteOffsetPerPatch[PATCH_COUNT] = {0,-12,0,12,12,12,0,0,12,0,0,0,0,0,0}; // 0-based
int bassNoteOffsetPerPatch[PATCH_COUNT] = {0,-12,0,12,12,12,0,0,12,0,0,0,0,0,0}; // 0-based

int notePushedCount[256] = {0}; // to handle multiple buttons for one note

// 13 columns, four rows

#define BASS_INPUT_NR_COUNT 2 // per bank

#define NOTE_ON_VELOCITY 100
#define NOTE_OFF_VELOCITY 127

#define DIRECTION_BUTTON_PIN 10

const int noteON = 144;  // 10010000
const int noteOFF = 128; // 10000000
const int instrumentSelect = 192; // 11000000


int patch = 0; // 0-based!

// Test optical gates
// Output: D2..D7 for column 0..5
// Input: A0..A5, D11, D12 for sensor 0..7

static const bool PRINT_ANALOG = false;

int inputPinNr[] = {A0,A1,A2,A3,A4,A5,11,12};
int bassInputPinNr[] = {9, 8}; // was: 10, 8
int outputPinNr[] = {2,3,4,5,6,7};

bool buttonState[48]; // bankNr*8+inputNr
bool bassButtonState[12]; // bankNr*2+inputNr

bool pullState;

bool mojcaMode = true;

static const byte midi_C0   = 12;
static const byte midi_Cis0 = 13;
static const byte midi_D0   = 14;
static const byte midi_Es0  = 15;
static const byte midi_E0   = 16;
static const byte midi_F0   = 17;
static const byte midi_Fis0 = 18;
static const byte midi_G0   = 19;
static const byte midi_Gis0 = 20;
static const byte midi_A0   = 21;
static const byte midi_B0   = 22;
static const byte midi_H0   = 23;
static const byte midi_C1   = 24;
static const byte midi_Cis1 = 25;
static const byte midi_D1   = 26;
static const byte midi_Es1  = 27;
static const byte midi_E1   = 28;
static const byte midi_F1   = 29;
static const byte midi_Fis1 = 30;
static const byte midi_G1   = 31;
static const byte midi_Gis1 = 32;
static const byte midi_A1   = 33;
static const byte midi_B1   = 34;
static const byte midi_H1   = 35;
static const byte midi_C2   = 36;
static const byte midi_Cis2 = 37;
static const byte midi_D2   = 38;
static const byte midi_Es2  = 39;
static const byte midi_E2   = 40;
static const byte midi_F2   = 41;
static const byte midi_Fis2 = 42;
static const byte midi_G2   = 43;
static const byte midi_Gis2 = 44;
static const byte midi_A2   = 45;
static const byte midi_B2   = 46;
static const byte midi_H2   = 47;
static const byte midi_C3   = 48;
static const byte midi_Cis3 = 49;
static const byte midi_D3   = 50;
static const byte midi_Es3  = 51;
static const byte midi_E3   = 52;
static const byte midi_F3   = 53;
static const byte midi_Fis3 = 54;
static const byte midi_G3   = 55;
static const byte midi_Gis3 = 56;
static const byte midi_A3   = 57;
static const byte midi_B3   = 58;
static const byte midi_H3   = 59;
static const byte midi_C4   = 60;
static const byte midi_Cis4 = 61;
static const byte midi_D4   = 62;
static const byte midi_Es4  = 63;
static const byte midi_E4   = 64;
static const byte midi_F4   = 65;
static const byte midi_Fis4 = 66;
static const byte midi_G4   = 67;
static const byte midi_Gis4 = 68;
static const byte midi_A4   = 69;
static const byte midi_B4   = 70;
static const byte midi_H4   = 71;
static const byte midi_C5   = 72;
static const byte midi_Cis5 = 73;
static const byte midi_D5   = 74;
static const byte midi_Es5  = 75;
static const byte midi_E5   = 76;
static const byte midi_F5   = 77;
static const byte midi_Fis5 = 78;
static const byte midi_G5   = 79;
static const byte midi_Gis5 = 80;
static const byte midi_A5   = 81;
static const byte midi_B5   = 82;
static const byte midi_H5   = 83;
static const byte midi_C6   = 84;
static const byte midi_Cis6 = 85;
static const byte midi_D6   = 86;
static const byte midi_Es6  = 87;
static const byte midi_E6   = 88;
static const byte midi_F6   = 89;
static const byte midi_Fis6 = 90;
static const byte midi_G6   = 91;
static const byte midi_Gis6 = 92;
static const byte midi_A6   = 93;
static const byte midi_B6   = 94;
static const byte midi_H6   = 95;
static const byte midi_C7   = 96;
static const byte midi_Cis7 = 97;
static const byte midi_D7   = 98;

#define midi_chord_A1  {midi_A1,  midi_Cis2, midi_E2}
#define midi_chord_B1  {midi_B1,  midi_D2,   midi_F2}
#define midi_chord_C2  {midi_C2,  midi_E2,   midi_G2}
#define midi_chord_D2  {midi_D2,  midi_Fis2, midi_A2}
#define midi_chord_D2m {midi_D2,  midi_F2,   midi_A2}
#define midi_chord_Es2 {midi_Es2, midi_G2,   midi_B2}
#define midi_chord_F2  {midi_F2,  midi_A2,   midi_C3}
#define midi_chord_G2  {midi_G2,  midi_H2,   midi_D3}
#define midi_chord_no  {0, 0, 0}

static const byte midi_chord_A0_special  = midi_A0;
static const byte midi_chord_B0_special  = midi_B0;
static const byte midi_chord_C1_special  = midi_C0;
static const byte midi_chord_D1_special  = midi_D0;
static const byte midi_chord_D1m_special = midi_Cis0;
static const byte midi_chord_Es1_special = midi_Es0;
static const byte midi_chord_F1_special  = midi_F0;
static const byte midi_chord_G1_special  = midi_G0;

// notation in comments is in "octave-3"

/* PUSH
    {--,ds,as,gs,es,gs,ds,ds,--,--,--,--,--},
   {f0,bs,d1,f1,bs,d2,f2,bs,d3,f3,bs,d4,--},
  {c0,f0,a0,c1,f1,a1,c2,f2,a2,c3,f3,a3,c4},
   {c0,e0,g0,c1,e1,g1,c2,e2,g2,c3,e3,g3,--}
*/
byte pushNoteNumber[4][13]={
  {          0,        0,        0,        midi_Fis4, midi_Es5, 0, 0, 0, 0, 0, 0, 0, 0},
  {     midi_F3,  midi_B3,  midi_D4,  midi_F4,  midi_B4,  midi_D5,  midi_F5,  midi_B5,  midi_D6,  midi_F6,  midi_B6,  midi_D7,  0},
  {midi_C3,  midi_F3,  midi_A3,  midi_C4,  midi_F4,  midi_A4,  midi_C5,  midi_F5,  midi_A5,  midi_C6,  midi_F6,  midi_A6,  midi_C7},
  {     midi_C3,  midi_E3,  midi_G3,  midi_C4,  midi_E4,  midi_G4,  midi_C5,  midi_E5,  midi_G5,  midi_C6,  midi_E6,  midi_G6,  0},
};

/* PULL
    {--,as,gs,ds,ds,gs,as,ds,--,--,--,--,--}
   {a0,es,f1,a1,c2,es,f2,a2,c3,es,g3,a3,--}
  {e0,bs,c1,e1,g1,bs,c2,e2,g2,bs,d3,e3,g4}
   {f0,g0,b0,d1,f1,a1,b1,d2,f2,a2,b2,d3,--}
 */
byte pullNoteNumber[4][13]={
  {          0,        0,        0,        midi_Cis4, midi_Cis5, 0, 0, 0, 0, 0, 0, 0, 0},
  {     midi_A3,  midi_Es4, midi_F4,  midi_A4,  midi_C5,  midi_Es5, midi_F5,  midi_A5,  midi_C6,  midi_Es6, midi_G6,  midi_A6,  0},
  {midi_E3,  midi_B3,  midi_C4,  midi_E4,  midi_G4,  midi_B4,  midi_C5,  midi_E5,  midi_G5,  midi_B5,  midi_C6,  midi_E6,  midi_G6},
  {     midi_F3,  midi_G3,  midi_H3,  midi_D4,  midi_F4,  midi_A4,  midi_H4,  midi_D5,  midi_F5,  midi_A5,  midi_H5,  midi_D6,  0}
};

// Vincent's two-row harmonica
/* PUSH:
  (D#C F A D F A C F A)
 (F#G C E G C E G C E G)
*/

byte vincentPushNoteNumber[4][13]={
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {     midi_Es5,  midi_C4,  midi_F4,  midi_A4,  midi_D5,  midi_F5,  midi_A5,  midi_C6,  midi_F6,  midi_A6,  0, 0, 0},
  {midi_Fis4, midi_G3,  midi_C4,  midi_E4,  midi_G4,  midi_C5,  midi_E5,  midi_G5,  midi_C6,  midi_E6,  midi_G6,  0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

/* PULL:
  (C#E G BbC E G BbC E)
 (G#B D F A B D F A B D)
*/
byte vincentPullNoteNumber[4][13]={
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {     midi_Cis5, midi_E4,  midi_G4,  midi_B4,  midi_C5,  midi_E5,  midi_G5,  midi_B5,  midi_C6,  midi_E6,  0, 0, 0},
  {midi_Gis4, midi_H3,  midi_D4,  midi_F4,  midi_A4,  midi_H4,  midi_D5,  midi_F5,  midi_A5,  midi_H5,  midi_D6, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


/*
  0 1 2 3 4 5     inputNr 0
   0 1 2 3 4 -     inputNr 1

  BASS PUSH:
  c C  f  F x x
   a A bb Bb x x
*/
byte vincentBassPushNoteNumber[2][6][3]={
  {
    midi_chord_C2, {midi_C2, 0, 0},
    midi_chord_F2, {midi_F2, 0, 0},
    midi_chord_no,  midi_chord_no
  }, {
    midi_chord_A1, {midi_A2, 0, 0},
    midi_chord_B1, {midi_B1, 0, 0},
    midi_chord_no,  midi_chord_no
  }
};

/*
  BASS PULL:
  g  G  c  C x x
   dm D bb Bb x x
 */
byte vincentBassPullNoteNumber[2][6][3]={
  {
    midi_chord_G2,  {midi_G2, 0, 0},
    midi_chord_C2,  {midi_C2, 0, 0},
    midi_chord_no,   midi_chord_no
  }, {
    midi_chord_D2m, {midi_D2, 0, 0},
    midi_chord_B1,  {midi_B1, 0, 0},
    midi_chord_no,   midi_chord_no
  }
};

/*
  0 1 2 3 4 5     inputNr 0
   0 1 2 3 4 -     inputNr 1

  BASS PUSH:
  c  C  f  F  b  B
   G  a  A  d  D
*/
byte mojcaBassPushNoteNumber[2][6][3]={
  {
    midi_chord_C2, {midi_C2, 0, 0},
    midi_chord_F2, {midi_F2, 0, 0},
    midi_chord_B1, {midi_B1, 0, 0}
  }, {
                   {midi_G2, 0, 0},
    midi_chord_A1, {midi_A2, 0, 0},
    midi_chord_D2, {midi_D2, 0, 0}, midi_chord_Es2
  }
};

/*
  BASS PULL:
  g   G   c   C   f   F
    E   dm  D   es  Es
 */
byte mojcaBassPullNoteNumber[2][6][3]={
  {
    midi_chord_G2, {midi_G2, 0, 0},
    midi_chord_C2, {midi_C2, 0, 0},
    midi_chord_F2, {midi_F2, 0, 0}
  },
  {
                    {midi_E2,  0, 0},
    midi_chord_D2m, {midi_D2,  0, 0},
    midi_chord_Es2, {midi_Es2, 0, 0}, midi_chord_no
  }
};


/*
 * The special instrument (#1) has recordings for basses and chords, so play only one note for each bass button.
  BASS PUSH:
  c  C  f  F  b  B
   G  a  A  d  D
  C0 C2 F0 F2 A#0 A#1
   G2 A0 A2  D0  D2  x

 *
 */
byte specialMojcaBassPushNoteNumber[2][6]={
  {
    midi_chord_C1_special, midi_C2,
    midi_chord_F1_special, midi_F2,
    midi_chord_B0_special, midi_B1,
  }, {
                           midi_G2,
    midi_chord_A0_special, midi_A2,
    midi_chord_D1_special, midi_D2, 0,
  }
};

byte specialMojcaBassPullNoteNumber[2][6]={
  {
    midi_chord_G1_special, midi_G2,
    midi_chord_C1_special, midi_C2,
    midi_chord_F1_special, midi_F2
  },
  {
                            midi_E2,
    midi_chord_D1m_special, midi_D2,
    midi_chord_Es1_special, midi_Es2, 0
  }
};




/*
  0 1 2 3 4 5     inputNr 0
   0 1 2 3 4 -     inputNr 1

  VINCENT BASS PUSH:
  c C  f  F x x
   a A bb Bb x x
*/

byte specialVincentBassPushNoteNumber[2][6]={
  {
    midi_chord_C1_special, midi_C2,
    midi_chord_F1_special, midi_F2,
    0, 0
  }, {
    midi_chord_A0_special, midi_A2,
    midi_chord_B0_special, midi_B1,
    0, 0
  }
};


/*
  VINCENT BASS PULL:
  g  G  c  C x x
   dm D bb Bb x x
 */
byte specialVincentBassPullNoteNumber[2][6]={
  {
    midi_chord_G1_special, midi_G2,
    midi_chord_C1_special, midi_C2,
    0, 0
  },
  {
    midi_chord_D1m_special, midi_D2,
    midi_chord_B0_special,  midi_B1,
    0, 0
  }
};







//const char *noteNames = "C DbD EbE F GbG AbA BbB ";
const char *noteNames = "C C#D D#E F F#G G#A BbB ";

// row: 0 = bottom, 3 = top row
void sensorToCoordinate(int bank, int inputNr, int& row, int& column)
{
  row = 3-(inputNr/2);
  column = (5-bank)*2+(1-inputNr%2);

  if (row == 3 && column == 0) {
    row = 1;
    column = 0;
  } else if (row == 1) {
    column += 1;
  }
}

void coordinateToSensor(int row, int column, int& bank, int& inputNr) {
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
}

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

int getSpecialBassNoteNumber(bool pull, int bankNr, int inputNr) {
  if (mojcaMode) {
    //int noteOffset = bassNoteOffsetPerPatch[patch];
    //int noteNumber;
    if (pull) {
      return specialMojcaBassPullNoteNumber[inputNr][bankNr];
    } else { // push
      return specialMojcaBassPushNoteNumber[inputNr][bankNr];
    }
    /*
    if (noteNumber > 0) {
      noteNumber += noteOffset;
    }
    return noteNumber;
    */
  } else {
    // vincent mode
    if (pull) {
      return specialVincentBassPullNoteNumber[inputNr][bankNr];
    } else { // push
      return specialVincentBassPushNoteNumber[inputNr][bankNr];
    }
  }
}

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

void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  if (MIDI) {
    if (MIDInote>0) {
      if (command == noteON) {
        notePushedCount[MIDInote]++;
        Serial.write(command); //send note on or note off command
        Serial.write(MIDInote); //send pitch data
        Serial.write(MIDIvelocity); //send velocity data
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
void manageRedGreenLeds() {
  bool anyButtonPressed = false;
  for (int i=0;i<48;i++) {
    if (buttonState[i]) {
      anyButtonPressed = true;
    }
  }
  digitalWrite(ANY_BUTTON_PUSHED_LED,anyButtonPressed); // high signal, led on if any button is pressed
  digitalWrite(DIRECTION_LED,pullState); // inverted, so low signal and led on if pushing
}*/



/*
void incrementDecrementPatch(int column) {
  // increment or decrement patch
  if (column == 10) {
    //patch = (patch-2+PATCH_COUNT)%PATCH_COUNT+1;
    patch=patch-1;
    if (patch<1) {
      patch=14;
    }
  } else if (column == 11) {
    //patch=(patch)%PATCH_COUNT+1;
    patch=patch+1;
    if (patch>14) {
      patch=1;
    }
  }
  MIDImessage2(instrumentSelect,patch);
}*/

void setPatchAccordingToButtonState_old() {
  int bankNr;
  int inputNr;

  // bottom row
  for (int i=0;i<10;i++) {
    coordinateToSensor(0,i,bankNr,inputNr);
    if (buttonState[bankNr*8+inputNr]) {
      patch=i+1;
      MIDImessage2(instrumentSelect,patch);
      return;
    }
  }
  // top row
  for (int i=0;i<4;i++) {
    coordinateToSensor(3,i+1,bankNr,inputNr);
    if (buttonState[bankNr*8+inputNr]) {
      patch=i+11;
      MIDImessage2(instrumentSelect,patch);
      return;
    }
  }
}

// binary!
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
  //if (newPatchNr>0) {
    patch=newPatchNr;
    MIDImessage2(instrumentSelect,patch);
  //}
}

// Play or stop the note of a specific treble button number, given pull state. Keeps internally track of number of times a specific note is pressed.
// Returns noteNumber. Is 0 if no note is assigned to the specific button, given the current mojcaMode and myPullState.
int turnTrebleButton(bool noteOn, int buttonNr, int myPullState) {
  // buttonNr: 0..47
  int bankNr = buttonNr/8;
  int inputNr = buttonNr%8;
  int row, column;
  sensorToCoordinate(bankNr, inputNr, row, column);

  int noteNumber = getNoteNumber(myPullState,row,column);
  MIDImessage(noteOn?noteON:noteOFF, noteNumber, noteOn?NOTE_ON_VELOCITY:NOTE_OFF_VELOCITY);
  return noteNumber;
}

// Play or stop the note of a specific bass button number, given pull state. Keeps internally track of number of times a specific note is pressed.
void turnBassButton(bool noteOn, int buttonNr, int myPullState) {
  // send note off on oldPullState and note on on pull
  int bankNr = buttonNr/2;
  int inputNr = buttonNr%2;

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
  for (int buttonNr=0;buttonNr<12;buttonNr++) {
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
  for (int buttonNr=0; buttonNr<12; buttonNr++) {
    if (bassButtonState[buttonNr]) {
      turnBassButton(false,buttonNr,!pullState);
    }
  }

  // pressed bass notes on
  for (int buttonNr=0; buttonNr<12; buttonNr++) {
    if (bassButtonState[buttonNr]) {
      // send note off on oldPullState and note on on pull
      turnBassButton(true, buttonNr, pullState);
    }
  }
}

void manageTrebleButtons(int bankNr) {
  for (int inputNr=0;inputNr<8;inputNr++) {
    int digitalValue = !digitalRead(inputPinNr[inputNr]);
    int buttonNr = bankNr*8+inputNr;
    if (digitalValue != buttonState[buttonNr]) {
      buttonState[buttonNr]=digitalValue;
      int noteNumber = turnTrebleButton(buttonState[buttonNr],buttonNr,pullState);
      if ((noteNumber == 0) && digitalValue) {
        // possibly special keys to change patch
        int row, column;
        sensorToCoordinate(bankNr, inputNr, row, column);
        if (!mojcaMode && row == 3 && column == 7) {
          setPatchAccordingToButtonState();
          resetAllButtons();
        }
      }
    }
  }
}

void manageBassButtons(int bankNr) {
  // bass
  for (int inputNr=0;inputNr<BASS_INPUT_NR_COUNT;inputNr++) {
    int noteOn = digitalRead(bassInputPinNr[inputNr]);
    if (noteOn != bassButtonState[bankNr*BASS_INPUT_NR_COUNT+inputNr]) {
      bassButtonState[bankNr*BASS_INPUT_NR_COUNT+inputNr]=noteOn;
      turnBassButton(noteOn,bankNr*BASS_INPUT_NR_COUNT+inputNr,pullState);
    }
  }
}

void readButtons() {
  for (int bankNr=0;bankNr<=5;bankNr++) {
    digitalWrite(outputPinNr[bankNr],HIGH);
    delayMicroseconds(500); // to give electronics time to activate
    manageTrebleButtons(bankNr);
    manageBassButtons(bankNr);
    digitalWrite(outputPinNr[bankNr],LOW);
  }
}


void resetAllButtons() {
  turnAllTrebleButtonsOff();  
  turnAllBassButtonsOff();  
}

void loop() {
  bool newMojcaMode = analogRead(6)<512;
  if (newMojcaMode != mojcaMode) {
    resetAllButtons();
    mojcaMode = newMojcaMode;
  }

  bool newPullState = digitalRead(DIRECTION_BUTTON_PIN);
  if (newPullState != pullState) {
    pullState = newPullState;
    manageSwitchPull();
  }

  readButtons();
}

void playTestProgram() {
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


void setup() {
  Serial.begin(115200);
  pinMode(DIRECTION_BUTTON_PIN,INPUT_PULLUP); // direction pushbutton

#ifdef TEST_PROGRAM
  playTestProgram(); // this won't return
#endif
  
  // Six output pins
  for (int i=2; i<=7; i++) {
    pinMode(i,OUTPUT);
  }

  // Eight input pins
  for (int i=0;i<8;i++) {
    pinMode(inputPinNr[i],INPUT);
  }
  // Two extra input pins
  pinMode(8,INPUT);
  pinMode(10,INPUT);

  mojcaMode = analogRead(6)<512;

  for (int i=0;i<48;i++) {
    buttonState[i]=0;
  }
  for (int i=0;i<12;i++) {
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

void showAllButtons() {
  for (int bankNr=0;bankNr<=5;bankNr++) {
    digitalWrite(outputPinNr[bankNr],HIGH);
    Serial.print('#');
    Serial.print(bankNr);
    Serial.print(':');
    for (int inputNr=0;inputNr<8;inputNr++) {
      int digitalValue = !digitalRead(inputPinNr[inputNr]);
      if (PRINT_ANALOG && inputNr<6) {
        int analogValue = analogRead(inputNr);
        Serial.print(analogValue);
        Serial.print('/');
      }
      Serial.print(digitalValue);
    }
    digitalWrite(outputPinNr[bankNr],LOW);
    Serial.print(' ');
  }
  Serial.println();
}
*/
