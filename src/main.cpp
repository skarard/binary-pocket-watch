#include <TimeLib.h>
#include <Arduino.h>

// Define LED pins for hours and minutes
const int hourPins[] = {13, 15, 18, 20};     // 4 bits for hours
const int minutePins[] = {7, 6, 5, 4, 3, 2}; // 6 bits for minutes
const int numHourPins = sizeof(hourPins) / sizeof(hourPins[0]);
const int numMinutePins = sizeof(minutePins) / sizeof(minutePins[0]);

// Function prototypes
void setTimeFromSerial(const String &timeString);
void clearAllLEDs();
void displayBinary(int value, const int pins[], int numBits);
void loadingAnimation();

bool isTimeSet = false;

void setup()
{
  // Initialize LED pins
  for (int i = 0; i < numHourPins; i++)
  {
    pinMode(hourPins[i], OUTPUT);
  }
  for (int i = 0; i < numMinutePins; i++)
  {
    pinMode(minutePins[i], OUTPUT);
  }
  // Initialize Serial for time synchronization
  Serial.begin(9600);
  Serial.println("Send time in HHMM format (e.g., 1435 for 14:35)");
}

void loop()
{
  // Check if there's data available over Serial to update the time
  if (Serial.available())
  {
    String receivedData = Serial.readStringUntil('\n'); // Read until newline
    receivedData.trim();                                // Remove any leading/trailing whitespace

    if (receivedData.length() == 4 && receivedData.toInt() >= 0)
    {
      setTimeFromSerial(receivedData); // Parse and update clock
      isTimeSet = true;
    }
    else
    {
      Serial.println("Invalid input. Please use HHMM for time.");
    }
  }

  if (isTimeSet)
  {
    // Always display the current time in binary
    displayBinary(hour() % 12 == 0 ? 12 : hour() % 12, hourPins, numHourPins); // 12-hour format
    displayBinary(minute(), minutePins, numMinutePins);                        // Minute display
  }
  else
  {
    loadingAnimation();
  }
  delay(100); // Small delay for stability
}

// Function to parse and update time from a received HHMM format
void setTimeFromSerial(const String &timeString)
{
  if (timeString.length() != 4)
  { // Check if length is correct (4 characters for HHMM)
    Serial.println("Invalid format. Please use HHMM (e.g., 1435 for 14:35).");
    return;
  }

  int hour = timeString.substring(0, 2).toInt();
  int minute = timeString.substring(2, 4).toInt();

  if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60)
  {
    setTime(hour, minute, 0, day(), month(), year()); // Set time with current day, month, year
    Serial.print("Time synchronized to: ");
    Serial.print(hour < 10 ? "0" : "");
    Serial.print(hour); // Leading zero for single-digit hours
    Serial.print(":");
    Serial.print(minute < 10 ? "0" : "");
    Serial.println(minute); // Leading zero for single-digit minutes
  }
  else
  {
    Serial.println("Invalid time. Please use HHMM with 00 <= HH < 24 and 00 <= MM < 60.");
  }
}

// Function to display a binary value on a set of LEDs
void displayBinary(int value, const int pins[], int numBits)
{
  for (int i = 0; i < numBits; i++)
  {
    digitalWrite(pins[i], (value & (1 << i)) ? HIGH : LOW); // Set LED based on bit value
  }
}

// Function to turn off all LEDs
void clearAllLEDs()
{
  for (int i = 0; i < numHourPins; i++)
    digitalWrite(hourPins[i], LOW);
  for (int i = 0; i < numMinutePins; i++)
    digitalWrite(minutePins[i], LOW);
}

void loadingAnimation()
{
  struct LoadingPinDelay
  {
    int pin;
    int delayTime;
  };

  LoadingPinDelay loadingPins[] = {
      {15, 150},
      {13, 200},
      {7, 50},
      {6, 50},
      {5, 50},
      {4, 50},
      {3, 50},
      {2, 200},
      {20, 150},
      {18, 50}};

  const int numLoadingPins = sizeof(loadingPins) / sizeof(loadingPins[0]);
  for (int i = 0; i < numLoadingPins; i++)
  {
    clearAllLEDs();
    digitalWrite(loadingPins[i].pin, HIGH);
    delay(100); // Time LED stays on
    digitalWrite(loadingPins[i].pin, LOW);
    delay(loadingPins[i].delayTime); // Delay before the next LED
  }
};