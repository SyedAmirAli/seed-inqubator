#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <DHT22.h>
#include <Arduino.h>

// Define LCD pin connections
const int RS = 21, EN = 22, D4 = 14, D5 = 27, D6 = 26, D7 = 25, 
  DHT_PIN = 4,
  RELAY_PIN = 15,
  INCREMENT_BTN = 19,
  DECREMENT_BTN = 18,
  MODE_BTN = 5,
  SAVE_BTN = 17,
  DISPLAY_CONTROL_BTN = 23,
  DISPLAY_RELAY_PIN = 33;

int minTemp = 0, maxTemp = 0, mode = 0, setting = 0, displayState = 0;

unsigned long lastDebounceTime[6] = {0, 0, 0, 0, 0, 0};
const unsigned long debounceDelay = 200;

float temperature = 0.00, humidity = 0.00;

DHT22 dht22(DHT_PIN);
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void setup() {
  lcd.begin(16, 2); 
  lcd.setCursor(0, 0);
  lcd.print(" Sammo Agro LTD ");
  lcd.setCursor(0, 1);
  lcd.print("ESP32 Initialized");

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(DISPLAY_RELAY_PIN, OUTPUT);

  pinMode(INCREMENT_BTN, INPUT_PULLUP); // index 1
  pinMode(DECREMENT_BTN, INPUT_PULLUP); // index 2
  pinMode(MODE_BTN, INPUT_PULLUP); // index 3
  pinMode(SAVE_BTN, INPUT_PULLUP); // index 4
  pinMode(DISPLAY_CONTROL_BTN, INPUT_PULLUP); // index 5

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(DISPLAY_RELAY_PIN, LOW);

  // For debugging
  Serial.begin(115200);

  // firstly initialize the memory
  loadFromEEPROM();

  delay(5000); 
  lcd.clear();
}

void loop() {
  // Measure temperature & humidity
  temperature = dht22.getTemperature();
  humidity = dht22.getHumidity();

  // Print on LCD
  manageDisplay();

  // Handle button presses
  manageIncrDecrBtn();

  // Handle Mode Button Press
  manageMode();

  // Handle Save and Display Button
  managesetting();

  // Handle the heating LED/SWITCH/ELEMENT etc.
  syncRelay();

  // Handle EEPROM data saving
  saveSettings(); 

  // handle display status like on & of & duration
  manageseDisplayBtn();

  // handle LCD display status in display
  manageLCDDisplay();

  // handle restart button
  restart();

  delay(100);
}

void syncRelay() {
  if (temperature < minTemp) {
    // If current temperature is below minTemp, turn the relay ON
    digitalWrite(RELAY_PIN, HIGH);
    // Serial.println("Relay ON: Temperature below minTemp.");
  } else if (temperature > maxTemp) {
    // If current temperature exceeds maxTemp, turn the relay OFF
    digitalWrite(RELAY_PIN, LOW);
    // Serial.println("Relay OFF: Temperature above maxTemp.");
  }
}

void manageDisplay(){
  switch (setting) {
    case 0:
      printTempHumidity();
      break;
    case 1:
      setedTemperature();
      break;
    case 2:
      showHeatingStatus();
      break;
    case 3:
      showDisplayStatus();
      break;
    case 4:
      showMode();
      break;
    case 5:
      displaySaveMessage();
      break;
    case 6:
      displayResetMessage();
      break;
    default:
      showMode();
      break;
  }
}

void showHeatingStatus(){
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Heating LED/ELEM");

  lcd.setCursor(0, 1);
  if(digitalRead(RELAY_PIN) == HIGH){
    lcd.print("     ON - 1");
  } else {
    lcd.print("    OFF - 0");
  }
}

void showMode(){
  lcd.clear();
  
  if(mode == 0){
    lcd.setCursor(0, 0);
    lcd.print("  NO INCR/DECR ");
  }
  
  if(mode == 1){
    lcd.setCursor(0, 0);
    lcd.print("Increment Temp");
  }

  if(mode == 2){
    lcd.setCursor(0, 0);
    lcd.print("Decrement Temp");
  }

  lcd.setCursor(0, 1);
  lcd.print("MODE: ");
  lcd.print(mode);
  lcd.print(" SETNG: ");
  lcd.print(setting);
}

void setedTemperature(){
  lcd.clear();

  if(mode == 1 && setting == 1){
    lcd.setCursor(0, 0);
    lcd.print(" +/- MIN Tempe ");
  } else if(mode == 2 && setting == 1){
    lcd.setCursor(0, 0);    
    lcd.print(" +/- MAX Tempe ");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Set Temperature: ");
  }
  
  lcd.setCursor(0, 1);
  lcd.print("F: ");
  lcd.print(minTemp);
  lcd.print("C");
  lcd.print(" - ");
  lcd.print("T: ");
  lcd.print(maxTemp);
  lcd.print("C");
}

void printTempHumidity() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("  Temp: ");
  lcd.print(temperature, 1);
  lcd.print("C ");

  lcd.setCursor(0, 1);
  lcd.print("Humidty: ");
  lcd.print(humidity, 1);
  lcd.print("% ");
}

