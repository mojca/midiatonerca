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
 */

/*
 * Patches:
 * 1. SpecialRecording
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
 * 15. Accordion
 *
 */

// if true then a simplistic test program will run
//#define TEST_PROGRAM

// true to send notes to Raspberry PI at 31250, false to send to console at 115200
const bool MIDI = true;



const int PATCH_COUNT = 15;
int isSpecialInstrument[PATCH_COUNT] = {true,false,false,false,false,false,false,false,false,false,false,false,false,false}; // 0-based
int trebleNoteOffsetPerPatch[PATCH_COUNT] = {0,12,24,24,24,24,12,12,24,12,12,12,12,12,0}; // 0-based
int bassNoteOffsetPerPatch[PATCH_COUNT] = {0,12,24,24,24,24,12,12,24,12,12,12,12,12,0}; // 0-based

int notePushedCount[256] = {0}; // to handle multiple buttons for one note

// 13 columns, four rows

#define BASS_INPUT_NR_COUNT 2 // per bank
/*
#define ANY_BUTTON_PUSHED_LED 8
#define DIRECTION_LED 10
*/

#define NOTE_ON_VELOCITY 100
#define NOTE_OFF_VELOCITY 127

#define DIRECTION_BUTTON_PIN 10

// true to send notes to Raspberry PI, false to send to console, both at 115200
const bool MIDI = true;
const int noteON = 144;  // 10010000
const int noteOFF = 128; // 10000000
//const bool USE_TILT = false;
const int instrumentSelect = 192; // 11000000


int patch = 1; // 1-based, so subtract one for array indexing

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



//#define TREBLE_NOTE_OFFSET 12
//#define BASS_NOTE_OFFSET 0

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
    {midi_C3,  midi_E3,  midi_G3}, {midi_C2, 0, 0},
    {midi_F3,  midi_A3,  midi_C4}, {midi_F2, 0, 0},
    {0,        0,        0      }, {0,       0, 0}
  }, {
    {midi_A2,  midi_Cis3, midi_E3}, {midi_A1, 0, 0},
    {midi_B2,  midi_D3,   midi_F3}, {midi_B1, 0, 0},
    {0,        0,         0      }, {0,       0, 0}
  }
};

/*
  BASS PULL:
  g  G  c  C x x
   dm D bb Bb x x
 */
byte vincentBassPullNoteNumber[2][6][3]={
  {
    {midi_G3,  midi_H3,  midi_D4}, {midi_G2, 0, 0},
    {midi_C3,  midi_E3,  midi_G3}, {midi_C2, 0, 0},
    {0,        0,        0      }, {0,       0, 0}
  }, {
    {midi_D3,  midi_F3,  midi_A3}, {midi_D2, 0, 0},
    {midi_B2,  midi_D3,  midi_F3}, {midi_B1, 0, 0},
    {0,        0,        0      }, {0,       0, 0}
  }
};


static const byte midi_C   = 36; // C2
static const byte midi_Cis = 37; // C#2
static const byte midi_D   = 38; // D2
static const byte midi_Dis = 39; // D#2
static const byte midi_Es  = 39; // Eb2
static const byte midi_E   = 40; // E2
static const byte midi_F   = 41; // F2
static const byte midi_Fis = 42; // F#2
static const byte midi_G   = 43; // G2     should be G1
static const byte midi_Gis = 44; // G#2
static const byte midi_A   = 45; // A2
static const byte midi_B   = 46; // Bb2
static const byte midi_H   = 47; // B2

static const byte octave = 12;

static const byte midi_chord_A0_special  = midi_A   - 2 * octave;
static const byte midi_chord_B0_special  = midi_B   - 2 * octave;
static const byte midi_chord_C1_special  = midi_C   - 2 * octave;
static const byte midi_chord_D1_special  = midi_D   - 2 * octave;
static const byte midi_chord_Es1_special = midi_Es  - 2 * octave;
static const byte midi_chord_F1_special  = midi_F   - 2 * octave;
static const byte midi_chord_G1_special  = midi_G   - 2 * octave;
static const byte midi_chord_D1m_special = midi_Cis - 2 * octave;

#define midi_chord_B0  {midi_B  + 0*octave, midi_D + octave, midi_F +   octave}
#define midi_chord_C1  {midi_C  +   octave, midi_E + octave, midi_G +   octave}
#define midi_chord_Es1 {midi_Es +   octave, midi_G + octave, midi_B +   octave}
#define midi_chord_F1  {midi_F  +   octave, midi_A + octave, midi_C + 2*octave}
#define midi_chord_G1  {midi_G  +   octave, midi_H + octave, midi_D + 2*octave}
#define midi_chord_D1m {midi_D  +   octave, midi_F + octave, midi_A +   octave}

