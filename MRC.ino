m#include <Wire.h>
#include "MAX30105.h"
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal.h>
#include <Servo.h> //Library servo
MAX30105 particleSensor; // initialize MAX30102 with I2C
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
Servo myservo;

//inisialisasi Global Variabel untuk Oximeternya
double avered = 0; 
double aveir = 0;
double sumirrms = 0;
double sumredrms = 0;
double frate = 0.95; //low pass filter for IR/red LED value to eliminate AC component
#define USEFIFO

float celsius;

void setup()
{
  Serial.begin(115200);
  //while(!Serial); //We must wait for Teensy to come online
  //delay(100);
  //Serial.println("");
  //Serial.println("MAX30102");
  //Serial.println("");
  //delay(100);
  // Initialize sensor
  particleSensor.begin(Wire, I2C_SPEED_FAST); //Use default I2C port, 400kHz speed

  byte ledBrightness = 70; //Options: 0=Off to 255=50mA
  byte sampleAverage = 1; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  int sampleRate = 400; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 69; //Options: 69, 118, 215, 411
  int adcRange = 16384; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

  Serial.begin(9600); //Seriar untuk Oximeternya
  lcd.begin(16, 2);
  mlx.begin();

  myservo.attach(8);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13,OUTPUT);
}
 
void loop() {
  uint32_t ir, red , green;
  double fred, fir;
  double SpO2 = 0; //raw SpO2 before low pass filtered

  particleSensor.check(); //Check the sensor

  red = particleSensor.getFIFORed(); //why getFOFOIR output Red data by MAX30102 on MH-ET LIVE breakout board
  ir = particleSensor.getFIFOIR(); //why getFIFORed output IR data by MAX30102 on MH-ET LIVE breakout board

    fred = (double)red;
    fir = (double)ir;
    avered = avered * frate + (double)red * (1.0 - frate);//average red level by low pass filter
    aveir = aveir * frate + (double)ir * (1.0 - frate); //average IR level by low pass filter
    sumredrms += (fred - avered) * (fred - avered); //square sum of alternate component of red level
    sumirrms += (fir - aveir) * (fir - aveir);//square sum of alternate component of IR level

    double R = (sqrt(sumredrms) / avered) / (sqrt(sumirrms) / aveir);
    // Serial.println(R);
    SpO2 = 112 - (17*R);  //http://ww1.microchip.com/downloads/jp/AppNotes/00001525B_JP.pdf
    particleSensor.nextSample(); //We're finished with this sample so move to next sample
    Serial.println(SpO2);
    //Serial.println(",");Serial.println(R);


  celsius = mlx.readObjectTempC();
  celsius = celsius + 2;
  lcd.setCursor(0, 0);
  lcd.print("SuhuBdn :");
  lcd.print(celsius);
  lcd.print("C"); 

  lcd.setCursor(0, 1);
  lcd.print("SpO2    :");
  lcd.print(SpO2);
  lcd.print("%");

  if (celsius <= 35.3){
    myservo.write(-15);
    digitalWrite(10, HIGH);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);// LED RED
    digitalWrite(13, LOW);// Buzzer
  }

  if (celsius <= 37.6 && celsius >= 35.3){
    myservo.write(75);
    delay(15);
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);//LED BLUE
    digitalWrite(12, LOW);// LED RED
    digitalWrite(13, LOW);// Buzzer

  }

  if(celsius >=37.7) {
    myservo.write(-15);
    delay(15);
    digitalWrite(10, LOW);  
    digitalWrite(11, LOW);
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);
    delay(15);
  }

  }  
