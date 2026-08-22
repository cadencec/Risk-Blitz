//////////////////////////////////////////////////
              //  LICENSE  //
//////////////////////////////////////////////////
#pragma region LICENSE
/*
  ************************************************************************************
  * MIT License
  *
  * Copyright (c) 2025 Crunchlabs LLC (LabelMaker Code)

  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is furnished
  * to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  *
  ************************************************************************************
*/
#pragma endregion LICENSE

//////////////////////////////////////////////////
              //  LIBRARIES  //
//////////////////////////////////////////////////
#pragma region LIBRARIES
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

#pragma endregion LIBRARIES

//////////////////////////////////////////////////
          //  PINS AND PARAMETERS  //
//////////////////////////////////////////////////
#pragma region PINS AND PARAMS

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 for a 16x2 display

ezButton button1(14); //joystick button handler
#define INIT_MSG "Initializing..." // Text to display on startup
#define MODE_NAME "   RISK BLITZ   " //these are variables for the text which is displayed in different menus. 
#define PRINT_CONF " ARE YOU SURE?  " //try changing these, or making new ones and adding conditions for when they are used
#define MENU "ATTACK   DEFEND" //this one clears the menu for editing
#define R_ATCK "RESULTS: ATTACK "
#define R_DEF "RESULTS: DEFEND "
#define R_TROOP "TROOPS LEFT: "


// Joystick setup
const int joystickXPin = A2;  // Connect the joystick X-axis to this analog pin
const int joystickYPin = A1;  // Connect the joystick Y-axis to this analog pin
const int joystickButtonThreshold = 200;  // Adjust this threshold value based on your joystick

// Menu parameters
const char nums[] = "_0123456789";
int numsSize = sizeof(nums) - 1;
String text;  // Store the label text

int currentCharacter = 0; //keep track of which character is currently displayed under the cursor
int cursorPosition = 0; //keeps track of the cursor position (left to right) on the screen
int currentPage = 0; //keeps track of the current page for menus
const int charactersPerPage = 16; //number of characters that can fit on one row of the screen



// Creates states to store what the current menu and joystick states are
// Decoupling the state from other functions is good because it means the sensor / screen aren't hardcoded into every single action and can be handled at a higher level
enum State { MainMenu, EditingAttack, EditingDefend, PrintConfirmation, Results };
State currentState = MainMenu;
State prevState = Results;

enum jState {LEFT, RIGHT, UP, DOWN, MIDDLE, UPRIGHT, UPLEFT, DOWNRIGHT, DOWNLEFT};
jState joyState = MIDDLE;
jState prevJoyState = MIDDLE;




bool joyUp;
bool joyDown;
bool joyLeft;
bool joyRight;
int button1State;
int joystickX;
int joystickY;
int attackNum;
int defendNum;
#pragma endregion PINS AND PARAMS



//////////////////////////////////////////////////
              //  S E T U P  //
//////////////////////////////////////////////////
#pragma region SETUP
void setup() {
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(INIT_MSG);  // print start up message

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);

  button1.setDebounceTime(50);  //debounce prevents the joystick button from triggering twice when clicked

  lcd.clear();
}
#pragma endregion SETUP

//////////////////////////////////////////////////
                //  L O O P  //
