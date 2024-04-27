// Include necessary libraries
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
#include "Adafruit_TSL2591.h"

// Define pins and variables for the first sensor
const int sensorPin = 33;        // Pin 33 used as ADC input
const int maxValue = 2240;       // Maximum sensor value representing 100%

// Define pins and variables for the second sensor
const int sensorPin2 = 32;       // Pin for the second sensor
const int maxValue2 = 2240;      // Maximum sensor value for the second sensor

// Define pin connected to the relay controlling the pump
const int pumpRelayPin = 16;

// Pins for X-axis
const int stepPinX1 = 4;
const int stepPinX2 = 13;
const int dirPinX1 = 2;
const int dirPinX2 = 12;
const int enablePinX = 5;

// Pins for Y-axis
const int stepPinY = 19;
const int dirPinY = 18;
const int enablePinY = 15;

// Pins for Z-axis
const int stepPinZ = 26;
const int dirPinZ = 25;
const int enablePinZ = 27;

// Number of rotations for each axis for sensor 1 and sensor 2
int numRotationsSensor1_X = 10;
int numRotationsSensor1_Y = 10;
int numRotationsSensor1_Z = 10;

int numRotationsSensor2_X = 15;
int numRotationsSensor2_Y = 15;
int numRotationsSensor2_Z = 15;

int state = 0; // State variable for state machine

Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // Creating an instance of the TSL2591 sensor

// Define relay pins for controlling lights and fans
const int relayPinLights = 17; // Pin connected to the relay controlling the lights
const int relayPinFans = 14;   // Pin connected to the relays controlling the fans

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Moisture Sensor and Water Pump Control Example");

  // Set pump relay pin as output
  pinMode(pumpRelayPin, OUTPUT);

  // Initialize pin modes for X-axis
  pinMode(stepPinX1, OUTPUT);
  pinMode(stepPinX2, OUTPUT);
  pinMode(dirPinX1, OUTPUT);
  pinMode(dirPinX2, OUTPUT);
  pinMode(enablePinX, OUTPUT);

  // Initialize pin modes for Y-axis
  pinMode(stepPinY, OUTPUT);
  pinMode(dirPinY, OUTPUT);
  pinMode(enablePinY, OUTPUT);

  // Initialize pin modes for Z-axis
  pinMode(stepPinZ, OUTPUT);
  pinMode(dirPinZ, OUTPUT);
  pinMode(enablePinZ, OUTPUT);

  // Initialize relay pins for lights and fans
  pinMode(relayPinLights, OUTPUT);
  pinMode(relayPinFans, OUTPUT);

  // Initialize I2C communication
  Wire.begin();

  // Initialize SHT31 sensor
  if (!sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  // Initialize TSL2591 sensor
  if (!tsl.begin()) {
    Serial.println("Couldn't find TSL2591 sensor");
    while (1);
  }

  tsl.setGain(TSL2591_GAIN_LOW); // Set sensor gain to low
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS); // Set integration time to 100ms
}

