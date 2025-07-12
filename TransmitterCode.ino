// ▶️ Transmitter
// •	Laser signal → D13
// •	Serial input via PC
// •	Laser module: VCC → 5V, GND → GND

const int laserPin = 13;  // Laser connected to digital pin 13
const unsigned long dotDuration = 50;  // Duration of a dot in milliseconds
const unsigned long dashDuration = 3 * dotDuration;
const unsigned long intraCharGap = dotDuration; // Time between dots and dashes within a character
const unsigned long interCharGap = 3 * dotDuration; // Time between characters
const unsigned long wordGap =                                                                                                                                                                                                                                                     7 * dotDuration; // Time between words

const char CLEAR = 0;
const char DOT = 3;
const char DASH = 2;
const char alphabet[26][6] {
  { 'A', DOT, DASH, CLEAR, CLEAR, CLEAR},
  { 'B', DASH, DOT, DOT, DOT, CLEAR},
  { 'C', DASH, DOT, DASH, DOT, CLEAR},
  { 'D', DASH, DOT, DOT, CLEAR, CLEAR},
  { 'E', DOT, CLEAR, CLEAR, CLEAR, CLEAR},
  { 'F', DOT, DOT, DASH, DOT, CLEAR},
  { 'G', DASH, DASH, DOT, CLEAR, CLEAR},
  { 'H', DOT, DOT, DOT, DOT, CLEAR},
  { 'I', DOT, DOT, CLEAR, CLEAR, CLEAR},
  { 'J', DOT, DASH, DASH, DASH, CLEAR},
  { 'K', DASH, DOT, DASH, CLEAR, CLEAR},
  { 'L', DOT, DASH, DOT, DOT, CLEAR},
  { 'M', DASH, DASH, CLEAR, CLEAR, CLEAR},
  { 'N', DASH, DOT, CLEAR, CLEAR, CLEAR},
  { 'O', DASH, DASH, DASH, CLEAR, CLEAR},
  { 'P', DOT, DASH, DASH, DOT, CLEAR},
  { 'Q', DASH, DASH, DOT, DASH, CLEAR},
  { 'R', DOT, DASH, DOT, CLEAR, CLEAR},
  { 'S', DOT, DOT, DOT, CLEAR, CLEAR},
  { 'T', DASH, CLEAR, CLEAR, CLEAR, CLEAR},
  { 'U', DOT, DOT, DASH, CLEAR, CLEAR},
  { 'V', DOT, DOT, DOT, DASH, CLEAR},
  { 'W', DOT, DASH, DASH, CLEAR, CLEAR},
  { 'X', DASH, DOT, DOT, DASH, CLEAR},
  { 'Y', DASH, DOT, DASH, DASH, CLEAR},
  { 'Z', DASH, DASH, DOT, DOT, CLEAR}
};

void setup() {
  pinMode(laserPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.toUpperCase();
    for (int i = 0; i < input.length(); i++) {
      char c = input.charAt(i);
      if (c == ' ') {
        delay(wordGap);
      } else {
        sendMorseCode(c);
        delay(interCharGap);
      }
    }
    delay(1000); // Pause before allowing new input
  }
}

void sendMorseCode(char c) {
  int index = -1;
  for (int i = 0; i < 26; i++) {
    if (alphabet[i][0] == c) {
      index = i;
      break;
    }
  }
  if (index != -1) {
    for (int j = 1; j < 6; j++) {
      if (alphabet[index][j] == DOT) {
        digitalWrite(laserPin, HIGH);
        delay(dotDuration);
        digitalWrite(laserPin, LOW);
        delay(intraCharGap);
      } else if (alphabet[index][j] == DASH) {
        digitalWrite(laserPin, HIGH);
        delay(dashDuration);
        digitalWrite(laserPin, LOW);
        delay(intraCharGap);
      } else {
        break;
      }
    }
  }
}