/*
  0 1 2 3 4 5     inputNr 0
   0 1 2 3 4 -     inputNr 1

  BASS PUSH:
  c  C  f  F  b  B
   G  a  A  d  D
*/
byte mojcaBassPushNoteNumber[2][6][3]={
  {
    midi_chord_C1, {midi_C, 0, 0},
    midi_chord_F1, {midi_F, 0, 0},
    midi_chord_B0, {midi_B - octave, 0, 0}
  }, {
                                                             {midi_G, 0, 0},
    {midi_A + 0*octave, midi_Cis + octave, midi_E + octave}, {midi_A - octave, 0, 0},
    {midi_D +   octave, midi_Fis + octave, midi_A + octave}, {midi_D, 0, 0}, {0, 0, 0}
  }
};

/*
  BASS PULL:
  g   G   c   C   f   F
    E   dm  D   es  Es
 */
byte mojcaBassPullNoteNumber[2][6][3]={
  {
    midi_chord_G1, {midi_G, 0, 0},
    midi_chord_C1, {midi_C, 0, 0},
    midi_chord_F1, {midi_F, 0, 0}
  },
  {
    {midi_E, 0, 0},
    midi_chord_D1m, {midi_D,  0, 0},
    midi_chord_Es1, {midi_Es, 0, 0}, {0,0,0}
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
    midi_chord_C1_special, midi_C,
    midi_chord_F1_special, midi_F,
    midi_chord_B0_special, midi_B - octave,
  }, {
                           midi_G,
    midi_chord_A0_special, midi_A - octave,
    midi_chord_D1_special, midi_D, 0,
  }
};

byte specialMojcaBassPullNoteNumber[2][6]={
  {
    midi_chord_G1_special, midi_G,
    midi_chord_C1_special, midi_C,
    midi_chord_F1_special, midi_F
  },
  {
    midi_E,
    midi_chord_D1m_special, midi_D,
    midi_chord_Es1_special, midi_Es, 0
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
    int noteOffset = trebleNoteOffsetPerPatch[patch-1];
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
    int noteOffset = trebleNoteOffsetPerPatch[patch-1];
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
    //int noteOffset = bassNoteOffsetPerPatch[patch-1];
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
    // not yet implemented!
  }
}

int* getBassNoteNumbers(bool pull, int bankNr, int inputNr) {
  static int noteNumbers[3];
  for (int i=0;i<3;i++) {
    if (mojcaMode) {
      int noteOffset = bassNoteOffsetPerPatch[patch-1];//-12;
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
      int noteOffset = bassNoteOffsetPerPatch[patch-1];
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
    if (command == noteON) {
      notePushedCount[MIDInote]++;
    } else if (command == noteOFF) {
      notePushedCount[MIDInote]--;
    }
    Serial.write(command); //send note on or note off command
    Serial.write(MIDInote); //send pitch data
    Serial.write(MIDIvelocity); //send velocity data
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



void showAnalog() {
//  for (int bankNr=0;bankNr<=5;bankNr++) {
//    digitalWrite(outputPinNr[bankNr],HIGH);
//  }
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
      //Serial.print(' ');
    }
    digitalWrite(outputPinNr[bankNr],LOW);
    Serial.print(' ');
  }
  Serial.println();
}







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
}

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
  if (newPatchNr>0) {
    patch=newPatchNr;
    MIDImessage2(instrumentSelect,patch);
  }
}