//////////////////////////////////////////////////
#pragma region LOOP
void loop() {

  button1.loop();
  button1State = button1.getState();

  joystickX = analogRead(joystickXPin);
  joystickY = analogRead(joystickYPin);
  joyUp = joystickY < (512 - joystickButtonThreshold);
  joyDown = joystickY > (512 + joystickButtonThreshold);
  joyLeft = joystickX < (512 - joystickButtonThreshold);
  joyRight = joystickX > (512 + joystickButtonThreshold);

  switch (currentState) {  //state machine that determines what to do with the input controls based on what mode the device is in

    case MainMenu:
      {
        attackNum = 0;
        defendNum = 0;

        if (prevState != MainMenu) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(MODE_NAME);
          lcd.setCursor(0, 1);
          lcd.print("      START     ");
          cursorPosition = 5;
          prevState = MainMenu;
        }

        lcd.setCursor(cursorPosition, 1);

        if (millis() % 600 < 400) {  // Blink every 500 ms
          lcd.print(">");
        } else {
          lcd.print(" ");
        }

        if (button1.isPressed()) {  //handles clicking options in text size setting
          lcd.clear();
          currentState = EditingAttack;
          prevState = MainMenu;
        }
      }
      break;

    case EditingAttack:  //in the editing mode, joystick directional input adds and removes characters from the string, while up and down changes characters
      //pressing the joystick button will switch the device into the Print Confirmation mode

      // Editing mode
      if (prevState != EditingAttack) {
        lcd.clear();
        prevState = EditingAttack;
      }
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(MENU);
      lcd.setCursor(0, 1);
      lcd.print(attackNum);
      lcd.setCursor(9,1);
      lcd.print(defendNum);
      lcd.setCursor(0, 1);

      // Check if the joystick is moved up (previous letter) or down (next letter)

      if (joyUp) {  //UP (previous character)
        Serial.println(attackNum);
        if (attackNum < 999) {
          attackNum++;
          // lcd.print(attackNum);
          //Serial.println("Character UP");
        }
        delay(190);  // Delay to prevent rapid scrolling

      } else if (joyDown) {  //DOWN (next character)
        Serial.println(attackNum);
        if (attackNum > 0) {
          attackNum--;  //increment character value
          // lcd.print(attackNum);
          //Serial.println("Character DOWN");
        }
        delay(190);  // Delay to prevent rapid scrolling
      } else {
        if (millis() % 600 < 450) {
          lcd.print(attackNum);
        } else {
          lcd.print(" ");
        }
      }

      if (joyRight) {  //RIGHT adds a space or character to the label
        currentState = EditingDefend;
        prevState = EditingAttack;
        delay(250);  // Delay to prevent rapid multiple presses
      }

      if (button1.isPressed()) {
        if (attackNum != 0 && defendNum != 0){
          lcd.clear();
          currentState = PrintConfirmation;
          prevState = EditingAttack;
        }
      }

      break;

    case EditingDefend:  //in the editing mode, joystick directional input adds and removes characters from the string, while up and down changes characters
      //pressing the joystick button will switch the device into the Print Confirmation mode

      // Editing mode
      if (prevState != EditingDefend) {
        lcd.clear();
        prevState = EditingDefend;
      }
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(MENU);
      lcd.setCursor(0, 1);
      lcd.print(attackNum);
      lcd.setCursor(9, 1);
      lcd.print(defendNum);
      lcd.setCursor(9, 1);

      // Check if the joystick is moved up (previous letter) or down (next letter)

      if (joyUp) {  //UP (previous character)
        Serial.println(defendNum);
        if (defendNum < 999) {
          defendNum++;
          // lcd.print(defendNum);
          //Serial.println("Character UP");
        }
        delay(190);  // Delay to prevent rapid scrolling

      } else if (joyDown) {  //DOWN (next character)
        Serial.println(defendNum);
        if (defendNum > 0) {
          defendNum--;  //increment character value
          // lcd.print(defendNum);
          //Serial.println("Character DOWN");
        }
        delay(190);  // Delay to prevent rapid scrolling
      } else {
        if (millis() % 600 < 450) {
          lcd.print(defendNum);
        } else {
          lcd.print(" ");
        }
      }

      if (joyLeft) {  //RIGHT adds a space or character to the label
        currentState = EditingAttack;
        prevState = EditingDefend;
        delay(250);  // Delay to prevent rapid multiple presses
      }

      if (button1.isPressed()) {
        if (attackNum != 0 && defendNum != 0){
          lcd.clear();
          currentState = PrintConfirmation;
          prevState = EditingDefend;
        }
      }

      break;

    case PrintConfirmation:
      // Print confirmation mode
      if (prevState == EditingAttack || prevState == EditingDefend) {
        lcd.setCursor(0, 0);    //move cursor to the first line
        lcd.print(PRINT_CONF);  //print menu text
        lcd.setCursor(0, 1);    // move cursor to the second line
        lcd.print("   YES     NO   ");
        lcd.setCursor(2, 1);
        cursorPosition = 2;
        prevState = PrintConfirmation;
      }

      //the following two if statements help move the blinking cursor from one option to the other.
      if (joyLeft) {  //left
        lcd.setCursor(0, 1);
        lcd.print("   YES     NO   ");
        lcd.setCursor(2, 1);
        cursorPosition = 2;
        delay(200);
      } else if (joyRight) {  //right
        lcd.setCursor(0, 1);
        lcd.print("   YES     NO   ");
        lcd.setCursor(10, 1);
        cursorPosition = 10;
        delay(200);
      }

      lcd.setCursor(cursorPosition, 1);

      if (millis() % 600 < 400) {  // Blink every 500 ms
        lcd.print(">");
      } else {
        lcd.print(" ");
      }

      if (button1.isPressed()) {    //handles clicking options in print confirmation
        if (cursorPosition == 2) {  //proceed to printing if clicking yes
          lcd.clear();
          currentState = Results;
          prevState = PrintConfirmation;

        } else if (cursorPosition == 10) {  //return to editing if you click no
          lcd.clear();
          currentState = EditingAttack;
          prevState = PrintConfirmation;
        }
      }

      break;

    case Results:
      // Printing mode
      if (prevState == PrintConfirmation) {
        lcd.setCursor(0, 0);

        while(attackNum > 0 && defendNum > 0){
          int a[3] = {0, 0, 0};
          int d[2] = {0, 0};
          int numAttackDie = min(3, attackNum);
          int numDefendDie = min(2, defendNum);

          for(int i = 0; i< numAttackDie; i++){
            a[i] = random(1,7);
            Serial.print("Attack Die ");
            Serial.print(i);
            Serial.print(" = ");
            Serial.println(a[i]);
          }
          for(int i = 0; i< numDefendDie; i++){
            d[i] = random(1,7);
            Serial.print("Defend Die ");
            Serial.print(i);
            Serial.print(" = ");
            Serial.println(d[i]);
          }
          sortDescending(a, numAttackDie);
          sortDescending(d, numDefendDie);

          int numBattles = min(numAttackDie, numDefendDie);

          for (int i = 0; i < numBattles; i++) {
              if (a[i] > d[i]) {
                  defendNum--;
                  Serial.println("Attack won");
              }
              else {
                  attackNum--;
                  Serial.println("Defend won");
              }
          }

          
          Serial.print("attack remaining: ");
          Serial.print(attackNum);
          Serial.print("  defend remaining: ");
          Serial.println(defendNum);
        }

        if(attackNum == 0) lcd.print(R_DEF);
        if(defendNum == 0) lcd.print(R_ATCK);
        lcd.setCursor(0,1);
        lcd.print(R_TROOP);
        lcd.print(max(attackNum, defendNum));

      }

      if(button1.isPressed()){
        lcd.clear();
        currentState = MainMenu;
        prevState = Results;
      }

      break;
  }
}
#pragma endregion LOOP

//////////////////////////////////////////////////
              // FUNCTIONS  //
//////////////////////////////////////////////////
#pragma region FUNCTIONS

void sortDescending(int array[], int length) {
  for(int i = 0; i < length - 1; i++){
    for(int j = 0; j < length; j++){
      if (array[j] > array [i]){
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
      }
    }
  }
}

void resetScreen() {
  lcd.clear();          // clear LCD
  lcd.setCursor(0, 0);  // set cursor to row 0 column 0
  lcd.print(": ");
  lcd.setCursor(1, 0);  //move cursor down to row 1 column 0
  cursorPosition = 1;
}
#pragma endregion FUNCTIONS

//////////////////////////////////////////////////
               //  END CODE  //
//////////////////////////////////////////////////
