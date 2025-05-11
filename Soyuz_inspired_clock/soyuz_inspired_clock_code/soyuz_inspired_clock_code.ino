#include <Wire.h>
#include <Rtc_Pcf8563.h>
#include <TM1637TinyDisplay6.h>
#include <TM1637TinyDisplay.h>

// Main Display pins
#define CLK 11
#define DIO 10

// Stopwatch Display pins
#define STP_CLK 9
#define STP_DIO 8

// Create RTC and Display objects
Rtc_Pcf8563 rtc;
TM1637TinyDisplay6 main_display(CLK, DIO);
TM1637TinyDisplay stp_display(STP_CLK, STP_DIO);

// Input pins
const int mode_pin = 6;
const int setting_time_pin = 7;
const int time_input_pin = 5;
// The latching button for the stopwatch: it is HIGH by default and goes LOW when pressed.
const int stopwatch_pin = 4;

// Stopwatch variables
unsigned long stopwatch_elapsed = 0;      // Elapsed time in seconds
unsigned long stopwatch_start_time = 0;   // The RTC time (in seconds) when the stopwatch started
bool stopwatch_running = false;           // Whether the stopwatch is actively counting
int prev_button_state = HIGH;             // Default state is HIGH

// Helper function to get total seconds from RTC (since midnight)
unsigned long getRtcTotalSeconds() {
  int hr = rtc.getHour();
  int min = rtc.getMinute();
  int sec = rtc.getSecond();
  return (unsigned long)hr * 3600UL + (unsigned long)min * 60UL + sec;
}

void setup() {
  Serial.begin(9600);

  // Setup pins. The non-stopwatch pins use internal pull-ups.
  pinMode(mode_pin, INPUT_PULLUP);
  pinMode(setting_time_pin, INPUT_PULLUP);
  pinMode(time_input_pin, INPUT_PULLUP);
  // Use INPUT_PULLUP for the stopwatch pin (button is normally HIGH and goes LOW when pressed).
  pinMode(stopwatch_pin, INPUT_PULLUP);

  // Initialize the displays.
  main_display.setBrightness(BRIGHT_HIGH);
  main_display.clear();
  stp_display.setBrightness(BRIGHT_HIGH);
  stp_display.clear();
}

void loop() {
  // -------- RTC Clock Update (Main Display) --------
  int hr  = rtc.getHour();
  int min = rtc.getMinute();
  int sec = rtc.getSecond();

  // Debug output for the RTC time.
  Serial.print("Time: ");
  Serial.print(hr);
  Serial.print(":");
  Serial.print(min);
  Serial.print(":");
  Serial.println(sec);

  // Build a six-digit number in hhmmss format.
  long timeInt = (long)hr * 10000L + min * 100L + sec;
  char timeStr[7];  // 6 digits + null terminator.
  sprintf(timeStr, "%06ld", timeInt);
  main_display.showString(timeStr);

  // -------- Stopwatch Update (Using RTC as Time Source) --------
  int buttonState = digitalRead(stopwatch_pin);
  
  // When the pin is HIGH, clear the stopwatch and start counting.
  if (buttonState == HIGH) {
    // On a transition from LOW to HIGH, reset the stopwatch.
    if (prev_button_state == LOW) {
      stopwatch_start_time = getRtcTotalSeconds();
      stopwatch_elapsed = 0;
    }
    stopwatch_running = true;
    
    // Use the RTC to calculate elapsed seconds.
    unsigned long current_total = getRtcTotalSeconds();
    // Handle midnight wrap-around.
    if (current_total < stopwatch_start_time)
      current_total += 86400UL;
    stopwatch_elapsed = current_total - stopwatch_start_time;
  } 
  else {
    // When the pin is LOW, pause the stopwatch.
    stopwatch_running = false;
    // stopwatch_elapsed remains unchanged (frozen).
  }
  
  // Save the current button state for edge detection.
  prev_button_state = buttonState;

  // Format the stopwatch time as mm:ss.
  unsigned int display_minutes = stopwatch_elapsed / 60;
  unsigned int display_seconds = stopwatch_elapsed % 60;
  char stopwatchStr[5];  // 4 digits + null terminator.
  sprintf(stopwatchStr, "%02u%02u", display_minutes, display_seconds);
  

  // Short delay to ensure both displays update simultaneously.
  main_display.showString(timeStr);
  stp_display.showString(stopwatchStr);
  delay(100);
}
