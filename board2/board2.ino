#include "Arduino.h"

#define PWM_PIN 13
#define PWM_PIN2 14


const int outputPin1 = 13;  // First signal output pin
const int outputPin2 = 14;  // Second signal output pin

const int pwmChannel1 = 0;  
const int pwmChannel2 = 1;  
const int resolution = 8;   

void generateWave1(void *parameter);
void generateWave2(void *parameter);

void setup() {
    Serial.begin(115200);
    pinMode(outputPin1, OUTPUT);
    pinMode(outputPin2, OUTPUT);
    digitalWrite(outputPin1, LOW);
    digitalWrite(outputPin2, LOW);
    
     digitalWrite(PWM_PIN, LOW);
    // digitalWrite(PWM_PIN2, LOW);
      tone(PWM_PIN, 1000);



    // ledcSetup(pwmChannel1, 666, resolution);
    // ledcAttachPin(outputPin1, pwmChannel1);
    // ledcWriteTone(pwmChannel1, 666);

    // ledcSetup(pwmChannel2, 666, resolution);
    // ledcAttachPin(outputPin2, pwmChannel2);
    // ledcWriteTone(pwmChannel2, 666);

    // xTaskCreatePinnedToCore(generateWave1, "Wave1", 2048, NULL, 1, NULL, 0);
    // xTaskCreatePinnedToCore(generateWave2, "Wave2", 2048, NULL, 1, NULL, 1);
}

void loop() {
  digitalWrite(PWM_PIN, LOW);
    // digitalWrite(PWM_PIN2, LOW);
  tone(PWM_PIN, 1000);
    // tone(PWM_PIN2, 1500);
    
    // No need to do anything here, FreeRTOS tasks handle the signals
}

void generateWave1(void *parameter) {
    // int frequency = 666;
    while (1) {
      for (int frequency = 666;frequency<=1000; frequency = frequency+1){
        //  for (int frequency = 1;frequency<=2; frequency = frequency+1){
        uint32_t half_time_us = (int)round(1000000/(2*frequency));
        // Serial.printf("siginal1 halftime:");
        // Serial.println(half_time_us);
        digitalWrite(outputPin1, HIGH);
        delayMicroseconds(half_time_us);
        digitalWrite(outputPin1, LOW);
        delayMicroseconds(half_time_us);
        // ledcWriteTone(pwmChannel1, frequency);
        // Serial.printf("Wave1 Frequency: %d Hz\n", frequency);
        // frequency = (frequency == 666) ? 1000 : 666;
        // vTaskDelay(pdMS_TO_TICKS(1000));  // Change frequency every 1 second
      }
    }
}

void generateWave2(void *parameter) {
    // int frequency = 666;
    while (1) {
      for (int frequency = 833;frequency<=1500; frequency = frequency+1){
        // for (int frequency = 2;frequency<=3; frequency = frequency+1){
        uint32_t half_time_us2 = (int)round(1000000/(2*frequency));
        // Serial.printf("siginal2 halftime:");
        // Serial.println(half_time_us2);
        digitalWrite(outputPin2, HIGH);
        delayMicroseconds(half_time_us2);
        digitalWrite(outputPin2, LOW);
        delayMicroseconds(half_time_us2);
        // ledcWriteTone(pwmChannel2, frequency);
        // Serial.printf("Wave2 Frequency: %d Hz\n", frequency);
        // frequency = (frequency == 666) ? 1000 : 666;
        // vTaskDelay(pdMS_TO_TICKS(1000));  // Change frequency every 1 second
      }
    }
}
