#include "Arduino.h"

#define PWM_PIN 13        // 输出 PWM 方波的 GPIO 引脚
#define MIN_FREQ 500      // 最小频率 (Hz)
#define MAX_FREQ 1000     // 最大频率 (Hz)
#define STEP 1            // 频率步进值 (Hz)
#define UPDATE_MS 10      // 频率更新间隔 (ms)
#define INPUT_PIN 25

volatile int current_freq = MIN_FREQ; // 当前频率
volatile bool increasing = true; // 控制频率上升/下降
volatile bool state = false; // 方波状态 (HIGH/LOW)
unsigned long lastToggleTime = 0; // 上次翻转时间
unsigned long lastUpdateTime = 0; // 上次更新频率时间
unsigned long halfPeriod = 1000000 / (2 * current_freq); // 当前频率的半周期 (µs)
unsigned long startTimeTask3, endTimeTask3, periodTask3,startTimeTask4, endTimeTask4, periodTask4;
int F1,F2,F;

void task1(void *pvParameters){
  while(1){
   unsigned long now = micros(); // 获取当前时间 (µs)

    // 计算 PWM 切换 (T/2)
    if (now - lastToggleTime >= halfPeriod) {
        state = !state; // 翻转方波状态
        digitalWrite(PWM_PIN, state); // 输出到 GPIO
        lastToggleTime = now; // 记录翻转时间
    }

    // 频率调整 (每 10ms 变化 1Hz)
    if (millis() - lastUpdateTime >= UPDATE_MS) {
        lastUpdateTime = millis(); // 记录更新时间

        // 频率增减逻辑
        if (increasing) {
            current_freq += STEP;
            if (current_freq >= MAX_FREQ) increasing = false;
        } else {
            current_freq -= STEP;
            if (current_freq <= MIN_FREQ) increasing = true;
        }

        halfPeriod = 1000000 / (2 * current_freq); // 更新半周期
    }
  }
}

void task2(void *pvParameters){
  while(1){
  // monitor.jobStarted(3);
      // Wait for a rising edge (LOW → HIGH transition)
    while (digitalRead(INPUT_PIN) == LOW);  
    startTimeTask3 = micros();  // Record the time of the first rising edge

    // Wait for the next rising edge
    while (digitalRead(INPUT_PIN) == HIGH);  // Ensure we exit the previous HIGH state
    while (digitalRead(INPUT_PIN) == LOW);  
    endTimeTask3 = micros();  // Record the time of the second rising edge

    // Calculate the period (time difference between two rising edges)
    periodTask3 = endTimeTask3 - startTimeTask3;

    // Calculate frequency: f = 1 / period (convert microseconds to seconds)
    F1 = (int)round(1000000.0 / periodTask3);  // Frequency in Hz
   
    Serial.printf("F1: ");
    Serial.println(F1);
    delay(1000);
    // monitor.jobEnded(3);
  }

}


void setup() {
    pinMode(PWM_PIN, OUTPUT);
    digitalWrite(PWM_PIN, LOW);
     Serial.begin(115200);
    pinMode(INPUT_PIN, INPUT);
    xTaskCreatePinnedToCore(task1, "Wave1", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(task2, "Wave2", 2048, NULL, 1, NULL, 1);
}

void loop() {
  
    // unsigned long now = micros(); // 获取当前时间 (µs)

    // // 计算 PWM 切换 (T/2)
    // if (now - lastToggleTime >= halfPeriod) {
    //     state = !state; // 翻转方波状态
    //     digitalWrite(PWM_PIN, state); // 输出到 GPIO
    //     lastToggleTime = now; // 记录翻转时间
    // }

    // // 频率调整 (每 10ms 变化 1Hz)
    // if (millis() - lastUpdateTime >= UPDATE_MS) {
    //     lastUpdateTime = millis(); // 记录更新时间

    //     // 频率增减逻辑
    //     if (increasing) {
    //         current_freq += STEP;
    //         if (current_freq >= MAX_FREQ) increasing = false;
    //     } else {
    //         current_freq -= STEP;
    //         if (current_freq <= MIN_FREQ) increasing = true;
    //     }

    //     halfPeriod = 1000000 / (2 * current_freq); // 更新半周期
    // }
}
