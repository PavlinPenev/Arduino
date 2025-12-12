#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27,16,2);
volatile int DarknessLevel;
volatile int Environmental_sound;

const int PHOTOCELL_PIN = A1;
const int SOUND_SENSOR_PIN = 2;
const int DOOR_LED_PIN = 7;
const int DOOR_LED_ON_TIME = 3000;
const int DARKNESS_TRESHOLD = 620;

const int COIN_SLOT_PIN = A0;
const int PARKING_SERVO_PIN = 11;
int IsCoinInserted;
Servo ParkingServo;

const int RAIN_SENSOR_PIN = A2;
const int WINDOW_SERVO_PIN = 10;
const int RAIN_TRESHOLD = 100;
int RainLevel;
Servo WindowServo; 

const int FAN_PIN = 6;
const int FLAME_SENSOR_PIN = 8;
const int ALARM_PIN = 3;
const int FIRE_LED_PIN = 12;
int flame;

const int HUMIDITY_TEMP_SENSOR_PIN = 13;
const int SENSOR_TYPE = 11;
int temperature;
int humidity;
DHT dht12(HUMIDITY_TEMP_SENSOR_PIN, SENSOR_TYPE);

const int DOOR_SERVO_PIN = 9;
const int RED_BUTTON_PIN = 4;
const int GREEN_BUTTON_PIN = 5;
const String password = "GRRG"; 
const unsigned long INPUT_TIMEOUT = 4000;
unsigned long lastPressTime = 0;
Servo DoorServo;
String passwordInput = "";
String asterisksInput = "";
bool prevGreenButtonState;
bool prevRedButtonState;

void setup()
{
  runLcd();
  prepareDoorLedJob();  
  prepareParking();
  prepareRainSensor();
  prepareFlameAlarm();
  prepareTempHumidityControl();
  prepareDoorPassword();
}


void loop()
{
  runDoorLedJob();
  handleCoinInsert();
  handleRainLevel();
  handleFire();
  handleTempAndHumidity();
  handlePasswordButtonPress();
}

void runLcd() 
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  // lcd.print("Pavkata");
  // lcd.setCursor(0, 1);
  // lcd.print("WeLoveDigi TL");
}

void prepareDoorLedJob()
{
  DarknessLevel = 0;
  Environmental_sound = 0;
  Serial.begin(9600);
  pinMode(PHOTOCELL_PIN, INPUT);
  pinMode(SOUND_SENSOR_PIN, INPUT);
  pinMode(DOOR_LED_PIN, OUTPUT);
}

void runDoorLedJob()
{
  DarknessLevel = analogRead(PHOTOCELL_PIN);
  Environmental_sound = digitalRead(SOUND_SENSOR_PIN);

  if (DarknessLevel > DARKNESS_TRESHOLD) 
  {
    if (Environmental_sound == 1) 
    {
      digitalWrite(DOOR_LED_PIN, HIGH);
      delay(DOOR_LED_ON_TIME);
      digitalWrite(DOOR_LED_PIN, LOW);
    }
  } 
  else 
  {
    digitalWrite(DOOR_LED_PIN, LOW);
  }
}

void prepareParking()
{
  IsCoinInserted = 0;
  pinMode(COIN_SLOT_PIN, INPUT);
  ParkingServo.attach(PARKING_SERVO_PIN);
}

void handleCoinInsert()
{
  IsCoinInserted = digitalRead(COIN_SLOT_PIN);

  // Sensor gives a 0 value when there is a coin input
  if (IsCoinInserted == 0)
  {
    ParkingServo.write(180);
    delay(0);
  }
  else if (IsCoinInserted == 1)
  {
    ParkingServo.write(90);
    delay(0);
  }
}

void prepareRainSensor()
{
  RainLevel = 0;
  pinMode(RAIN_SENSOR_PIN, INPUT);
  WindowServo.attach(WINDOW_SERVO_PIN);
  WindowServo.write(0);
}

void handleRainLevel()
{
  RainLevel = analogRead(RAIN_SENSOR_PIN);
  
  if (RAIN_TRESHOLD < RainLevel)
  {
    WindowServo.write(90);
  }
  else 
  {
    WindowServo.write(0);
  }
}

void prepareFlameAlarm()
{
  flame = 0;
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(FIRE_LED_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
}

void handleFire()
{
  flame = digitalRead(FLAME_SENSOR_PIN);

  if (flame == 1)
  {
    setFanPower(255);
    digitalWrite(FIRE_LED_PIN, HIGH);
    tone(ALARM_PIN, 3532);
  }
  else
  {
    setFanPower(0);
    digitalWrite(FIRE_LED_PIN, LOW);
    noTone(ALARM_PIN);
  }
}

void prepareTempHumidityControl()
{
  temperature = 0;
  humidity = 0;
  dht12.begin();
}

void handleTempAndHumidity()
{
  int currentTemp = dht12.readTemperature();
  int currentHumidity = dht12.readHumidity();

  if(currentTemp != temperature || currentHumidity != humidity)
  {
    Serial.println("In Display temp humidity");
    lcd.clear();
    temperature = currentTemp;
    humidity = currentHumidity; 
    printTempAndHumidity();
  }

  if(temperature > 30)
  {
    setFanPower(255);
  } else {
    setFanPower(0);
  }
}

void setFanPower(int speed)
{
  if (speed <= 0)
  {
    analogWrite(FAN_PIN, 0);
  }
  else if (speed > 255)
  {
    analogWrite(FAN_PIN, 255);
  }
  else
  {
    analogWrite(FAN_PIN, speed);
  }
}

void prepareDoorPassword()
{
  pinMode(GREEN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
  DoorServo.attach(DOOR_SERVO_PIN);
  DoorServo.write(75);
}

void handlePasswordButtonPress()
{
  bool isGreenPressed = digitalRead(GREEN_BUTTON_PIN);
  bool isRedPressed = digitalRead(RED_BUTTON_PIN);

  if (isGreenPressed == LOW && prevGreenButtonState == HIGH)
  {
    pressButton('G');
  }

  if (isRedPressed == LOW && prevRedButtonState == HIGH)
  {
    pressButton('R');
  }

  prevGreenButtonState = isGreenPressed;
  prevRedButtonState = isRedPressed;
}

void pressButton(char button)
{
  tone(ALARM_PIN, 2000, 80);
  passwordInput += button;
  asterisksInput += "*";

  lcd.clear();
  lcd.setCursor(0, 0);
  
  if (passwordInput.length() < password.length())
  {
    lcd.print("Password:");
    lcd.setCursor(0, 1);
    lcd.print(asterisksInput);
  }
  else 
  {
    if (password == passwordInput)
    {
      lcd.print("Password");
      lcd.setCursor(0, 1);
      lcd.print("correct!");
      DoorServo.write(180);
      delay(3000);
      DoorServo.write(75);
    }
    else
    {
      lcd.print("Password");
      lcd.setCursor(0, 1);
      lcd.print("incorrect!");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Password:");
      lcd.setCursor(0, 1);
      lcd.print(asterisksInput);
    }

    resetInput();
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    printTempAndHumidity();
  }
}

void resetInput()
{
  passwordInput = "";
  asterisksInput = "";
}

void printTempAndHumidity()
{
  lcd.print("Temperature: ");
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
}