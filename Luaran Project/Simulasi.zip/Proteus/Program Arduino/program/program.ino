#include <TimerOne.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>


// Pin definition Buzzer
#define  Pin_Buzzer 2

#define OOM_PIN A2

// Konfigurasi nilai kalibrasi sensor Pulse
#define Pulse_pin A1
int HBCount = 0;
int HBCheck = 0;
int TimeinSec = 0;
int HBperMin = 0;
int HBStart = 5;
int HBStop = 4;
int HBFinal=0;
bool HBStartCheck = false;
int count;

// Konfigurasi nilai kalibrasi sensor oksigen
const float minOxygenPercentage = 0;   // Persentase O2 minimum yang diukur oleh sensor oksigen
const float maxOxygenPercentage = 100; // Persentase O2 maksimum yang diukur oleh sensor oksigen

// Konfigurasi nilai kalibrasi sensor MPX
#define pressurePin A0
#define kpa2atm 0.00986923267
int val;
float pkPa; // pressure in kPa
float pAtm; // pressure in Atm
unsigned long time;

// Buat objek ILI9341 dengan pin yang sesuai
Adafruit_ILI9341 tft = Adafruit_ILI9341(8, 9, 11, 13, 10, 12);

void timerIsr()
{
  if(HBStartCheck == 1)
  {
      TimeinSec = TimeinSec + 1;
  }
}

int simulateHeartRate() {  // Calls function on our pulseSensor object that returns BPM as an "int".
  if(digitalRead(HBStart) == LOW){
    HBStartCheck = true;
  }
  if(digitalRead(HBStop)== LOW){
    HBStartCheck = false;
    HBCount=0;
    HBFinal=0;
    count=0;
  }
  HBStartCheck = true;
  if(HBStartCheck){
    if(digitalRead(Pulse_pin) == HIGH && count < 10)
      {
        HBCount = HBCount + 1;
        HBCheck = 1;
        return HBFinal = HBCount*6;
      }
    }
}

float simulateBloodOxygenLevel() {
  float OOM = analogRead(OOM_PIN);
  OOM = map(OOM, 0, 1023, 0, 100);
  return OOM; 
}

float simulatePressure() {
  val = analogRead(pressurePin);
  pkPa = ((float)val/(float)1023+0.095)/0.00009;
  pAtm = kpa2atm*pkPa;
  return pAtm;
}

void displayData(int& pulse, float& oximeter, float& bloodPressure) {
  tft.fillRect(0, 0, 320, 240, ILI9341_BLACK); // Bersihkan layar

  tft.setCursor(10, 20);
  tft.print("Pulse: ");
  tft.print(pulse);
  tft.print(" bpm");

  tft.setCursor(10, 60);
  tft.print("OOM: ");
  tft.print(oximeter);
  tft.print(" %");

  tft.setCursor(10, 100);
  tft.print("MPX: ");
  tft.print(bloodPressure);
  tft.print(" kPa");

  if(pulse >= 30 || oximeter >= 50 || bloodPressure >= 50){
    tft.setCursor(10, 140);
    tft.print("Buzzer nyala");
    digitalWrite(Pin_Buzzer, HIGH);
  } else{
    digitalWrite(Pin_Buzzer, LOW);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(Pin_Buzzer, OUTPUT);
  pinMode(Pulse_pin, INPUT);
  pinMode(HBStart, INPUT_PULLUP);
  pinMode(HBStop, INPUT_PULLUP);
  Timer1.initialize(800000); 
  Timer1.attachInterrupt( timerIsr );
  tft.begin();
  tft.setRotation(1); // Orientasi landscape
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_RED); 
  tft.setTextSize(2);
}

void loop() {
  int heartRate = simulateHeartRate();
  float bloodOxygen = simulateBloodOxygenLevel();
  float pressure = simulatePressure();
  displayData(heartRate, bloodOxygen, pressure); // Tampilkan data pada layar
  Serial.println(String(heartRate) + " BPM");
  Serial.println(String(bloodOxygen) + " %");
  Serial.println(String(pressure) + " kPa");
}