void loop() {
  // Read moisture level from the first sensor
  int sensorValue1 = analogRead(sensorPin);
  float percentage1 = (sensorValue1 / (float)maxValue) * 100.0;

  // Read moisture level from the second sensor
  int sensorValue2 = analogRead(sensorPin2);
  float percentage2 = (sensorValue2 / (float)maxValue2) * 100.0;

  // Print moisture levels for both sensors
  Serial.print("Moisture Percentage (Sensor 1): ");
  Serial.print(percentage1, 2);
  Serial.println("%");

  Serial.print("Moisture Percentage (Sensor 2): ");
  Serial.print(percentage2, 2);
  Serial.println("%");

  // State machine to control actions based on moisture levels
  switch (state) {
    case 0: // BELOW_THRESHOLD
      if (percentage1 < 60.0 || percentage2 < 60.0) {
        Serial.println("Moisture below 60%: Activating pump and moving axes forward");

        // Move X-axis forward
        digitalWrite(dirPinX1, LOW);
        digitalWrite(dirPinX2, LOW);
        for (int i = 0; i < numRotationsSensor1_X; i++) {
          simpleMoveX(stepPinX1, stepPinX2, 200); // 200 steps per motor for 1 full rotation
        }
        delay(2000); // Wait 2 seconds

        // Move Y-axis forward
        digitalWrite(dirPinY, LOW);
        digitalWrite(enablePinX, HIGH); // Disable X-axis motors
        digitalWrite(enablePinY, LOW); // Enable Y-axis motor
        for (int i = 0; i < numRotationsSensor1_Y; i++) {
          simpleMoveY(stepPinY, 200); // 200 steps for 1 full rotation
        }
        delay(2000); // Wait 2 seconds

        // Move Z-axis forward
        digitalWrite(dirPinZ, LOW);
        digitalWrite(enablePinY, HIGH); // Disable Y-axis motor
        digitalWrite(enablePinZ, LOW); // Enable Z-axis motor
        for (int i = 0; i < numRotationsSensor1_Z; i++) {
          simpleMoveZ(stepPinZ, 200); // 200 steps for 1 full rotation
        }

        // Move axes backward
        digitalWrite(dirPinZ, HIGH);
        digitalWrite(enablePinZ, LOW); // Enable Z-axis motor
        for (int i = 0; i < numRotationsSensor1_Z; i++) {
          simpleMoveZ(stepPinZ, 200); // 200 steps for 1 full rotation
        }

        digitalWrite(dirPinY, HIGH);
        digitalWrite(enablePinY, LOW); // Enable Y-axis motor
        for (int i = 0; i < numRotationsSensor1_Y; i++) {
          simpleMoveY(stepPinY, 200); // 200 steps for 1 full rotation
        }

        digitalWrite(dirPinX1, HIGH);
        digitalWrite(dirPinX2, HIGH);
        digitalWrite(enablePinY, HIGH); // Disable Y-axis motor
        digitalWrite(enablePinX, LOW); // Enable X-axis motors
        for (int i = 0; i < numRotationsSensor1_X; i++) {
          simpleMoveX(stepPinX1, stepPinX2, 200); // 200 steps per motor for 1 full rotation
        }

        state = 1; // Move to next state
      }
      break;

    case 1: // SENSOR_1_BACKWARD_DONE
      Serial.println("Sensor 1: Axes moved forward and backward, waiting before proceeding to Sensor 2");
      delay(10000); // Wait 10 seconds before moving to Sensor 2
      state = 2; // Move to next state
      break;

    case 2: // SENSOR_2_FORWARD
      if (percentage2 < 60.0) {
        Serial.println("Sensor 2: Moisture below 60%: Activating pump and moving axes forward");

        // Move X-axis forward
        digitalWrite(dirPinX1, LOW);
        digitalWrite(dirPinX2, LOW);
        for (int i = 0; i < numRotationsSensor2_X; i++) {
          simpleMoveX(stepPinX1, stepPinX2, 200); // 200 steps per motor for 1 full rotation
        }
        delay(2000); // Wait 2 seconds

        // Move Y-axis forward
        digitalWrite(dirPinY, LOW);
        digitalWrite(enablePinX, HIGH); // Disable X-axis motors
        digitalWrite(enablePinY, LOW); // Enable Y-axis motor
        for (int i = 0; i < numRotationsSensor2_Y; i++) {
          simpleMoveY(stepPinY, 200); // 200 steps for 1 full rotation
        }
        delay(2000); // Wait 2 seconds

        // Move Z-axis forward
        digitalWrite(dirPinZ, LOW);
        digitalWrite(enablePinY, HIGH); // Disable Y-axis motor
        digitalWrite(enablePinZ, LOW); // Enable Z-axis motor
        for (int i = 0; i < numRotationsSensor2_Z; i++) {
          simpleMoveZ(stepPinZ, 200); // 200 steps for 1 full rotation
        }

        // Move axes backward
        digitalWrite(dirPinZ, HIGH);
        digitalWrite(enablePinZ, LOW); // Enable Z-axis motor
        for (int i = 0; i < numRotationsSensor2_Z; i++) {
          simpleMoveZ(stepPinZ, 200); // 200 steps for 1 full rotation
        }

        digitalWrite(dirPinY, HIGH);
        digitalWrite(enablePinY, LOW); // Enable Y-axis motor
        for (int i = 0; i < numRotationsSensor2_Y; i++) {
          simpleMoveY(stepPinY, 200); // 200 steps for 1 full rotation
        }

        digitalWrite(dirPinX1, HIGH);
        digitalWrite(dirPinX2, HIGH);
        digitalWrite(enablePinY, HIGH); // Disable Y-axis motor
        digitalWrite(enablePinX, LOW); // Enable X-axis motors
        for (int i = 0; i < numRotationsSensor2_X; i++) {
          simpleMoveX(stepPinX1, stepPinX2, 200); // 200 steps per motor for 1 full rotation
        }

        state = 3; // Move to next state
      }
      break;

    case 3: // SENSOR_2_BACKWARD_DONE
      Serial.println("Sensor 2: Axes moved forward and backward, waiting before proceeding");
      delay(10000); // Wait 10 seconds before proceeding
      state = 0; // Reset state to check moisture level again
      break;
  }

  temperatureFanControl();
  lightControl();
}
if (Serial.available() > 0) {
    char command = Serial.read();
    float distance = 5;
    switch (command) {
      case 'X':
        bool forward = TRUE;
        moveX(stepPin1, stepPin2, distance, forward)// Code to move X motor forward
        break;
      case 'x':
        bool forward = FALSE;
        moveX(stepPin1, stepPin2, distance, forward)// Code to move X motor backward
        break;
      case 'Y':
        bool forward = TRUE;
        moveY(int stepPin, int steps)// Code to move Y motor forward
        break;
      case 'y':
        bool forward = FALSE;
        moveY(int stepPin, int steps)// Code to move Y motor backward
        break;
      case 'Z':
        bool forward = TRUE;
        moveZ(int stepPin, int steps)// Code to move Z motor forward
        break;
      case 'z':
        bool forward = FALSE;
        moveZ(int stepPin, int steps)// Code to move Z motor backward
        break;
      case's1':
        int speed = 10000;
        break
      case's2':
        int speed = 1000;
        break
      case's3':
        int speed = 100;
        break
      default:
        break;
    }
  }
  }

  temperatureFanControl();
  lightControl();
}