void manageSwitchPull() {
  // Switch detected
  // all notes off? No, doesn't work with all synths...
  //MIDImessage3(176,123,0);

  for (int buttonNr=0; buttonNr<48; buttonNr++) {
    if (buttonState[buttonNr]) {
      // send note off on oldPullState and note on on pull
      int bankNr = buttonNr/8;
      int inputNr = buttonNr%8;
      int row, column;
      sensorToCoordinate(bankNr, inputNr, row, column);

      int noteNumber = getNoteNumber(!pullState,row,column);
      MIDImessage(noteOFF, noteNumber, NOTE_OFF_VELOCITY);

      noteNumber = getNoteNumber(pullState,row,column);
      MIDImessage(noteON, noteNumber, NOTE_ON_VELOCITY);
    }
  }

  for (int buttonNr=0; buttonNr<12; buttonNr++) {
    if (bassButtonState[buttonNr]) {
      // send note off on oldPullState and note on on pull
      int bankNr = buttonNr/2;
      int inputNr = buttonNr%2;

      //int noteNumber = getBassNoteNumber(!pullState,bankNr,inputNr);

      if (isSpecialInstrument[patch-1]) {
        // special instrument with single note for chords
        int noteNumber = getSpecialBassNoteNumber(!pullState,bankNr,inputNr);
        MIDImessage(noteOFF, noteNumber, NOTE_OFF_VELOCITY);

        noteNumber = getSpecialBassNoteNumber(pullState,bankNr,inputNr
        );
        MIDImessage(noteON, noteNumber, NOTE_ON_VELOCITY);
      } else {
        // normal instruments
        int* noteNumbers = getBassNoteNumbers(!pullState,bankNr,inputNr);
        for (int noteSubNr=0;noteSubNr<3;noteSubNr++) {
          if (noteNumbers[noteSubNr]>0) {
            MIDImessage(noteOFF, noteNumbers[noteSubNr], NOTE_OFF_VELOCITY);
          }
        }

        noteNumbers = getBassNoteNumbers(pullState,bankNr,inputNr);
        for (int noteSubNr=0;noteSubNr<3;noteSubNr++) {
          if (noteNumbers[noteSubNr]>0) {
            MIDImessage(noteON, noteNumbers[noteSubNr], NOTE_ON_VELOCITY);
          }
        }
      }
    }
  }
}

