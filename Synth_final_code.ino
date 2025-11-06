#define PSHIFT A0            // Y axis pin
#define KSHIFT A1            // Rotary encoder SW pin
#define ROT_A 2              // Rotary encoder CLK pin
#define ROT_B 3              // Rotary encoder DT pin
#define SPEAKER 4            // Speaker pin
#define TRANSPOSITION 5      // Transposition button pin
#define K1 6
#define K2 7   
#define K3 8
#define K4 9
#define K5 10
#define K6 11
#define K7 12
#define K8 13


int mode_matrix[6][8]{
  {261, 293, 329, 349, 392, 440, 494, 523}, // major
  {261, 293, 311, 349, 392, 415, 466, 523}, // minor
  {261, 293, 329, 370, 392, 440, 494, 523}, // lydian
  {261, 293, 329, 349, 392, 440, 466, 523}, // mixolydian
  {261, 293, 311, 349, 392, 440, 466, 523}, // dorian
  {261, 277, 311, 349, 392, 415, 466, 523}, // phyrgian
};

int notes[8] = {261, 293, 329, 349, 392, 440, 494, 523}; // starts in major mode // starts state of each button at HIGH

int note = 0;            // base note             
int mfreq = 0;           // Modified frequency, based on note
int modeNum = 0;
       // number to check if all buttons are off

int yValue = 0;           
float yFactor = 0;       // 2^(yFactor) * note = mfreq

int octaveOffset = 1;    // starting octave

float tnum = 0;    // Number of half-steps to transpose (-6 to 6)
float tfactor = 0; // transposition factor to multiply note by 2^(tfactor)

int tButtonState = 0;      // Current state of the transposition button
int tLastButtonState = 0;  // Previous state of the transposition button

int mButtonState = 0;      // Current state of the mode button
int mLastButtonState = 0;  // Previous state of the mode button

void setup() {
  pinMode(SPEAKER, OUTPUT);
  pinMode(ROT_A, INPUT);
  pinMode(ROT_B, INPUT);
  pinMode(TRANSPOSITION, INPUT_PULLUP);
  pinMode(K1, INPUT_PULLUP);
  pinMode(K2, INPUT_PULLUP);
  pinMode(K3, INPUT_PULLUP);
  pinMode(K4, INPUT_PULLUP);
  pinMode(K5, INPUT_PULLUP);
  pinMode(K6, INPUT_PULLUP);
  pinMode(K7, INPUT_PULLUP);
  pinMode(K8, INPUT_PULLUP);


  Serial.begin(9600);  //For monitoring 
}

void loop() {
  //Function prototypes
  handleButtons();        // Check button inputs for note selection
  handleJoystick();       // Modulate pitch bend with joystick
  handleRotaryEncoder();  // Change octave with rotary encoder
  handleTransposition();          // Transpose with press of a button, cycles
  handleMode();           // Changes what mode is used with rotary encoder button
  
  //Output sound

  if (note > 0) { // if a note is chosen
    tone(SPEAKER, mfreq); // Output modified frequency
  } else {
    noTone(SPEAKER);  // Stop speaker if no note is chosen
  }
}

void handleButtons() {
  int buttons[] = {K1, K2, K3, K4, K5, K6, K7, K8};
  for (int i = 7; i >= 0; i--) {
    if (digitalRead(buttons[i]) == LOW) {
      note = notes[i] * pow(2, octaveOffset) * pow(2, tfactor);
      return; 
    }
  }
  note = 0;
}


void handleJoystick() {
  yValue = analogRead(PSHIFT);  // Read y axis value
  
  yFactor = ((yValue - 526) / 526.0) / 12.0;  
  
  mfreq = note * pow(2, yFactor); 
}

void handleTransposition() {
  tButtonState = digitalRead(TRANSPOSITION); // Read the button state

  // Check if the button was pressed (transition from HIGH to LOW)
  if (tButtonState == LOW && tLastButtonState == HIGH) {
    tnum++; // Increment the number

    // Cycle back to -6 if it goes above 6
    if (tnum > 6) {
      tnum = -6;
    }
  }
  tLastButtonState = tButtonState;
  tfactor = (tnum/12);
}

void handleRotaryEncoder() {
  static int lastA = HIGH; //stores previous state of A
  int A = digitalRead(ROT_A);
  int B = digitalRead(ROT_B);
  
  if (A == LOW && lastA == HIGH) {  // Encoder rotation detected
    if (B == LOW)
    octaveOffset = constrain(octaveOffset - 1, -3, 3);  // Move down an octave going clockwise
    else octaveOffset = constrain(octaveOffset + 1, -3, 3);  // Move up an octave going counterclockwie
  }
  lastA = A; //updates A to current state
}

void handleMode() {
  mButtonState = analogRead(KSHIFT);
  if (mButtonState == LOW && mLastButtonState > 100){
    modeNum++;
    Serial.println(modeNum);
    if (modeNum > 5) {
      modeNum = 0;
    }
        // transfer notes from mode into notes martix
    for (int i = 0; i < 8; i++) {
    notes[i] = mode_matrix[modeNum][i];
    }
  }
  mLastButtonState = mButtonState;

}

