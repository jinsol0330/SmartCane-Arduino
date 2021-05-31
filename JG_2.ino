#include<Wire.h>
#include<SoftwareSerial.h>

int RX = 2;
int TX = 3;
SoftwareSerial MyBTSerial(RX, TX);

const int MPU_ADDR = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

double angleAcX, angleAcY, angleAcZ;
double angleGyX, angleGyY, angleGyZ;
double angleFiX, angleFiY, angleFiZ;

const double RADIAN_TO_DEGREE = 180 / 3.14159;
const double DEGREE_PER_SECOND = 32767 / 250;
const double ALPHA = 1 / (1 + 0.04);

unsigned long now = 0;
unsigned long past = 0;
unsigned long t1, t2;
int flag, flag2;
bool miss_case;
double dt = 0;

double baseAcX, baseAcY, baseAcZ;
double baseGyX, baseGyY, baseGyZ;

const int touchSensor = 4;  // 터치센서 핀 설정
//const int buzzerSensor = 5;  // 수동 버저 핀 설정

void setup() {
  initSensor();
  Serial.begin(115200);
  MyBTSerial.begin(9600);
  pinMode(touchSensor, INPUT);
//  pinMode(buzzerSensor, OUTPUT);
  calibrateSensor();
  past = millis();
  t1 = millis();
  flag = 0;
  flag2 = 0;
  miss_case = false;
}

void loop() {
  getData();
  getDT();
  miss_cane();
}

void initSensor() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void getData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);
 
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
}

void getDT() {
  now = millis();
  dt = (now - past) / 1000.0;
  past = now;
}

void calibrateSensor() {
  double sumAcX = 0, sumAcY = 0, sumAcZ = 0;
  double sumGyX = 0, sumGyY = 0, sumGyZ = 0;
  getData();
  for (int i = 0; i < 10 ; i++) {
    getData();
    sumAcX += AcX; sumAcY += AcY; sumAcZ += AcZ;
    sumGyX += GyX; sumGyY += GyY; sumGyZ += GyZ;
  }
  baseAcX = sumAcX / 10;
  baseAcY = sumAcY / 10;
  baseAcZ = sumAcZ / 10;
  baseGyX = sumGyX / 10;
  baseGyY = sumGyY / 10;
  baseGyZ = sumGyZ / 10;
}

void miss_cane() {
  int touchValue = digitalRead(touchSensor);
  int res = 1;
  int res2 = 2;
  
  angleAcY = atan(-AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2)));
  angleAcY *= RADIAN_TO_DEGREE;
  angleGyY += ((GyY - baseGyY) / DEGREE_PER_SECOND) * dt;
  double angleTmp = angleFiY + angleGyY * dt;
  angleFiY = ALPHA * angleTmp + (1.0 - ALPHA) * angleAcY;

  // 임계값 출력
//  Serial.println(angleFiY);
  
  if (touchValue == LOW) {
    flag2 = 0;
    t2 = millis();
    if (t2-t1 > 5000) {
      if (angleFiY < -20) {
//        Serial.println("Miss cane!!!!!");
        while(flag<1) {
           MyBTSerial.print((char)res);
           Serial.println(res);
           flag++;
           miss_case = true;
         }
//        tone(buzzerSensor, 700);
        }
      } else {
        flag = 0;
//        Serial.println("Not Touched!");
      }
  } else {
      if (miss_case) {
        while(flag2<1) {
          MyBTSerial.print((char)res2);
          Serial.println(res2);
          flag2++;
        }
        miss_case = false;
      }
      flag = 0;
      t1 = t2;
//    Serial.println("Touched!");
//    noTone(buzzerSensor);
  }
}
