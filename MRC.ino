#include <Wire.h>
#include "MAX30105.h"
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal.h>
#include <Servo.h> //Library servo
MAX30105 particleSensor; // initialize MAX30102 with I2C
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
Servo myservo;
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

void setup()
{
  Serial.begin(9600);
  // Initialize sensor
  particleSensor.begin(Wire, I2C_SPEED_FAST); //Use default I2C port, 400kHz speed
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
  lcd.begin(16, 2); //inisialisasi lcd
  mlx.begin(); //mlx nya di begin
  myservo.attach(8); //servo nya di attach dengan mengambil pin 8 sebagai pin nya
  pinMode(10, OUTPUT); //pinmOde 10-13 sebagai outputnya
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13,OUTPUT);
}
 
void loop() {  //masuk ke program utama
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
    Serial.print("SpO2");
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
     Serial.println("Suhu dibawah 35.3 :");
     Serial.println(celsius);
    myservo.write(-15);
    digitalWrite(10, HIGH); //LED YELLOW
    digitalWrite(11, LOW); // LED BLUE
    digitalWrite(12, LOW);// LED RED
    digitalWrite(13, LOW);// Buzzer
  }

  if (celsius <= 37.6 && celsius >= 35.4){
      myservo.write(75);
    Serial.println("Suhu diantara 35.4 s/d 37.6 :");
     Serial.println(celsius);
    delay(15);
    digitalWrite(10, LOW); // LED YELLOW
    digitalWrite(11, HIGH);//LED BLUE
    digitalWrite(12, LOW);// LED RED
    digitalWrite(13, LOW);// Buzzer

  }

  if(celsius >= 37.7) {
    Serial.println("Suhu diatas 37.7 :" );
    Serial.println(celsius);
    myservo.write(-15);
    delay(15);
    digitalWrite(10, LOW);  // LED YELLOW
    digitalWrite(11, LOW);  //LED BLUE
    digitalWrite(12, HIGH); // LED RED
    digitalWrite(13, HIGH); // Buzzer
    delay(15);
  }

  } 
