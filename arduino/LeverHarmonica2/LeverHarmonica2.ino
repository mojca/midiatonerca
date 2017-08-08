// code for diatonic accordion with 3 rows (12+13+12) buttons
// plus 7 semitones

// for driving the LEDs
#include <Adafruit_NeoPixel.h>

static const bool MIDI = true;

// 31250 // for midi instrument
//115200 // for Hairless MIDI

static const unsigned long serialSpeed = 115200;
static const int noteON = 144;  // 10010000
static const int noteOFF = 128; // 10000000
static const bool USE_TILT = false;
static const int instrumentSelect = 192; // 11000000
static const int midiInstrumentAccordion = 21;
static const unsigned long debounceTimeMs = 100;

static const int pinPixel = 2;
static const int pinPushPull = A8;
static const int nButtons = 44;
int inputPinNr[] = {
  3, 4, 5, 6, 7,
    49, 51, 53,
    46, 48, 50, 52,
  23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47,
  22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44,
  A15, A14, A13, A12, A11, A10, A9};

unsigned long lastChangeTimestamp[nButtons];
bool buttonState[nButtons];
bool pullState;

//Adafruit_NeoPixel leds = Adafruit_NeoPixel(nButtons, pinPixel, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leds = Adafruit_NeoPixel(nButtons+100, pinPixel, NEO_GRB + NEO_KHZ800);
// red color for push
uint32_t color_push = leds.Color(255, 0, 0);
// blue color for pull
uint32_t color_pull = leds.Color(0, 0, 255);

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
void sensorToCoordinate(int button, int& row, int& column)
{
  if(button < 12) {
    row = 0;
    column = button;
  } else if(button < 25) {
    row = 1;
    column = button - 12;
  } else if(button < 37) {
    row = 2;
    column = button - 25;
  } else {
    row = 3;
    column = button - 37 + 1;
  }
}


void setup() {
  if (MIDI) {
    //Serial.begin(31250); // for midi instrument
    Serial.begin(serialSpeed); // for Hairless MIDI
    
    MIDImessage2(instrumentSelect,midiInstrumentAccordion);
  } else {
    Serial.begin(9600);
  }

  leds.begin();
  leds.show(); // Initialize all pixels to 'off'
  unsigned long timestamp = millis();

  for(int i=0; i<nButtons; i++) {
    pinMode(inputPinNr[i], INPUT_PULLUP);
    buttonState[i]=0;
    lastChangeTimestamp[i] = timestamp;
    leds.setPixelColor(i, leds.Color(0, 0, 0));
  }
  pinMode(pinPushPull, INPUT_PULLUP);
  pullState = 0;

  leds.setPixelColor(18, leds.Color(0, 255, 0));
  leds.show();

  /*
  delay(1000);
  leds.show();
120
  leds.setPixelColor(25, leds.Color(0, 255, 0));
  leds.setPixelColor(26, leds.Color(255, 0, 0));
  leds.setPixelColor(27, leds.Color(0, 0, 255));
  delay(1000);
  leds.show();

  leds.setPixelColor(37, leds.Color(0, 255, 0));
  leds.setPixelColor(38, leds.Color(255, 0, 0));
  leds.setPixelColor(39, leds.Color(0, 0, 255));
  delay(1000);
  leds.show();
  */
}

void loop() {
  if (MIDI) {
    sendMidi();
  } else {
    updateButtonState();
  }
}

void updateButtonState() {
  // TODO
  bool pull = false;
  unsigned long timestamp = millis();


  /*
  bool pull = difitalRead(xxx);
  if (pull != pullState) {
    // off&on all pressed buttons
    Serial.println("##");
    pullState = pull;    
  }
  */
  for(int i=0; i<nButtons; i++) {
    int oldstate = buttonState[i];
    buttonState[i] = !digitalRead(inputPinNr[i]);
    if (buttonState[i] != oldstate) {
      //if (timestamp < lastChangeTimestamp[i] || (timestamp - lastChangeTimestamp[i] > debounceTimeMs)) {

      int row, column;
      sensorToCoordinate(i, row, column);
      int noteNumber = getNoteNumber(pull,row,column);

      Serial.print("button change ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(buttonState[i]);
      Serial.print(" ");
      Serial.print(noteNumber);
      Serial.println("");
    }
  }
}

// writes to midi
void sendMidi() {
  bool pull = digitalRead(pinPushPull);;
  unsigned long timestamp = millis();

  //pull = digitalRead(9);
  if (pull != pullState) {
    // Switch detected
    // all notes off
    MIDImessage3(176,123,0);
    MIDImessage3(176,120,0); // doesn't seem to help either

    for (int i=0; i<nButtons; i++) {
      if (buttonState[i]) {
        int row, column;
        sensorToCoordinate(i, row, column);
        int noteNumber = getNoteNumber(pull,row,column);
        MIDImessage(noteON, noteNumber, 100);
      }
    }
    pullState = pull;
  }

  for(int i=0; i<nButtons; i++) {
    int oldstate = buttonState[i];
    int newstate = !digitalRead(inputPinNr[i]);
    if (newstate != oldstate) {
      if (timestamp < lastChangeTimestamp[i] || (timestamp - lastChangeTimestamp[i] > debounceTimeMs)) {
        buttonState[i] = newstate;
        lastChangeTimestamp[i] = timestamp;
        int row, column;
        sensorToCoordinate(i, row, column);
        int noteNumber = getNoteNumber(pull,row,column);

        if (noteNumber>0) {
          if (buttonState[i]) {
            MIDImessage(noteON, noteNumber, 100);
          } else {
            MIDImessage(noteOFF, noteNumber, 127);
          }
        }
      }
    }
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

