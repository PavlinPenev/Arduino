#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

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

void setup()
{
  runLcd();
  prepareDoorLedJob();  
  prepareParking();
  prepareRainSensor();
}


void loop()
{
  runDoorLedJob();
  handleCoinInsert();
  handleRainLevel();
}

void runLcd() 
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Pavkata");
  lcd.setCursor(0, 1);
  lcd.print("WeLoveDigi TL");
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
  //Serial.println(DarknessLevel);
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
  Serial.println(IsCoinInserted);

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
  Serial.println("Rain Level: " + RainLevel);
  if (RAIN_TRESHOLD < RainLevel)
  {
    WindowServo.write(90);
  }
  else 
  {
    WindowServo.write(0);
  }
}