void manageIncrDecrBtn() {
  switch (mode) {
    case 1:
      if (pushing(INCREMENT_BTN, 0)) {
        minTemp++;
        // Serial.println("Increment Min Temp");
      }
      if (pushing(DECREMENT_BTN, 1)) {
        minTemp--;
        // Serial.println("Decrement Min Temp");
      }
      break;
    case 2:
      if (pushing(INCREMENT_BTN, 0)) {
        maxTemp++;
        // Serial.println("Increment Max Temp");
      }
      if (pushing(DECREMENT_BTN, 1)) {
        maxTemp--;
        // Serial.println("Decrement Max Temp");
      }
      break;
    default:
      break;
  }
}

bool pushing(int btnPin, int btnIndex) {
  static int lastState[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
  int currentState = digitalRead(btnPin);

  if (currentState == LOW && lastState[btnIndex] == HIGH && 
      (millis() - lastDebounceTime[btnIndex] > debounceDelay)) {
    lastDebounceTime[btnIndex] = millis();
    lastState[btnIndex] = currentState;
    return true;
  }

  lastState[btnIndex] = currentState;
  return false;
}

void manageMode(){
  if(pushing(MODE_BTN, 3)){
    mode++;
    if(mode >= 3) mode = 0;

    // Serial.print("MODE PUSHING  => ");
    // Serial.println(mode);
    Serial.print("DISPLAY CONTROL STATE => ");
    Serial.println(displayState);
  }
}

void managesetting(){
  if(pushing(SAVE_BTN, 4)){
    setting++;
    if(setting > 4) setting = 0;

    // Serial.print("SAVE PUSHING/SETTING  => ");
    // Serial.println(setting);
  }
}

void manageseDisplayBtn(){
  if(pushing(DISPLAY_CONTROL_BTN, 5)){
    displayState++;
    if(displayState > 2) displayState = 0;

    // Serial.print("LCD DISPLAY CONTROL SWITCH  => ");
    // Serial.println(displayState);
  }
}

void manageLCDDisplay() {
  static unsigned long displayTimer = 0;
  
    if (displayState == 0) {
        digitalWrite(DISPLAY_RELAY_PIN, LOW);
    } else if (displayState == 1) {
        digitalWrite(DISPLAY_RELAY_PIN, HIGH);
    } else if (displayState == 2) {
        if (displayTimer == 0) {
            displayTimer = millis();
            digitalWrite(DISPLAY_RELAY_PIN, HIGH);
        } else if (millis() - displayTimer >= 30000) {
            digitalWrite(DISPLAY_RELAY_PIN, LOW);
            displayTimer = 0; // Reset timer
            displayState = 0;
        }
    }
}

void showDisplayStatus(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" DISPLAY STATUS");
  
  if(displayState == 0){
    lcd.setCursor(0, 1);
    lcd.print(" DISPLAY IS OFF");
  }
  if(displayState == 1){
    lcd.setCursor(0, 1);
    lcd.print(" DISPLAY IS ON");
  }
  if(displayState == 2){
    lcd.setCursor(0, 1);
    lcd.print("DISPLAY ON 30sec");
  }
}

void saveSettings() {
  static unsigned long buttonPressStart = 0; // Track when the button was first pressed
  static bool buttonPressed = false;        // Track the button's press state

  if (digitalRead(MODE_BTN) == LOW) {
    if (!buttonPressed) {
      // Button was just pressed
      buttonPressStart = millis();
      buttonPressed = true;
    } else if (millis() - buttonPressStart >= 4000) {
      // Button has been held for 5 seconds
      saveToEEPROM(minTemp, maxTemp); // Save minTemp and maxTemp to persistent storage
    }
  } else {
    // Button released
    buttonPressed = false;
  }
}

void restart() {
  static unsigned long buttonPressStart = 0; // Track when the button was first pressed
  static bool buttonPressed = false;        // Track the button's press state

  if (digitalRead(DISPLAY_CONTROL_BTN) == LOW) {
    if (!buttonPressed) {
      // Button was just pressed
      buttonPressStart = millis();
      buttonPressed = true;
    }

    unsigned long holdDuration = millis() - buttonPressStart;

    if (holdDuration >= 3000) { 
      // Button held for 3 seconds: restart ESP
      setting = 6;  // Set loading message
      delay(1000);  // Optional: Display message for a short duration
      ESP.restart(); 
    } else if (holdDuration >= 500) {
      // Button held for 500 ms: show restart message
      setting = 6; 
    }
  } else {
    // Button released
    buttonPressed = false;
  }
}


void displayResetMessage(){
  // Serial.println("Settings saved!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Restarting ESP32");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");
  delay(3000);
  setting = 0;
}

void displaySaveMessage(){
  // Serial.println("Restarting MCU !");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Settings Saved!");
  delay(3000);
  setting = 1;
}

void saveToEEPROM(int minVal, int maxVal) {
  setting = 5;
  EEPROM.begin(512); // Initialize EEPROM with 512 bytes
  EEPROM.write(0, minVal); // Store minTemp at address 0
  EEPROM.write(1, maxVal); // Store maxTemp at address 1
  EEPROM.commit();         // Commit changes to EEPROM
}

void loadFromEEPROM() {
  EEPROM.begin(512);
  minTemp = EEPROM.read(0); // Load minTemp from address 0
  maxTemp = EEPROM.read(1); // Load maxTemp from address 1
}


