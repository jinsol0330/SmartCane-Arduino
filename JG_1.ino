// LED Strip
#include <Adafruit_NeoPixel.h>  // Neopixel을 사용하기 위해서 라이브러리를 불러옴
#include <avr/power.h>
#define PIN 10    // 디지털 입력 핀 설정
#define NUMPIXELS 30    // Neopixel LED 소자 수 (LED가 24개라면 , 24로 작성)
#define BRIGHTNESS 255   // 밝기 설정 0(어둡게) ~ 255(밝게) 까지 임의로 설정 가능
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int lightSensor = A2; // 조도센서 핀 설정
const int vibrateSensor = 11; // 진동모터 핀 설정

#include<Servo.h>

// 정면
#define TRIG 4 //TRIG 핀 설정 (초음파 보내는 핀)
#define ECHO 5 //ECHO 핀 설정 (초음파 받는 핀)
// 좌측
#define TRIG2 6 //TRIG 핀 설정 (초음파 보내는 핀)
#define ECHO2 7 //ECHO 핀 설정 (초음파 받는 핀)
// 우측
#define TRIG3 8 //TRIG 핀 설정 (초음파 보내는 핀)
#define ECHO3 9 //ECHO 핀 설정 (초음파 받는 핀)

Servo servo;

int value = 0;
long duration = 0;
long distance = 0;
long duration2 = 0;
long distance2 = 0; 
long duration3 = 0;
long distance3 = 0; 
int anglepos =  100;

void setup() {
//  servo.attach(5);
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);
  pinMode(lightSensor, INPUT);
  pinMode(vibrateSensor, OUTPUT);
  strip.setBrightness(BRIGHTNESS);    // BRIGHTNESS 만큼 밝기 설정 
  strip.begin();    // Neopixel 제어를 시작
  strip.show();     // Neopixel 동작 초기화
}

void loop() {
  servo_pilot();
  dark_env();
}

void dark_env() {
  int lightValue = analogRead(lightSensor);
  
  if (lightValue <= 350) {
//    Serial.print(lightValue);
//    Serial.println("-----> Dark!!!!");
    colorWipe(strip.Color(255,255,255),0);  // white
  } else {
//    Serial.print(lightValue);
//    Serial.println("-----> Light!!!");
    colorWipe(strip.Color(0,0,0),0);
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i,c);
    strip.show();
  }
}

void servo_pilot() {
    // Trig 핀으로 10us의 pulse 발생
    digitalWrite(TRIG, LOW); 
    delayMicroseconds(2);   
    digitalWrite(TRIG, HIGH); 
    delayMicroseconds(10);            
    digitalWrite(TRIG, LOW);
    duration = pulseIn(ECHO, HIGH);
    distance = duration / 58.2;
      
    digitalWrite(TRIG2, LOW); 
    delayMicroseconds(2); 
    digitalWrite(TRIG2, HIGH); 
    delayMicroseconds(10);  
    digitalWrite(TRIG2, LOW);      
    duration2 = pulseIn(ECHO2, HIGH); 
    distance2 = duration2 / 58.2;

    digitalWrite(TRIG3, LOW); 
    delayMicroseconds(2); 
    digitalWrite(TRIG3, HIGH); 
    delayMicroseconds(10);  
    digitalWrite(TRIG3, LOW);      
    duration3 = pulseIn(ECHO3, HIGH); 
    distance3 = duration3 / 58.2; 
    
//    Serial.println(distance);
//    Serial.println(distance2);
//    Serial.println(distance3); 

    servo.attach(2);
    follow();    
    delay(200);
    servo.detach();
}

void follow() {
  // 정면만 감지
  if (distance < 30 && distance2 > 100 && distance3 > 100) {
    Serial.println("->3");
    anglepos = 40;
  } 
  // 좌측만 감지
  else if (distance > 100 && distance2 < 30 && distance3 > 100) {
    Serial.println("->5");    
    anglepos = 70;
  } 
  // 우측만 감지
  else if (distance > 100 && distance2 > 100 && distance3 < 30) {
    Serial.println("->4");
    digitalWrite(vibrateSensor, LOW);
    anglepos = 130;
  }
  // 정면 & 좌측 감지
  else if (distance < 30 && distance2 < 30 && distance3 > 100) {
    Serial.println("->3");
    digitalWrite(vibrateSensor, LOW);
    anglepos = 40;
  }
  // 정면 & 우측 감지
  else if (distance < 30 && distance2 > 100 && distance3 < 30) {
    Serial.println("->2");
    digitalWrite(vibrateSensor, LOW);
    anglepos = 160;
  }
  // 좌측 & 우측 감지
  else if (distance > 100 && distance2 < 30 && distance3 < 30) {
    Serial.println("->1");
    digitalWrite(vibrateSensor, LOW);
    anglepos = 100;
  }
  // 정면 & 좌측 & 우측 모두 감지 "O"
  else if (distance < 30 && distance2 < 30 && distance3 < 30) {
    Serial.println("->Stop!");
    digitalWrite(vibrateSensor, HIGH);
    delay(1000);
    anglepos = 100;
  }
  // 정면 & 좌측 & 우측 모두 감지 "X"
  else {
    Serial.println("->1");
    digitalWrite(vibrateSensor, LOW);
    anglepos = 100;
  }
  
  servo.write(anglepos);
}
