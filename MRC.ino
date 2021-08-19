#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
int pos = 0; 
#include <Wire.h>
#include "MAX30105.h"
#include <LiquidCrystal.h>
#include <Servo.h> //Library servo
MAX30105 particleSensor; // initialize MAX30102 with I2C
#define USEFIFO
uint32_t ir, red , green;
double fred, fir;
double SpO2 = 0; //raw SpO2 before low pass filtered
//inisialisasi Global Variabel untuk Oximeternya
double avered = 0; 
double aveir = 0;
double sumirrms = 0;
double sumredrms = 0;
double celsius;
double frate = 0.95; //low pass filter for IR/red LED value to eliminate AC component
byte ledBrightness = 70; //Options: 0=Off to 255=50mA
byte sampleAverage = 1; //Options: 1, 2, 4, 8, 16, 32
byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
int sampleRate = 400; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
int pulseWidth = 69; //Options: 69, 118, 215, 411
int adcRange = 16384; //Options: 2048, 4096, 8192, 16384
Servo myservo;

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit MLX90614 test");  
  mlx.begin();  
  myservo.attach(8);
}

void loop() {

  
  celsius = mlx.readObjectTempC();
  celsius = celsius + 2;
  Serial.print("Celcius sekarang adalah : ");
  Serial.print(celsius);


  if (celsius <= 35.3){
    Serial.println("Suhu dibawah 35.3 :");
     Serial.println(celsius);
    myservo.write(-15);
    digitalWrite(10, HIGH);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);// LED RED
    digitalWrite(13, LOW);// Buzzer
  }

  if (celsius <= 37.6 && celsius >= 35.3){
    myservo.write(75);
    Serial.println("Suhu diantara 35.3 s/d 37.6 :");
     Serial.println(celsius);
    delay(15);
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);//LED BLUE
    digitalWrite(12, LOW);// LED RED
    digitalWrite(13, LOW);// Buzzer

  }

  if(celsius >=37.7) {
    Serial.println("Suhu diatas 37.7 :" );
    Serial.println(celsius);
    myservo.write(-15);
    delay(15);
    digitalWrite(10, LOW);  
    digitalWrite(11, LOW);
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);

  }
  delay(500);
}
