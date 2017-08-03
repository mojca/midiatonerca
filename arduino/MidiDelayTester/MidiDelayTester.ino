/*
 * MIDI delay tester for lever harmonica
 * The red button triggersa note. That's all!
 */

static const int noteON = 144;  // 10010000
static const int noteOFF = 128; // 10000000
static const int instrumentSelect = 192; // 11000000


void setup() {
  //Serial.begin(31250); // for midi instrument
  Serial.begin(115200); // for Hairless MIDI
  MIDImessage2(instrumentSelect,21);
  
  pinMode(9,INPUT_PULLUP); // direction pushbutton
}

void loop() {
  static bool wasPressed = false;
  bool pressed = !digitalRead(9);
  if (pressed != wasPressed) {
    if (pressed) {
      MIDImessage3(noteON, 60, 100);
    } else {
      MIDImessage3(noteOFF, 60, 100);
    }
    wasPressed = pressed;
  }
}

// for MIDI messages that use two bytes
void MIDImessage2(int command, int data) {
  Serial.write(command);
  Serial.write(data);
}

// for MIDI messages that use three bytes
void MIDImessage3(int command, int data, int data2) {
  Serial.write(command);
  Serial.write(data);
  Serial.write(data2);
}

