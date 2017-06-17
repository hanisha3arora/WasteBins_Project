#include "QuickStats.h" 

const int pwPin1 = 11;
long pulse1, sensor1, feet;
const int MAX_NUM = 40;
int current_num = 0;
float arr[MAX_NUM];
QuickStats stats;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pwPin1, INPUT);
}
void read_sensor() {
  pulse1 = pulseIn(pwPin1, HIGH); //inches
  sensor1 = pulse1 / 147; //total inches
  feet = sensor1 / 12; 
  arr[current_num] = sensor1;
  current_num++;
}

void printAll() {
  Serial.print("total inches-");
  Serial.print(sensor1);
  Serial.print(" feet - ");
  Serial.println(feet);
}

void averageFunc() {
  float averageInches = 0;
  int averageFeet = 0;
  float finalInches = 0;
  float sum = 0;
  for(int i = 0; i < MAX_NUM; ++i) {
    sum += arr[i];
  }
  averageInches = sum / MAX_NUM;
  averageFeet = averageInches / 12;
  finalInches = fmod(averageInches, 12.0);
  Serial.print("Average - feet ");
  Serial.print(' ');
  Serial.print(averageFeet);
  Serial.print(' ');
  Serial.print("Average - inches ");
  Serial.println(finalInches);
  Serial.print("Total Average Inches: ");
  Serial.println(averageInches); 
  if(averageFeet < 8) {
    Serial.println("Car is here");
  }
  else {
    Serial.println("Car is not here");
  }
}

void modeFunc() {
  float modeInches = stats.mode(arr, MAX_NUM, .000001);
  float finalInches = 0;
  int modeFeet = modeInches / 12;
  finalInches = fmod(modeInches, 12.0);
  Serial.print("Total Mode Inches: ");
  Serial.print(modeInches);
  Serial.print(' ');
  Serial.print("Feet- ");
  Serial.print(modeFeet);
  Serial.print(' ');
  Serial.print("Inches- ");
  Serial.println(finalInches);
  if(modeFeet < 8) {
    Serial.println("Car is here");
  }
  else {
    Serial.println("Car is not here");
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  read_sensor();
  printAll();
  if(current_num == MAX_NUM) {
    averageFunc();
    modeFunc();
    delay(2000);
    current_num = 0;
  }
  delay(1000);
}
