//
// Created by Nick Anthony Miras on 2/26/26.
//
#include <Arduino.h>
#include <Keypad.h>
#include <Servo.h>

// --- Servo Configuration ---
Servo lockServo;
constexpr int SERVO_PIN = 10;       // Pin connected to the servo signal wire
constexpr int LOCKED_ANGLE = 0;     // Angle for the "locked" position
constexpr int UNLOCKED_ANGLE = 90;  // Angle for the "unlocked" position
constexpr int UNLOCK_DURATION = 10000; // How long the door stays unlocked (in milliseconds)

// --- Keypad Configuration ---
constexpr byte ROWS = 4;
constexpr byte COLS = 4;

// Define the keymap based on a standard 4x4 membrane keypad
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Connect keypad ROW1, ROW2, ROW3 and ROW4 to these Arduino pins.
byte rowPins[ROWS] = {9, 8, 7, 6};
// Connect keypad COL1, COL2, COL3 and COL4 to these Arduino pins.
byte colPins[COLS] = {5, 4, 3, 2};

// Initialize the Keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- Passcode Configuration ---
const String CORRECT_PASSCODE = "1234"; // Set your secret passcode here
String enteredPasscode = "";            // Buffer to hold user input

void setup() {
  Serial.begin(9600);

  // Attach and set initial locked position
  lockServo.attach(SERVO_PIN);
  lockServo.write(LOCKED_ANGLE);

  Serial.println("System Initialized.");
  Serial.println("Enter passcode and press '#' to submit.");
  Serial.println("Press '*' to clear your input.");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    // If the user presses '*', clear the current entry
    if (key == '*') {
      enteredPasscode = "";
      Serial.println("\nInput cleared.");
    }
    // If the user presses '#', submit the entry
    else if (key == '#') {
      Serial.println(); // New line for cleaner output

      if (enteredPasscode == CORRECT_PASSCODE) {
        Serial.println("Access Granted! Unlocking...");

        // Unlock sequence
        lockServo.write(UNLOCKED_ANGLE);
      } else {
        Serial.println("Access Denied! Incorrect passcode.");
      }

      // Reset the buffer for the next attempt
      enteredPasscode = "";
    }
    // For any other key, add it to the passcode buffer
    else {
      enteredPasscode += key;
      Serial.print("*"); // Print a star to hide the actual password on serial monitor
    }
  }
}