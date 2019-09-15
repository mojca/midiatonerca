/*
 * BassBoard class
 * Column pins are OUTPUTs
 * Row pins are INPUTS
 * BASS_TESTING: row pins 3, 4; column pins 5-10
 * TREBLE_TESTING: row pins 2-9, column pins A0-A5
 * ANALOG_TESTING: row pins A0..A5, column pins 2-7
 */
//#include <vector>
using namespace std;

class BassBoard {  
  public:
    static const int BASS_TESTING = 0;
    static const int TREBLE_TESTING = 1;
    static const int ANALOG_TESTING = 2;
    static const int WOODEN_HARMONICA = 10;
    static const int ACRYLIC_HARMONICA = 11;
    
    static const int MAX_ROW_COUNT = 10;
    static const int MAX_COL_COUNT = 10;

    int ROW_COUNT;
    int COL_COUNT;
    int ROW_PINS[MAX_ROW_COUNT];
    int COL_PINS[MAX_COL_COUNT];
    
    // harmonicaNr: BASS_TEST, TREBLE_TEST, WOODEN_HARMONICA, ACRYLIC_HARMONICA
    BassBoard(int harmonicaNr);
    void init();
    // Activates specific column
    void activateColumn(int c);
    int readRow(int r);
    // Copies _state to _previousState, reads buttons and copy to _state
    void readButtons();
    // Prints current state to serial
    void printState();
    // Prints current state if different from previousState
    void printStateIfChanged();
    void setAnalogReading(bool analogReading) { _analogReading=analogReading; }
  private:
    bool _analogReading; // normally false
    int _state[MAX_ROW_COUNT][MAX_COL_COUNT]; // Digital: 0=depressed, 1=pressed. Analog: sensor value 0-1023
    int _previousState[MAX_ROW_COUNT][MAX_COL_COUNT]; //
};

BassBoard::BassBoard(int harmonicaNr) : _analogReading(false) {
  if (harmonicaNr == BASS_TESTING) {
    ROW_COUNT = 2;
    COL_COUNT = 6;
    for (int i=0;i<ROW_COUNT;i++) {
      ROW_PINS[i] = 3+i;
    }
    for (int i=0;i<COL_COUNT;i++) {
      COL_PINS[i] = 5+i;
    }
  } else if (harmonicaNr == TREBLE_TESTING) {
    ROW_COUNT = 8;
    COL_COUNT = 6;
    for (int i=0;i<COL_COUNT;i++) {
      COL_PINS[i] = A0+i;
    }
    for (int i=0;i<ROW_COUNT;i++) {
      ROW_PINS[i] = 2+i;
    }
  } else if (harmonicaNr == ANALOG_TESTING) {
    ROW_COUNT = 6;
    COL_COUNT = 6;
    for (int i=0;i<COL_COUNT;i++) {
      COL_PINS[i] = 2+i;
    }
    for (int i=0;i<ROW_COUNT;i++) {
      ROW_PINS[i] = A0+i;
    }
  } else if (harmonicaNr == WOODEN_HARMONICA) {
    
  } else if (harmonicaNr == ACRYLIC_HARMONICA) {
    
  } else {
    // Unknown harmonica!
  }
}

void BassBoard::init() {
  void init();{
    for (int i=0;i<=1;i++) {
      pinMode(ROW_PINS[i],INPUT);
    }
    for (int i=0;i<COL_COUNT;i++) {
      pinMode(COL_PINS[i],OUTPUT);
      digitalWrite(COL_PINS[i],LOW);
    }
    for (int r=0;r<ROW_COUNT;r++) {
      for (int c=0;c<COL_COUNT;c++) {
        _state[r][c]=0;
        _previousState[r][c]=0;
      }
    }
  }
}

void BassBoard::activateColumn(int c) {
  digitalWrite(COL_PINS[c],HIGH);  
}

int BassBoard::readRow(int r) {
  int v;
  if (_analogReading) {
    v = analogRead(ROW_PINS[r]);
  } else {
    v = digitalRead(ROW_PINS[r]);
  }
  return v;
}

void BassBoard::readButtons() {
  for (int c=0;c<COL_COUNT;c++) {
    digitalWrite(COL_PINS[c],HIGH);
    delay(2); // milliseconds
    for (int r=0;r<ROW_COUNT;r++) {
      _previousState[r][c] = _state[r][c];
      int v;
      if (_analogReading) {
        v = analogRead(ROW_PINS[r]);
      } else {
        v = digitalRead(ROW_PINS[r]);
      }
      _state[r][c] = v;
    }
    digitalWrite(COL_PINS[c],LOW);    
  }
}

void BassBoard::printState() {
  for (int r=0;r<ROW_COUNT;r++) {
    Serial.print("Row ");
    Serial.print(r);
    Serial.print(": ");
    for (int c=0;c<COL_COUNT;c++) {
      Serial.print(_state[r][c]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println();
}

void BassBoard::printStateIfChanged() {
  bool changed=false;
  for (int r=0;r<ROW_COUNT;r++) {
    for (int c=0;c<COL_COUNT;c++) {
      if (_state[r][c] != _previousState[r][c]) {
        changed = true;
      }
    }
  }
  if (changed) {
    printState();
  }
}
