/***************************************************************
 * Purpose:
 * Count down the days, hours, minutes, and seconds to target event
 *
 * 
 * Hardware:
 * - Adafruit RGB Character LCD Shield -  16x2 Character LCD
 * https://learn.adafruit.com/rgb-lcd-shield/
 * - Arduino UNO
 * - DS3231 RTC - https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout
 * - CR1220 coin battery

****************************************************************/

#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <RTClib.h>

// Initialize hardware
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
RTC_DS3231 rtc;

// ----------- TARGET DATES ----------- //
struct Target {
  const char* name;
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};

// Add target events here
// name, year, month, day, hour, min, sec
Target targets[] = {  
  {
    "New Year",
    2027, 1, 1,
    0, 0, 0
  },
  {
    "Christmas",
    2026, 12, 25,
    0, 0, 0
  },
  {
    "Halloween",
    2026, 10, 31,
    0, 0, 0
  },
};
const int NUM_TARGETS =
  sizeof(targets) / sizeof(targets[0]);
int targetIndex = 0; // used to cycle through target datees

Target currentTarget = targets[0]; // assign initial target date

// short names for days and months
const char* daysOfTheWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};   // Sunday = 0
const char* months[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};

// fun or frak messages
const char* fnMessages[] = { 
  "party",
  "cake",
  "holiday",
  "celebration",
  "treats",
  "fun",
  "gifts",
  "cocktails",
  "holiday",
  "cheers",
  "scary clowns"
};
int messageIndex = 0; // used to cycle through fnMessages
int numMessages = sizeof(fnMessages) / sizeof(fnMessages[0]); // calculates the number of msgs to cycle through

// Backlight color definitions
#define OFF 0x0
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
int colorIndex = 0; // Used for cycling through backlight colors
const uint8_t backlightColors[] = {
  WHITE, VIOLET, YELLOW, GREEN, TEAL, BLUE, RED
};
int numColors = sizeof(backlightColors) / sizeof(backlightColors[0]); // calculates the number of colors to cycle through

// Button handling
#define BUTTON_NONE 0 // No button pressed
uint8_t lastButton = BUTTON_NONE;
const int displayDelay = 2500; // the delay time after each button display

