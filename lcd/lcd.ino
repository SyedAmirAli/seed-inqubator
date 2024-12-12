#include <LiquidCrystal.h>
#include <DHT22.h>

const int POT_PIN = 32; // GPIO 32 or analogpin
const int DHT_PIN = 4; // D4 or SDA supporting pin
const int LED_PIN = 23; // D4 or SDA supporting pin
const int STATUS_PAUSE_PIN = 12;
const int TEMP_AND_HUM_PAUSE_PIN = 12;
DHT22 dht22(DHT_PIN);

// Initialize the LCD with the appropriate pins based on your connections
// RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(21, 22, 14, 27, 26, 25); 

int pulseCounter = 0, state = 0, printState = 1, interval = 10000, start = 0, end = 0; // Variable to keep track of the pulse count
float temperature = 0.00, humidity = 0.00;
unsigned long prevMillis = millis();

void setup() {
  lcd.begin(16, 2); // Set up the LCD's number of columns and rows (16x2)
  lcd.print("Hello, World!"); // Print the first line
  
  // Allow time for the user to read "Hello, World!"
  delay(2000); 
  lcd.clear(); // Clear the LCD

  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(STATUS_PAUSE_PIN, INPUT_PULLUP);
  pinMode(TEMP_AND_HUM_PAUSE_PIN, INPUT_PULLUP);
}


// Equivalent to the JavaScript function
void findCondition(int n = 1, int step = 4, int startPoint = 8) {  
  int newStart = step * (n - 1) + 1;
  int newEnd = step * n;

  newStart += startPoint;
  newEnd += startPoint;

  start = newStart;
  end = newEnd;
}

int managePritState(){
  unsigned long currentMillis = millis(); // Get the current time

  // Check if 3 seconds have passed
  if (currentMillis - prevMillis >= interval) {
    prevMillis = currentMillis; // Update the last time the state was changed

    // Update the state and cycle back to 1 after reaching 3
    printState++;
    if (printState > 2) {
      printState = 1; // Reset to 1 if state exceeds 3
    }
  }

  return printState;
}

void loop() {
  // Measure potentiometer
  int potValue = analogRead(POT_PIN);
  state = map(potValue, 0, 4095, 1, 10);

  // Measure temparature & Humidity
  temperature = dht22.getTemperature();
  humidity = dht22.getHumidity();

  // manage prit state of lcd display
  findCondition(state, 5, 10);
  managePritState();
  printCurrentState();
  // manage print state handler
  
  // control the LED over the potentiometer state & temperature
  handleLed();

  delay(1000); // Wait for 1 second before the next update
}

void handleLed(){
  if(temperature < start){
    digitalWrite(LED_PIN, HIGH);
  } 

  if(temperature > end){
    digitalWrite(LED_PIN, LOW);
  }
}

void printTempHumidity(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: "); 
  lcd.print(temperature);
  lcd.print("C"); 

  lcd.setCursor(0, 1);
  lcd.print("Humidity: "); 
  lcd.print(humidity);
  lcd.print("%"); 
}

void pritLedConditionState(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("F: "); 
  lcd.print(start);
  lcd.print("C - "); 
  lcd.print("To: "); 
  lcd.print(end);
  lcd.print("C"); 

  lcd.setCursor(0, 1);
  lcd.print("LED: "); 
  if(digitalRead(LED_PIN) == HIGH){
    lcd.print("ON");
  } else {
    lcd.print("OFF");
  }
}

int printCurrentState(){
  bool state1 = digitalRead(STATUS_PAUSE_PIN) == HIGH;
  bool state2 = digitalRead(TEMP_AND_HUM_PAUSE_PIN) == HIGH;

  if(!state1 && !state2){
    if(printState == 1){
      pritLedConditionState();
    }

    if(printState == 2){
      printTempHumidity();
    }

    return 1;
  }

  if(state1 && state2){
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Invalid Keypress");

    return 0;
  }

  if(state1){
    pritLedConditionState();
    return 0;
  }

  if(state2){
    printTempHumidity();
    return 0;
  }
}

// void inititateDht22(){
//   // Serial.println(dht22.debug());

  

    // Print the current state to the serial monitor
    // Serial.print("Print State: ");
    // Serial.println(printState); // Call getState to print the current state

  // inititateDht22();

  // Serial.print("Resistance: ");
  // Serial.println(potValue);
  // Manage DHT22 Sensor


  // Set cursor to the beginning of the second line
  // lcd.setCursor(0, 1); 
  // lcd.print("Count: "); // Print label
  // Serial.print("Count: "); // Print label
  // Serial.println(pulseCounter); // Print the current count
  // lcd.print(pulseCounter); // Print the current count
  
  // pulseCounter++; // Increment the counter

//   lcd.setCursor(0, 0);
//   lcd.print("T: "); 
//   lcd.print(temperature);
//   lcd.print(" - H: "); 
//   lcd.print(humidity);

//   Serial.print("Temperature: ");
//   Serial.print(temperature);
//   Serial.print("*C   ");
//   Serial.print(humidity);
//   Serial.print("%    ");
//   Serial.print("Pot State: ");
//   Serial.println(state);
// }