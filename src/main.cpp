//
// Created by Nick Anthony Miras on 2/26/26.
//
#include <Arduino.h>
#include <Keypad.h>
#include <Servo.h>
#include "audio.h"


// --- Servo Configuration ---
Servo lockServo;
constexpr int SERVO_PIN = 3;       // Pin connected to the servo signal wire
constexpr int LOCKED_ANGLE = 0;     // Angle for the "locked" position
constexpr int UNLOCKED_ANGLE = 90;  // Angle for the "unlocked" position
constexpr int UNLOCK_DURATION = 10000; // How long the door stays unlocked (in milliseconds)

// --- Keypad Configuration ---
constexpr byte ROWS = 4;
constexpr byte COLS = 4;

// --- Ultrasonic Sensor Configuration ---
constexpr int TRIG_PIN = 12; // Adjust if using different pins
constexpr int ECHO_PIN = 13; // Adjust if using different pins
constexpr float DISTANCE_THRESHOLD_CM = 5.0; // Distance threshold to consider door closed
unsigned long lastUltrasonicCheck = 0;
constexpr unsigned long ULTRASONIC_INTERVAL = 200; // Check distance every 200ms

// Define the keymap based on a standard 4x4 membrane keypad
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Connect keypad ROW1, ROW2, ROW3 and ROW4 to these Arduino pins.
byte rowPins[ROWS] = {7, 6, 5, 4};
// Connect keypad COL1, COL2, COL3 and COL4 to these Arduino pins.
byte colPins[COLS] = {11, 10, 9, 8};

// Initialize the Keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- Passcode Configuration ---
const String CORRECT_PASSCODE = "1234"; // Set your secret passcode here
String enteredPasscode = "";            // Buffer to hold user input

void setup() {
  Serial.begin(115200);
  // Wait for Serial to initialize (optional, remove if running untethered)
  while (!Serial) delay(10);

  // Initialize Ultrasonic Sensor Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW); // Start LOW

  // Attach and set initial locked position
  lockServo.attach(SERVO_PIN);
  lockServo.write(LOCKED_ANGLE);

  Serial.println("System Initialized.");
  Serial.println("Enter passcode and press '#' to submit.");
  Serial.println("Press '*' to clear your input.");
  Serial.println();

  // setupMicrophone(i2sIn);
  // Serial.println("Initializing INMP441 on Pico 2...");
  // // Start I2S at 16kHz (The standard frequency for Speech Recognition)
  // if (!i2sIn.begin(16000)) {
  //   Serial.println("Failed to initialize I2S! Check your wiring.");
  //   while (1); // Halt
  // }
  // Serial.println("I2S Initialized successfully.");
}

void loop() {
  // Check the ultrasonic sensor periodically without blocking
  if (millis() - lastUltrasonicCheck >= ULTRASONIC_INTERVAL) {
    lastUltrasonicCheck = millis();

    // Trigger specific sequence for HC-SR04
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Read the pulse duration (timeout = 30ms ~ 5m max distance)
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    if (duration > 0) {
      // Calculate distance in cm
      float distanceCm = duration * 0.034 / 2;
      
      // If the door is close enough, automatically lock the servo
      if (distanceCm < DISTANCE_THRESHOLD_CM) {
        // Only output if the state has changed to prevent log spamming, or just actuate
        lockServo.write(LOCKED_ANGLE);
      }
    }
  }

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