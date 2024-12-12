#include <LiquidCrystal.h>
#include <DHT22.h>

// Define LCD pin connections
const int RS = 21, EN = 22, D4 = 14, D5 = 27, D6 = 26, D7 = 25, 
  DHT_PIN = 4,
  RELAY_PIN = 15,
  INCREMENT_BTN = 19,
  DECREMENT_BTN = 18,
  MODE_BTN = 5,
  SAVE_BTN = 17;

int minTemp = 0, maxTemp = 0, mode = 0;

unsigned long lastDebounceTime[4] = {0, 0, 0, 0};
const unsigned long debounceDelay = 200;

float temperature = 0.00, humidity = 0.00;

DHT22 dht22(DHT_PIN);
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void setup() {
  lcd.begin(16, 2); 
  lcd.setCursor(0, 0);
  lcd.print("ESP32 Initialized");

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(INCREMENT_BTN, INPUT_PULLUP);
  pinMode(DECREMENT_BTN, INPUT_PULLUP);
  pinMode(MODE_BTN, INPUT_PULLUP);
  pinMode(SAVE_BTN, INPUT_PULLUP);

  digitalWrite(RELAY_PIN, LOW);

  Serial.begin(115200); // For debugging
  delay(2000); 
  lcd.clear();
}

void loop() {
  // Measure temperature & humidity
  temperature = dht22.getTemperature();
  humidity = dht22.getHumidity();

  // Print on LCD
  printTempHumidity();

  // Handle button presses
  manageIncrDecrBtn();

  delay(250);
}

void printTempHumidity() {
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature, 1);
  lcd.print("C ");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity, 1);
  lcd.print("% ");
}

void manageIncrDecrBtn() {
  switch (mode) {
    case 0:
      if (pushing(INCREMENT_BTN, 0)) {
        minTemp++;
        Serial.println("Increment Min Temp");
      }
      if (pushing(DECREMENT_BTN, 1)) {
        minTemp--;
        Serial.println("Decrement Min Temp");
      }
      break;
    case 1:
      if (pushing(INCREMENT_BTN, 0)) {
        maxTemp++;
        Serial.println("Increment Max Temp");
      }
      if (pushing(DECREMENT_BTN, 1)) {
        maxTemp--;
        Serial.println("Decrement Max Temp");
      }
      break;
    default:
      break;
  }
}

bool pushing(int btnPin, int btnIndex) {
  static int lastState[4] = {HIGH, HIGH, HIGH, HIGH};
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