// ----------- SETUP ----------- //
void setup() {
  // Serial monitor
  Serial.begin(115200);
  Serial.println("\nStart Serial.");

  // Initialize LCD: 16x2
  lcd.begin(16, 2);
  Serial.println("Initialize LCD.");

  // Initialize RTC
  if (!rtc.begin()) {
  lcd.setBacklight(RED);
  lcd.setCursor(0, 0);
  lcd.print("RTC ERROR");
  
  Serial.println("Couldn't find RTC");

  while (1);
  }

  // RTC lost power
  if (rtc.lostPower()) {
    Serial.println("RTC lost power!");
    // Set RTC to compile time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Startup Banner
  Serial.println("Countdown Timer!");
  showCurrentTime();
  lcd.clear();
  lcd.print("Countdown Timer");
  delay(displayDelay);
  lcd.setCursor(2, 1);
  lcd.print(currentTarget.name);
  delay(displayDelay);
  lcd.clear();

}

// --------- Cursor Placement --------- //
void lcdPrintCentered(const char* text, const int row = 0) {
  int length = strlen(text);
  int startPosition =
    (16 - length) / 2;

  if (startPosition < 0) {
    startPosition = 0;
  }
  lcd.setCursor(startPosition, row);
  lcd.print(text);
}

// ----------- TIME FUNCTION ----------- //
void showCurrentTime() {
  // Get the current date and time from the RTC
  DateTime now = rtc.now();

  // Get 24-hour time data
  int hour_24 = now.hour();
  int minute  = now.minute();
  int seconds  = now.second();
  int month = now.month();
  int dayOfWeek = now.dayOfTheWeek();
  int day = now.day();
  int year = now.year();

  // Determine AM or PM
  String period = (hour_24 >= 12) ? "pm" : "am";

  // Convert 24-hour to 12-hour format
  int hour_12 = hour_24 % 12;
  if (hour_12 == 0) hour_12 = 12;

  // Print Day of week and Date (e.g., "Mon Aug 10, 2026")
  // Serial output
  String dow = daysOfTheWeek[dayOfWeek];
  Serial.print(dow);
  Serial.print(" ");
  Serial.print(months[month]); Serial.print(" "); Serial.print(day); Serial.print(", "); Serial.print(year);
  Serial.print(" ");
  // Print 12-Hour Time with seconds (e.g., 9:17:00 AM)
  Serial.print(hour_12); Serial.print(':');
  if (minute < 10) Serial.print('0'); // leading zero
  Serial.print(minute); Serial.print(":"); 
  if (seconds < 10) Serial.print('0');
  Serial.print(seconds); Serial.print(" "); Serial.println(period);

  // LCD output
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(dow); lcd.print(" ");
  lcd.print(months[month]); lcd.print(" "); lcd.print(day); lcd.print(", "); lcd.print(year);
  lcd.setCursor(0,1);

  lcd.setCursor(4,1);
  lcd.print(hour_12); lcd.print(":");       
  if(minute < 10) lcd.print("0");
  lcd.print(minute); lcd.print(period);
  delay(displayDelay);
  lcd.clear();
}

// ----------- CYCLE TARGETS ----------- //
void cycleTargetDates() {
  DateTime now = rtc.now();

  targetIndex = (targetIndex + 1) % NUM_TARGETS;
  currentTarget = targets[targetIndex];

  Serial.print("Target index: ");
  Serial.println(targetIndex);

  Serial.print("Target name: ");
  Serial.println(currentTarget.name);

  Serial.print("Target date: ");
  Serial.print(currentTarget.month);
  Serial.print("/");
  Serial.print(currentTarget.day);
  Serial.print("/");
  Serial.println(currentTarget.year);

  showTargetEvent();
}

void orderedTargetDates() {
  DateTime now = rtc.now();

  DateTime currentDate(
    currentTarget.year,
    currentTarget.month,
    currentTarget.day,
    currentTarget.hour,
    currentTarget.minute,
    currentTarget.second
  );

  uint32_t nowTime = now.unixtime();
  uint32_t currentTime = currentDate.unixtime();

  int nextIndex = -1;
  uint32_t nextTime = UINT32_MAX;

  // First: find the earliest target after current target
  for (int i = 0; i < NUM_TARGETS; i++) {

    DateTime candidate(
      targets[i].year,
      targets[i].month,
      targets[i].day,
      targets[i].hour,
      targets[i].minute,
      targets[i].second
    );

    uint32_t candidateTime = candidate.unixtime();

    if (candidateTime > currentTime &&
        candidateTime > nowTime &&
        candidateTime < nextTime) {

      nextTime = candidateTime;
      nextIndex = i;
    }
  }

  // If nothing is after the current target,
  // wrap around to the earliest future target.
  if (nextIndex == -1) {

    uint32_t earliestTime = UINT32_MAX;

    for (int i = 0; i < NUM_TARGETS; i++) {

      DateTime candidate(
        targets[i].year,
        targets[i].month,
        targets[i].day,
        targets[i].hour,
        targets[i].minute,
        targets[i].second
      );

      uint32_t candidateTime = candidate.unixtime();

      if (candidateTime > nowTime &&
          candidateTime < earliestTime) {

        earliestTime = candidateTime;
        nextIndex = i;
      }
    }
  }

  // Update the current target
  if (nextIndex >= 0) {
    targetIndex = nextIndex;
    currentTarget = targets[targetIndex];
  }

  // Flash Target Event onscreen
  showTargetEvent();

}

// ----------- INFO DISPLAYS ----------- //
void showTargetEvent() {
  // line 1 event name
  lcdPrintCentered(currentTarget.name);

  // line 2 target date
  char targetDate[17];
  snprintf(targetDate, sizeof(targetDate), "%02d/%02d/%d", currentTarget.month, currentTarget.day, currentTarget.year);
  Serial.print("Target Date: "); Serial.println(targetDate);
  lcdPrintCentered(targetDate, 1);
  delay(displayDelay);
  lcd.clear();

}

void showCountdown() {  
  DateTime now = rtc.now();

  DateTime targetDate(
    currentTarget.year,
    currentTarget.month,
    currentTarget.day,
    currentTarget.hour,
    currentTarget.minute,
    currentTarget.second
  );

  int64_t remaining = (int64_t)targetDate.unixtime() - (int64_t)now.unixtime();

  // -------- COUNTED DOWN / EVENT ARRIVED or PASSED ------- //
  if (remaining < 0) {
    lcd.setBacklight(RED);
    lcdPrintCentered("Tick Tick Boom!");
    lcdPrintCentered(currentTarget.name, 1);

    return;
  }

  // --------- COUNTDOWN MATH ---------- //
  // Calculate days/hours/minutes/seconds
  int days = remaining / 86400;
  remaining %= 86400;

  int hours = remaining / 3600;
  remaining %= 3600;

  int minutes = remaining / 60;
  int seconds = remaining % 60;

  // Set unit words to singular if digit is 1; otherwise use the plural sense
  const char* dayText = (days == 1) ? "day" : "days";
  const char* hourText = (hours == 1) ? "hr" : "hrs";
  const char* minText = (minutes == 1) ? "min" : "mins";
  const char* secText = (seconds == 1) ? "sec" : "secs";

  char line1[17];
  char line2[17];

  // line for days hours
  snprintf(
    line1,
    sizeof(line1),
    "%d %s %d %s",
    days,
    dayText,
    hours,
    hourText
  );

  // line for mins secs
  snprintf(
    line2,
    sizeof(line2),
    "%02d %s %d %s",
    minutes,
    minText,
    seconds,
    secText
  );

  // use helper function to erase previous unit artifacts and properly refresh unit text
  printLine(0, line1);
  printLine(1, line2);
  
}

void printLine(int row, const char* text) {
  // Pad the line with blank spaces to erase previously displayed characters
  lcd.setCursor(0, row);

  // Print text
  lcd.print(text);

  // Fill remainder of line with spaces
  int length = strlen(text);

  for (int i = length; i < 16; i++) {
    lcd.print(" ");
  }
}

void cycleMessages() {
  messageIndex = (messageIndex + 1) % numMessages;

  Serial.print("Countdown to: "); Serial.println(fnMessages[messageIndex]);
  lcdPrintCentered("Countdown to");
  lcdPrintCentered(fnMessages[messageIndex], 1);
  delay(displayDelay);
  lcd.clear();
}

void cycleBacklight() {
  colorIndex = (colorIndex + 1) % numColors;

  Serial.print("Change backlight color.");
  lcd.setBacklight(backlightColors[colorIndex]);
}

// --------- BUTTON ACTIONS ---------- //
void checkButtons() {
  uint8_t buttons = lcd.readButtons();

  /* Button constants
  BUTTON_RIGHT
  BUTTON_LEFT
  BUTTON_UP
  BUTTON_DOWN
  BUTTON_SELECT
  */

  // Nothing pressed
  if (buttons == BUTTON_NONE) {
    lastButton = BUTTON_NONE;
    return;
  }

  // Ignore button while it is being held
  if (buttons == lastButton) {
    return;
  }

  // Remember this button press
  lastButton = buttons;

  if (buttons & BUTTON_LEFT) {
    Serial.println("\nLEFT Button Pressed");
    lcd.clear();
    lcd.setCursor(0,0);
    leftButton();
  }

  if (buttons & BUTTON_DOWN) {
    Serial.println("\nDOWN Button Pressed");
    lcd.clear();
    lcd.setCursor(0,0);
    downButton();
  }

  if (buttons & BUTTON_UP) {
    Serial.println("\nUP Button Pressed");
    lcd.clear();
    lcd.setCursor(0,0);
    upButton();
  }

  if (buttons & BUTTON_RIGHT) {
    Serial.println("\nRIGHT Button Pressed");
    lcd.clear();
    lcd.setCursor(0,0);
    rightButton();
  }

  if (buttons & BUTTON_SELECT) {
    Serial.println("\nSELECT Button Pressed");
    lcd.clear();
    lcd.setCursor(0,0);
    selectButton();
  }
      
}

void leftButton() {
  showCurrentTime();
}

void downButton() {
  cycleMessages();
}

void upButton() {
  cycleBacklight();
}

void rightButton() {
  showTargetEvent();
}

void selectButton() {
  // cycleTargetDates();
  orderedTargetDates();
}

// --------- MAIN LOOP ---------- //
void loop() {
  showCountdown();
  checkButtons();
  delay(1000);
}
