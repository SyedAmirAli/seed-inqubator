const int INCREMENT_BTN = 2,
          DECREMENT_BTN = 3,
          MODE_BTN = 4,
          SAVE_BTN = 5;

int minTemp = 0,
    maxTemp = 0;

int mode = 0;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(INCREMENT_BTN, INPUT_PULLUP);
  pinMode(DECREMENT_BTN, INPUT_PULLUP);
  pinMode(MODE_BTN, INPUT_PULLUP);
  pinMode(SAVE_BTN, INPUT_PULLUP);

}

void loop() {
  manageMode();
  serialPrint();

  switch(mode){
    case 0:
      if(pushing(INCREMENT_BTN)){
        minTemp++;
      }
      if(pushing(DECREMENT_BTN)){
        minTemp--;
      }
      break;
    case 1:
      if(pushing(INCREMENT_BTN)){
        maxTemp++;
      }
      if(pushing(DECREMENT_BTN)){
        maxTemp--;
      }
      break;
    case 2:
      break;
    case 3:
      break;
    default:
      break;
  }
}

void manageMode(){
  if(pushing(MODE_BTN)){
    mode++;

    if(mode > 3){
      mode = 0;
    }
  }
}

void serialPrint(){
  Serial.print("Minimum Temperature: ");
  Serial.print(minTemp);
  Serial.print(";\t\tMaximum Temperature: ");
  Serial.println(maxTemp);

  delay(1000);
}

bool pushing(int btnPin){
  static int lastState = HIGH;
  int currentState = digitalRead(btnPin);

  if(currentState == LOW && lastState == HIGH && (millis() - lastDebounceTime) > debounceDelay){
    lastDebounceTime = millis();
    lastState = currentState;
    return true;
  }

  lastState = currentState;
  return false;
}


