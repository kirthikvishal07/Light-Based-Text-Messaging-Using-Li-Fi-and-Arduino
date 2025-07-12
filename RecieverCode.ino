#include <LiquidCrystal.h>

const char CLEAR = 0;
const char DOT = 1;
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

const unsigned long dotDuration = 50; // duration [ms] of a dot; basic time unit
const unsigned long dashDuration = 3 * dotDuration;
const unsigned long tolerance = dotDuration; // tolerance for timing

enum State { 
  UP = 0, 
  DOWN = 1 
} state;

unsigned long lastChange;
unsigned long downDuration;
char character[5];
int characterIndex;

enum Action {
  START = 0,
  READ_DASHDOT = 1,
  READ_CHARACTER = 2,
  READ_WORD = 3
} action;

int buttonPin = 6;
LiquidCrystal lcd(12,11,5,4,3,2);
int lcdPosition = 0; // Track the cursor position on the LCD screen

void setup() {
  Serial.begin(9600);
  analogWrite(9,75);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("LiFi BASED TEXT");
  lcd.setCursor(0, 1);
  lcd.print("COMMUNICATION");
  delay(3000);
  lcd.clear();
  
  pinMode(buttonPin, INPUT_PULLUP);

  state = UP;
  characterIndex = 0;
  downDuration = 0;
  lastChange = 0;
  action = START;
}

void clearCharacter() {
  characterIndex = 0;
  for (int i = 0; i < 5; ++i) {
    character[i] = CLEAR;
  }
}

void readDashDot() {
  if (downDuration >= dashDuration - tolerance && downDuration < dashDuration + tolerance) {
    character[characterIndex] = DASH;
    Serial.println("DASH");
    characterIndex++;
  } else if (downDuration >= dotDuration - tolerance && downDuration < dotDuration + tolerance) {
    character[characterIndex] = DOT;
    Serial.println("DOT");
    characterIndex++;
  } else {
    Serial.println("Down duration unrecognized");
  }
}

char readCharacter() {
  for (int i = 0; i < 26; ++i) {
    bool found = true;
    for (int j = 0; found && j < 5; ++j) {
      if (character[j] != alphabet[i][j + 1]) {
        found = false;
      }
    }
    if (found) {
      return alphabet[i][0];
    }
  }
  return 0;
}

void loop() {
  State newState = digitalRead(buttonPin) ? UP : DOWN;

  if (newState == state) {
    if (newState == UP) {
      unsigned long upDuration = (millis() - lastChange);
     
      if (action == READ_DASHDOT && upDuration >= dotDuration - tolerance && upDuration < dotDuration + tolerance) {
        readDashDot();
        action = READ_CHARACTER;
      } else if (action == READ_CHARACTER && upDuration >= 3 * dotDuration - tolerance && upDuration < 3 * dotDuration + tolerance) {
        char c = readCharacter();
        if (c != 0) {
          Serial.print("Read character: ");
          Serial.println(c);
          lcd.print(c);
          lcdPosition++;

          if (lcdPosition == 16) {
            lcd.setCursor(0, 1); // Move cursor to the second line
          } else if (lcdPosition == 32) {
            lcd.clear(); // Clear screen if both lines are full
            lcdPosition = 0;
            lcd.setCursor(0, 0); // Reset cursor to the first line
          }
        } else {
          Serial.println("Unrecognized character");
        }
        clearCharacter();
        action = READ_WORD;
      } else if (action == READ_WORD && upDuration >= 7 * dotDuration - tolerance && upDuration < 7 * dotDuration + tolerance) {
        Serial.println("Read next word");
        lcd.print(' ');
        lcdPosition++;

        if (lcdPosition == 16) {
          lcd.setCursor(0, 1); // Move cursor to the second line
        } else if (lcdPosition == 32) {
          lcd.clear(); // Clear screen if both lines are full
          lcdPosition = 0;
          lcd.setCursor(0, 0); // Reset cursor to the first line
        }

        action = READ_DASHDOT;
      }
    } else {
      downDuration = (millis() - lastChange);
    }
  } else {
    if (state == UP && newState == DOWN) {
      downDuration = 0;
    }
    lastChange = millis();
    state = newState;
    action = READ_DASHDOT;
  }
}
