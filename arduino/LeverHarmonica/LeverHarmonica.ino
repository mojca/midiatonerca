// 13 columns, four rows

static const bool MIDI = true;
static const int noteON = 144;  // 10010000
static const int noteOFF = 128; // 10000000
static const bool USE_TILT = false;
static const int instrumentSelect = 192; // 11000000

static const int PATCH_COUNT = 14;
int patch = 1;

// Test optical gates
// Output: D2..D7 for column 0..5
// Input: A0..A5, D11, D12 for sensor 0..7

static const bool PRINT_ANALOG = false;

int inputPinNr[] = {A0,A1,A2,A3,A4,A5,11,12};
int outputPinNr[] = {2,3,4,5,6,7};

bool buttonState[48];//bankNr*8+inputNr
bool pullState;

bool mojcaMode = true;

/* PUSH
    {--,ds,as,gs,es,gs,ds,ds,--,--,--,--,--},
   {f0,bs,d1,f1,bs,d2,f2,bs,d3,f3,bs,d4,--},
  {c0,f0,a0,c1,f1,a1,c2,f2,a2,c3,f3,a3,c4},
   {c0,e0,g0,c1,e1,g1,c2,e2,g2,c3,e3,g3,--}
*/
byte pushNoteNumber[4][13]={
    { 0, 0, 0,54,63, 0, 0, 0, 0, 0, 0, 0, 0},
   {41,46,50,53,58,62,65,70,74,77,82,86, 0},
  {36,41,45,48,53,57,60,65,69,72,77,81,84},
   {36,40,43,48,52,55,60,64,67,72,76,79, 0}
};


/* PULL
    {--,as,gs,ds,ds,gs,as,ds,--,--,--,--,--}
   {a0,es,f1,a1,c2,es,f2,a2,c3,es,g3,a3,--}
  {e0,bs,c1,e1,g1,bs,c2,e2,g2,bs,d3,e3,g4}
   {f0,g0,b0,d1,f1,a1,b1,d2,f2,a2,b2,d3,--}
 */

// MIDI note numbers: octave+3
byte pullNoteNumber[4][13]={
    { 0, 0, 0,49,61, 0, 0, 0, 0, 0, 0, 0, 0},
   {45,51,53,57,60,63,65,69,72,75,79,81, 0},
  {40,46,48,52,55,58,60,64,67,70,72,76,79},
   {41,43,47,50,53,57,59,62,65,69,71,74, 0}
};


// Vincent's two-row harmonica
/* PUSH:
  (D#C F A D F A C F A)
 (F#G C E G C E G C E G)
*/

byte vincentPushNoteNumber[4][13]={
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {63,48,53,57,62,65,69,72,77,81, 0, 0, 0},
  {54,43,48,52,55,60,64,67,72,76,79, 0, 0},
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/* PULL:
  (C#E G BbC E G BbC E)
 (G#B D F A B D F A B D) 
*/
byte vincentPullNoteNumber[4][13]={
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   {61,52,55,58,60,64,67,70,72,76, 0, 0, 0},
  {56,47,50,53,57,59,62,65,69,71,74, 0, 0},
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
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

void setup() {
  pinMode(8,INPUT_PULLUP); // tilt sensor
  pinMode(9,INPUT_PULLUP); // direction pushbutton
  if (MIDI) {
    if (!digitalRead(9)) {
      Serial.begin(115200); // for Hairless MIDI
    } else {
      Serial.begin(31250); // for midi instrument
    }

    MIDImessage2(instrumentSelect,patch);
  } else {
    Serial.begin(9600);
  }
  
  // Six output pins
  for (int i=2; i<=7; i++) {
    pinMode(i,OUTPUT);
  }

  // Eight input pins
  for (int i=0;i<8;i++) {
    pinMode(inputPinNr[i],INPUT);
  }
  for (int i=0;i<48;i++) {
    buttonState[i]=0;
  }
  pullState = 0;
  /*
  if (!MIDI) {
    if (mojcaMode) {
      Serial.println("Four row, ...");
    } else {
      Serial.println("Two row, CG");
    }
  }*/
}

void loop() {
  //oldMojcaMode = mojcaMode;
  mojcaMode = analogRead(6)<512;
  //if (MIDI) {
    sendMidi();
  //} else {
  //  updateButtonState();
  //}
  //showAllButtons();
  //showAnalog();  
}

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


// writes to midi
void sendMidi() {
  bool pull;
  if (USE_TILT) {
    pull = !digitalRead(8);
  } else {
    pull = digitalRead(9);
  }
  if (pull != pullState) {
    // Switch detected
    // all notes off? No, doesn't work with all synths...
    //MIDImessage3(176,123,0);
    
    for (int buttonNr=0; buttonNr<48; buttonNr++) {
      if (buttonState[buttonNr]) {
        // send note off on pullState and note on on pull
        int bankNr = buttonNr/8;
        int inputNr = buttonNr%8;
        int row, column;
        sensorToCoordinate(bankNr, inputNr, row, column);

        int noteNumber = getNoteNumber(!pull,row,column);
        MIDImessage(noteOFF, noteNumber, 100);
        
        noteNumber = getNoteNumber(pull,row,column);
        MIDImessage(noteON, noteNumber, 100);
      }
    }
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
/*        if (pull) {
          noteNumber = pullNoteNumber[3-row][column];
        } else {
          noteNumber = pushNoteNumberse[3-row][column];
        }*/
        if (noteNumber>0) {
          if (digitalValue) {
            MIDImessage(noteON, noteNumber, 100);
          } else {
            MIDImessage(noteOFF, noteNumber, 127);
          }
        } else {
          // special keys
          // change patch
          if (digitalValue) {
            if (row == 0 && (10 <= column && column <= 11)) {
              if (column == 10) {
                patch = (patch-2+PATCH_COUNT)%PATCH_COUNT+1;
              } else {
                patch=(patch)%PATCH_COUNT+1;
              }
//              MIDImessage2(instrumentSelect,0);
              // indicate patch by number of blinks (+1)
              for (int i=0;i<patch-1;i++) {
//                MIDImessage3(noteON,60,100);
//                delay(100);
                MIDImessage3(noteOFF,60,100);
                delay(200);
              }             
              MIDImessage2(instrumentSelect,patch);
            }
          }
        }
      }
    }
    digitalWrite(outputPinNr[bankNr],LOW);
  }
}

int getNoteNumber(bool pull, int row, int column) {
  if (mojcaMode) {
    if (pull) {
      return pullNoteNumber[3-row][column];
    } else {
      return pushNoteNumber[3-row][column];
    }
  } else {
    if (pull) {
      return vincentPullNoteNumber[3-row][column];
    } else {
      return vincentPushNoteNumber[3-row][column];
    }
  }
}

void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  Serial.write(command); //send note on or note off command
  Serial.write(MIDInote); //send pitch data
  Serial.write(MIDIvelocity); //send velocity data
}

void MIDImessage2(int command, int data) {
  Serial.write(command);
  Serial.write(data);
}

void MIDImessage3(int command, int data, int data2) {
  Serial.write(command);
  Serial.write(data);
  Serial.write(data2);
}