// Function to move both stepper motors of the X-axis for a given distance and direction
void moveX(int stepPin1, int stepPin2, float distance, bool forward, int speed) {
  // Calculate the number of steps required based on the distance
  float stepsPerMM = 2000; // 200 steps per revolution, and 10 revolutions per mm (0.1mm per revolution)
  int steps = distance * stepsPerMM;

  // Set the direction of movement based on the 'forward' parameter
  digitalWrite(dirPinX1, forward ? LOW : HIGH); // Assuming dirPinX1 is one direction pin for X-axis
  digitalWrite(dirPinX2, forward ? LOW : HIGH); // Assuming dirPinX2 is the other direction pin for X-axis

  // Loop through the calculated number of steps
  for (int i = 0; i < steps; i++) {
    // Trigger an impulse to move the motors
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(speed); // Adjust delay as needed for motor speed
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(speed); // Adjust delay as needed for motor speed
  }
}

// Function to move the stepper motor of the Y-axis for a given distance and direction
void moveY(int stepPin, float distance, bool forward, int speed) {
  // Calculate the number of steps required based on the distance
  float stepsPerMM = 2000; // 200 steps per revolution, and 10 revolutions per mm (0.1mm per revolution)
  int steps = distance * stepsPerMM;

  // Set the direction of movement based on the 'forward' parameter
  digitalWrite(dirPinY, forward ? LOW : HIGH); // Assuming dirPinY is the direction pin for Y-axis

  // Loop through the calculated number of steps
  for (int i = 0; i < steps; i++) {
    // Trigger an impulse to move the motor
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(speed); // Adjust delay as needed for motor speed
    digitalWrite(stepPin, LOW);
    delayMicroseconds(speed); // Adjust delay as needed for motor speed
  }
}

// Function to move the stepper motor of the Z-axis for a given distance and direction
void moveZ(int stepPin, float distance, bool forward, int speed) {
  // Calculate the number of steps required based on the distance
  float stepsPerMM = 2000; // 200 steps per revolution, and 10 revolutions per mm (0.1mm per revolution)
  int steps = distance * stepsPerMM;

  // Set the direction of movement based on the 'forward' parameter
  digitalWrite(dirPinZ, forward ? LOW : HIGH); // Assuming dirPinZ is the direction pin for Z-axis

  // Loop through the calculated number of steps
  for (int i = 0; i < steps; i++) {
    // Trigger an impulse to move the motor
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(speed); // Adjust delay as needed for motor speed
    digitalWrite(stepPin, LOW);
    delayMicroseconds(speed); // Adjust delay as needed for motor speed
  }
}

// Function to move both stepper motors of the X-axis simultaneously
void simpleMoveX(int stepPin1, int stepPin2, int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(1000);
  }
}

// Function to move the stepper motor of the Y-axis
void simpleMoveY(int stepPin, int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
}

// Function to move the stepper motor of the Z-axis
void simpleMoveZ(int stepPin, int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
}

void temperatureFanControl() {
  float temperature = sht31.readTemperature();

  if (!isnan(temperature)) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    if (temperature > 26.7) {
      // Turn on all the fans
      digitalWrite(relayPinFans, HIGH);
      Serial.println("Fans turned on");
    } else {
      // Turn off all the fans
      digitalWrite(relayPinFans, LOW);
      Serial.println("Fans turned off");
    }
  } else {
    Serial.println("Failed to read from SHT31 sensor!");
  }
}

void lightControl() {
  unsigned long currentMillis = millis();
  static bool lightsOn = false; // Variable to track the current state of the lights
  static unsigned long lightsStartTime = 0; // Time when the lights were turned on

  // If lux is above 700, turn off the lights
  if (tsl.getLuminosity(TSL2591_VISIBLE) > 700) {
    digitalWrite(relayPinLights, LOW); // Turn off the lights
    lightsOn = false; // Update lights state
    lightsStartTime = 0; // Reset lights start time
  }
  // Otherwise, check light on/off cycle
  else {
    // If lights are on and the on time has elapsed, turn them off
    if (lightsOn && (currentMillis - lightsStartTime >= 30000)) {
      digitalWrite(relayPinLights, LOW); // Turn off the lights
      lightsOn = false; // Update lights state
    }
    // If lights are off and the off time has elapsed, turn them on
    else if (!lightsOn && (currentMillis - lightsStartTime >= 35000)) {
      digitalWrite(relayPinLights, HIGH); // Turn on the lights
      lightsOn = true; // Update lights state
      lightsStartTime = currentMillis; // Update lights start time
    }
  }
}
