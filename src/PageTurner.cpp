#include <Arduino.h>
#include <BleKeyboard.h>

BleKeyboard bleKeyboard("PageTurner", "dgd-soft", 100);

const float batVoltageLevels[] = {3.45,3.68,3.74,3.77,3.79,3.82,3.87,3.92,3.98,4.06};

#define BTN_LEFT_PIN 5
#define BTN_RIGHT_PIN 4
#define VOLTAGE_PIN 34

#define POWER_LED_PIN 18
#define BT_LED_PIN 19

class CTimer
{
  private:
    unsigned long lastMillis;
  public:
    CTimer()
    {
      lastMillis = millis();
    }

    void EveryNMillis(unsigned long ms, void (*callback)())
    {
      if ((millis() - lastMillis) > ms)
      {
        lastMillis += ms;
        callback();
      }
    }

    void EveryNSeconds(unsigned int s, void (*callback)())
    {
      EveryNMillis(s*1000,callback);
    }
};

class CLed
{
  private:
    unsigned long lastMillis=0;
    unsigned long delay=0;
    enum Mode {off,on,blink} mode = off;
    int pin;
    int blinkState=LOW;
  public:

    CLed(int pin) : pin(pin) {
      pinMode(pin,OUTPUT);
    }

    void On()
    {
      mode = on;
      digitalWrite(pin,HIGH);
    }

    void Off()
    {
      mode = off;
      digitalWrite(pin,LOW);
    }

    void Blink(unsigned long delay)
    {
      mode = blink;
      this->delay = delay;
      lastMillis = millis();
      int blinkState=HIGH;
      digitalWrite(pin,HIGH);
    }

    void update()
    {
      if (mode==blink)
      {
        if (millis()-lastMillis > delay)
        {
          blinkState = (blinkState==HIGH)?LOW:HIGH;
          digitalWrite(pin,blinkState);
          lastMillis = millis();
        }
      }
    }
};

class ButtonHandler 
{
  public:
    typedef void (*Callback)(int pin );
    typedef void (*DurationCallback)(int pin, bool longPress);

  private:
        uint8_t pin;
        int buttonState = HIGH;
        int lastState;
        unsigned long lastDebounceTime;
        unsigned long pressedTime;
        const unsigned long debounceDelay = 20;
        unsigned long longPressDuration;

        Callback onPress;
        DurationCallback onRelease;

  public: 
    ButtonHandler(uint8_t pin, Callback onPress = nullptr, DurationCallback onRelease = nullptr, unsigned long longPressDuration = 1000)
        : pin(pin), onPress(onPress), onRelease(onRelease), longPressDuration(longPressDuration) 
    {
        pinMode(pin, INPUT_PULLUP);

        lastState = digitalRead(pin);
        lastDebounceTime = 0;
        pressedTime = 0;
    }

    void update() 
    {
        int reading = digitalRead(pin);
        unsigned long currentTime = millis();

        if (reading != lastState) {
              lastDebounceTime = currentTime;
        }

        if ((currentTime - lastDebounceTime) > debounceDelay) {
              if (reading != buttonState) {
                    buttonState = reading;

                    if (buttonState == LOW) {
                        pressedTime = currentTime;
                        if (onPress) onPress(pin);
                    } else {
                        unsigned long pressDuration = currentTime - pressedTime;
                        if (onRelease) onRelease(pin,pressDuration >= longPressDuration);
                    }
              }
        }

        lastState = reading;
    }
};
  
void waitForConnection()
{
  CLed btLed(BT_LED_PIN);

  Serial.println("Starting BLE work!");

  btLed.Blink(200);
  while(!bleKeyboard.isConnected()) 
  { 
    Serial.print("."); 
    btLed.update();
    delay(100);
  } 
  Serial.println();
  Serial.println("Connected!");
  btLed.Off();
}

void ButtonPressed(int pin)
{
  switch(pin)
  {
    case BTN_LEFT_PIN:
      Serial.println("left pressed");
    break;
    case BTN_RIGHT_PIN:
      Serial.println("right pressed");
    break;  
  }
}

void ButtonReleased(int pin, bool longPress)
{
  switch(pin)
  {
    case BTN_LEFT_PIN:
      Serial.print("left released:"); Serial.println(longPress);
      if (longPress)
        bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
      else
        bleKeyboard.write(KEY_PAGE_UP);
      break;
    case BTN_RIGHT_PIN:
      Serial.print("right released:"); Serial.println(longPress);
      if (longPress)
        bleKeyboard.write(KEY_RIGHT_ARROW);
      else
        bleKeyboard.write(KEY_PAGE_DOWN);
    break;  
  }
}

ButtonHandler btnLeft(BTN_LEFT_PIN,nullptr,ButtonReleased,500);
ButtonHandler btnRight(BTN_RIGHT_PIN,nullptr,ButtonReleased,500);
CLed powerLed(POWER_LED_PIN);

void setup() {
  // Setup PINs

  pinMode(VOLTAGE_PIN, INPUT);
  
  // setup ADC
  analogSetAttenuation(ADC_11db);
  analogReadResolution(10);

  powerLed.On();
  Serial.begin(115200);
  bleKeyboard.begin();
  waitForConnection();
}

bool CheckButtonChanged(int btnPin, int &currentVal)
{
  int val = digitalRead(btnPin);
  //Serial.print("PIN_"); Serial.print(btnPin); Serial.print(" = "); Serial.print(val); Serial.print(" currentVal="); Serial.println(currentVal);
  if (val != currentVal)
  {
    currentVal = val;
    return true;
  }
  return false;
}

void UpdateBatteryPercent()
{
  /* Voltage Devider = R1 / R2 = 1M / 820K
   *  V2/Vges = R2/Rges => V2 = (R2/Rges)*Vges
   *  Vges/V2 = Rges/R2 => Vges = (Rges/R2)*V2
   * 
   *  4.2V Battery = 2.308V Input  
   * 
   * Max Analog Read = 2450mV (with ADC_ATTEN_DB_11)
   */

  // Sum 16 Measurements to reduce noise
  unsigned long val=0; for(int i=0;i<128;i++) val+=analogRead(VOLTAGE_PIN);
  Serial.print("analogRead = "); Serial.println(val/128);
  
  float vIn = (val >> 7) * (2.450 / 1024.0);
  Serial.print("vIn = "); Serial.println(vIn);

  float vBatt = vIn * 2.63;
  Serial.print("vBatt = "); Serial.println(vBatt);

  int level=0;
  while(level<10 && vBatt >= batVoltageLevels[level]) level++;
  Serial.print("battLevel = "); Serial.println(level*10);
  bleKeyboard.setBatteryLevel(level*10);

  if (level <= 3)
    powerLed.Blink(100);
  else if(level <=5 )
    powerLed.Blink(500);
  else 
    powerLed.On();

}  

void CheckButtons()
{
  btnLeft.update();
  btnRight.update();
}

void loop() {
  static CTimer batteryTimer;

  batteryTimer.EveryNSeconds(60,UpdateBatteryPercent);
  powerLed.update();
  CheckButtons();
}


