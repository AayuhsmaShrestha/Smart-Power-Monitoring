#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C myLcd(0x27, 16, 2); // Set I2C address to 0x27 for the LCD

// Define pins for other components
const int pirPin = 7;         // PIR motion sensor
const int buzzerPin = 11;     // Piezo buzzer
const int ldrPin = A0;        // LDR
const int potPin = A3;        // Potentiometer pin
const int led1Pin = 13;       // LED1 for motion detection
const int led2Pin = 2;        // LED2 for day/night detection
const int led3Pin = 4;        // LED3 for temperature indication
const int tempPin = A2;

unsigned long motionTimestamp = 0;  // Timestamp for motion logic
bool motionActive = false;

void setup() {
  // Initialize the LCD
  myLcd.init();
  myLcd.backlight();
  myLcd.print("Initializing...");

  // Set pin modes
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(potPin, INPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(tempPin, INPUT);
  
  delay(2000);
  myLcd.clear();
}

void loop() {
  // Read temperature from TMP36
  int tempValue = analogRead(tempPin);
  float tempVoltage = tempValue * (5.0 / 1023.0);    // Convert analog value to voltage
  float temperatureC = (tempVoltage - 0.5) * 100.0; // Convert voltage to Celsius
  
  // Temperature indication
  if (temperatureC < 20) {  
    digitalWrite(led3Pin, LOW); // Turn off blue LED
  } else {  
    digitalWrite(led3Pin, HIGH); // Turn on blue LED
  }

  // PIR motion detection
  int motionDetected = digitalRead(pirPin);

  if (motionDetected && !motionActive) {
    motionActive = true;
    motionTimestamp = millis();
    digitalWrite(led1Pin, HIGH); // Turn on LED1
  }

  if (motionActive && millis() - motionTimestamp >= 5000) {
    motionActive = false;
    digitalWrite(led1Pin, LOW); // Turn off LED1
  }

  // LDR light intensity detection
  int lightValue = analogRead(ldrPin);

  // Potentiometer voltage detection
  int potValue = analogRead(potPin);
  float potVoltage = (potValue / 1023.0) * 5.0; // Map to 0-5V range

  // LDR day/night logic
  if (lightValue > 500) {        // Adjust threshold for day/night
    digitalWrite(led2Pin, LOW);  // Turn off LED2 (daytime)
  } else {
    digitalWrite(led2Pin, HIGH); // Turn on LED2 (nighttime)
  }

  // Buzzer logic based on potentiometer voltage
  if (potVoltage > 4.0) {        // Adjust threshold for "high voltage"
    digitalWrite(buzzerPin, HIGH); 
  } else {
    digitalWrite(buzzerPin, LOW);  
  }

  // Display values on the LCD
  myLcd.setCursor(0, 0);
  myLcd.print("Light: ");
  myLcd.print(lightValue); // Display light value
  myLcd.print("    ");     // Clear any extra characters

  myLcd.setCursor(0, 1);
  myLcd.print("Voltage: ");
  myLcd.print(potVoltage, 2); // Display voltage with 2 decimal places
  myLcd.print("V   ");       // Clear any extra characters

  delay(500); // Small delay for stability
}
