
#include <Servo.h>

const int trigPin = 9;
const int echoPin = 10;

const int ENA = 3;
const int IN1 = 7;
const int IN2 = 8;
const int servoPin = 5;

Servo sortingServo;

// Simulation settings
const float conveyorSpeed = 5.0;       // Assumed cm/s
const float detectionDistance = 15.0;  // cm
const float lengthLimit = 10.0;        // cm

const unsigned long maxDetectionTime = 10000;

bool objectDetected = false;
unsigned long startTime = 0;

int shortCount = 0;
int longCount = 0;

float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    return 999;
  }

  return duration * 0.0343 / 2.0;
}

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  sortingServo.attach(servoPin);
  sortingServo.write(90);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);

  Serial.println("Length Sorting System Ready!");
}

void loop() {
  float distance = measureDistance();
  bool present = (distance < detectionDistance);

  if (present && !objectDetected) {
    objectDetected = true;
    startTime = millis();

    Serial.println("Object detected!");
  }

  if (objectDetected) {
    unsigned long elapsed = millis() - startTime;

    if (!present) {
      objectDetected = false;

      float timeSeconds = elapsed / 1000.0;
      float objectLength = conveyorSpeed * timeSeconds;

      Serial.print("Estimated length: ");
      Serial.print(objectLength);
      Serial.println(" cm");

      if (objectLength < lengthLimit) {
        shortCount++;
        sortingServo.write(30);
        Serial.println("BIN A - SHORT");
      } else {
        longCount++;
        sortingServo.write(150);
        Serial.println("BIN B - LONG");
      }

      Serial.print("Short objects: ");
      Serial.println(shortCount);

      Serial.print("Long objects: ");
      Serial.println(longCount);

      delay(1000);
      sortingServo.write(90);
    }

    else if (elapsed > maxDetectionTime) {
      objectDetected = false;

      Serial.println("Warning: Detection timeout!");
      Serial.println("Object not counted.");

      sortingServo.write(90);
    }
  }

  delay(50);
}
