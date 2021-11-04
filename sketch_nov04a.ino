//#define TAKE_SAMPLES

#include <arduinoFFT.h>

#ifndef TAKE_SAMPLES
#include "model.h"
//Eloquent::ML::Port::RandomForest clf;
Eloquent::ML::Port::SEFR clf;
//Eloquent::ML::Port::SVM clf;
#endif

#define INTERVAL    5
#define SAMPLE_NUM  128
#define MIC         A0
#define THRESHOLD   35


float features[SAMPLE_NUM];
double featuresForFFT[SAMPLE_NUM];
float mx;
float mn;
float denom;

int interval_led = millis();
bool should_blink = true;
bool led_state = HIGH;

arduinoFFT fft;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

int16_t readData(){
  return ((analogRead(MIC) - 512) >> 2);
}

void detectWord(){
  if(readData() > THRESHOLD+1){

    for(int i = 0; i < SAMPLE_NUM; i++){
      featuresForFFT[i] = readData() - THRESHOLD;
      delay(INTERVAL);
    }
asyncBlink();
    fft.Windowing(featuresForFFT, SAMPLE_NUM, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    mx = features[0];
    mn = features[0];
    asyncBlink();
    for(int i = 0; i < SAMPLE_NUM; i++){
      features[i] = featuresForFFT[i];
      if (features[i] > mx){
        mx = features[i];
      }
      if (features[i] < mn){
        mn = features[i];
      }
    }
    denom = mx-mn;
    for(int j=0; j < 128;j++){
      features[j] = (features[j] - mn)/denom;
    }
//    Serial.println("detected a word");
//printFeatures();

#ifndef TAKE_SAMPLES
  Serial.print("you said ");
  int output = clf.predict(features);
  switch(output){
    case 0:
      should_blink = true;
      break;
    case 1:
      should_blink = false;
      break;
  }
  Serial.println(output);
  delay(1000);
#else
  printFeatures();
#endif

  }
}

void asyncBlink(){
  if(should_blink){
//    Serial.println(interval_led);
//    Serial.println("should");
    if(millis()-interval_led > 500){
//      Serial.println("interval");
      led_state = !led_state;
      digitalWrite(LED_BUILTIN, led_state);
//      Serial.print(HIGH);
      interval_led = millis();
    }
  }
}

void printFeatures(){
  for (int i = 0; i < SAMPLE_NUM; i++){
    Serial.print(features[i]);
    Serial.print(i == SAMPLE_NUM - 1 ? "\n" : ",");
  }
}

void detectThreshold(){
  delay(5000);
  int sum = 0;
  for (int i =0; i < 200; i++){
    sum += readData();
  }

  Serial.print(sum/200);
}
void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println(readData() - THRESHOLD);
detectWord();
//detectThreshold();
asyncBlink();
}