void manageTrebleButtons(int bankNr) {
  for (int inputNr=0;inputNr<8;inputNr++) {
    int digitalValue = !digitalRead(inputPinNr[inputNr]);
    if (digitalValue != buttonState[bankNr*8+inputNr]) {
      buttonState[bankNr*8+inputNr]=digitalValue;

      int row, column;
      sensorToCoordinate(bankNr, inputNr, row, column);
      int noteNumber = getNoteNumber(pullState,row,column);
      if (noteNumber>0) {
        if (digitalValue) {
          MIDImessage(noteON, noteNumber, NOTE_ON_VELOCITY);
        } else {
          MIDImessage(noteOFF, noteNumber, NOTE_OFF_VELOCITY);
        }
      } else {
        if (digitalValue) {
          // special keys
          // change patch
          if (row == 0 && (10 <= column && column <= 11)) {
            incrementDecrementPatch(column);
          } else if (row == 3 && column == 7) {
            // set patch according to button state
            setPatchAccordingToButtonState();
          }
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
      if (!MIDI) {
        Serial.print("Bass change detected: bankNr=");
        Serial.print(bankNr);
        Serial.print(" inputNr=");
        Serial.print(inputNr);
        Serial.print(" noteOn=");
        Serial.println(noteOn);
      }
      bassButtonState[bankNr*BASS_INPUT_NR_COUNT+inputNr]=noteOn;

      if (isSpecialInstrument[patch-1]) {
        // special instrument with single note for chords
        int noteNumber = getSpecialBassNoteNumber(pullState,bankNr,inputNr);
        if (noteOn) {
          MIDImessage(noteON, noteNumber, NOTE_ON_VELOCITY);
        } else {
          MIDImessage(noteOFF, noteNumber, NOTE_OFF_VELOCITY);
        }
      } else {
        int* noteNumbers = getBassNoteNumbers(pullState,bankNr,inputNr);
        for (int noteSubNr=0;noteSubNr<3;noteSubNr++) {
          if (noteNumbers[noteSubNr]>0) {
            if (noteOn) {
              MIDImessage(noteON, noteNumbers[noteSubNr], NOTE_ON_VELOCITY);
            } else {
              MIDImessage(noteOFF, noteNumbers[noteSubNr], NOTE_OFF_VELOCITY);
            }
          }
        }
      }
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


// writes to midi
void sendMidi() {
  bool pull;
  //if (USE_TILT) {
  //  pull = !digitalRead(8);
  //} else {
  //}
  pull = digitalRead(DIRECTION_BUTTON_PIN);

  if (pull != pullState) {
    pullState = pull;
    manageSwitchPull();
  }

  readButtons();
}

void loop() {
  mojcaMode = analogRead(6)<512;
  sendMidi();
  //manageRedGreenLeds();
}

void playTestProgram() {
  pinMode(13,OUTPUT);
  bool led=true;

  int myPause=1000;
  while (1) {
    digitalWrite(13,led);
    led=!led;
    //MIDImessage2(instrumentSelect,1);
    //delay(1000);
/*
    // C0 on
    MIDImessage(noteON, 24, NOTE_ON_VELOCITY); // noteON = 144, NOTE_ON_VELOCITY=100      [144, 24, 100]   but it reads it as [255, 32, 48]
    delay(myPause);
 
    // C0 off  
    MIDImessage(noteOFF, 24, NOTE_OFF_VELOCITY); // noteOFF = 128, NOTE_OFF_VELOCITY=127    [128, 24, 127]  but it reads it as [255, 0, 48]
    delay(myPause);
*/
/*
    // C2 on
    MIDImessage(noteON, 48, NOTE_ON_VELOCITY);
    delay(myPause);
  
    // C2 off  
    MIDImessage(noteOFF, 48, NOTE_OFF_VELOCITY);
    delay(myPause);
*/
    // C4 on
    MIDImessage(noteON, 72, NOTE_ON_VELOCITY);
    delay(myPause);
  
    // C4 off  
    MIDImessage(noteOFF, 72, NOTE_OFF_VELOCITY);
    delay(myPause);
/*
    // C6 on
    MIDImessage(noteON, 96, NOTE_ON_VELOCITY);
    delay(myPause);
  
    // C6 off  
    MIDImessage(noteOFF, 96, NOTE_OFF_VELOCITY);
    delay(myPause);
*/    
  }
}


void setup() {
  /*
  if (USE_TILT) {
    pinMode(8,INPUT_PULLUP); // tilt sensor
  }*/
  pinMode(DIRECTION_BUTTON_PIN,INPUT_PULLUP); // direction pushbutton
  /*
  pinMode(ANY_BUTTON_PUSHED_LED,OUTPUT);
  pinMode(DIRECTION_LED,OUTPUT);
  */


  if (MIDI) {
    /*
    if (!digitalRead(9)) {
      Serial.begin(115200); // for Hairless MIDI
    } else {*/
      //Serial.begin(31250); // for midi instrument
      //Serial.begin(38400); // for midi instrument
      Serial.begin(115200);
    //}
    //MIDImessage2(instrumentSelect,patch);
  } else {
    Serial.begin(115200);
  }

#ifdef TEST_PROGRAM
  playTestProgram();
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
      Serial.println("Four row, ...");
    } else {
      Serial.println("Two row, CG");
    }
  }
}




// writes to console
/*
void updateButtonState() {
  bool pull;
  if (USE_TILT) {
    pull = !digitalRead(8);
  } else {
    pull = digitalRead(9);
  }
  if (pull != pullState) {
    // off&on all pressed buttons
    Serial.println("##");
    pullState = pull;
  }

  for (int bankNr=0;bankNr<=5;bankNr++) {
    digitalWrite(outputPinNr[bankNr],HIGH);
    delayMicroseconds(500); // to give electronics time to activate
    for (int inputNr=0;inputNr<8;inputNr++) {
      int digitalValue = !digitalRead(inputPinNr[inputNr]);
      if (digitalValue != buttonState[bankNr*8+inputNr]) {
        buttonState[bankNr*8+inputNr]=digitalValue;

        int row, column;
        sensorToCoordinate(bankNr, inputNr, row, column);
        int noteNumber = getNoteNumber(pull,row,column);
        /*
        if (pull >= 1) {
          noteNumber = pullNoteNumber[3-row][column];
        } else {
          noteNumber = pushNoteNumber[3-row][column];
        }* /

        if (false) {
          Serial.print("Bank ");
          Serial.print(bankNr);
          Serial.print(" input ");
          Serial.print(inputNr);
          Serial.print(" row ");
          Serial.print(row);
          Serial.print(" column ");
          Serial.print(column);
          if (pull >= 1) {
            Serial.print(" pull");
          } else {
            Serial.print(" push");
          }
          Serial.print(" note number ");
          Serial.print(noteNumber);
          Serial.print(" ");
        }

        bool printNote = digitalValue;

        if (noteNumber>0) {
          int noteIndex = noteNumber%12;
          int octave = noteNumber/12-3;

          if (printNote) {
            Serial.print(noteNames[noteIndex*2]);
            char ch = noteNames[noteIndex*2+1];
            if (ch != ' ') {
              Serial.print(ch);
            }
            Serial.print(octave);

            if (digitalValue) {
              Serial.print(" on");
            } else {
              Serial.print(" off");
            }
            Serial.println();
          }
        } else {
          Serial.println("Invalid button");
        }
      }
    }
    digitalWrite(outputPinNr[bankNr],LOW);
  }
}*/



  //if (MIDI) {
  //} else {
  //  updateButtonState();
  //}
  //showAllButtons();
  //showAnalog();
