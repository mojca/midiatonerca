#pragma once
/*
 * MIDI note definitions for lever and key harmonicas, both treble and bass sides, both Mojca and Vincent mode
 *
 * MIDI note number 69 is A4 concert pitch = 440 Hz
 * 
 * Note number 43 is G2
 * Note number 31 is G1
 * Note number 19 is G0 chord
 */




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


