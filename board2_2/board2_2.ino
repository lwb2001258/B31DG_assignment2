#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"


#define PWM_OUT_PIN1 13   // 输出方波的引脚
// #define PWM_IN_PIN  25   // 作为输入的引脚，检测方波
#define PWM_OUT_PIN2 27   // 输出方波的引脚


#define MIN_FREQ1    666
#define MAX_FREQ1    1000
#define STEP1        1
#define MIN_FREQ2    833
#define MAX_FREQ2    1500
#define STEP2        1
#define CHANGE_INTERVAL 10  // 每隔10ms改变1Hz

// 用于方波生成
volatile bool state1,state2 = false;
unsigned long lastToggleTime1,lastToggleTime2 = 0;
unsigned long halfPeriodUs1 = 1000000 / (2 * MIN_FREQ1);
unsigned long halfPeriodUs2 = 1000000 / (2 * MIN_FREQ2);
int current_freq1 = MIN_FREQ1;
int current_freq2 = MIN_FREQ2;
bool increasing1,increasing2 = true;
static unsigned long lastFreqUpdate1, lastFreqUpdate2 = 0;




void task1(void* params){
  while (1){
   unsigned long now = micros();


    if (now - lastToggleTime1 >= halfPeriodUs1) {
        state1 = !state1;
        digitalWrite(PWM_OUT_PIN1, state1);
        lastToggleTime1 = now;
    }


    if (millis() - lastFreqUpdate1 >= CHANGE_INTERVAL) {
        lastFreqUpdate1 = millis();

        if (increasing1) {
            current_freq1 += STEP1;
            if (current_freq1 >= MAX_FREQ1) increasing1 = false;
        } else {
            current_freq1 -= STEP1;
            if (current_freq1 <= MIN_FREQ1) increasing1 = true;
        }
        halfPeriodUs1 = 1000000 / (2 * current_freq1);
    }else{
      taskYIELD();
    }
    
  }
   
}


// void task2(void* params){
  void task2(){
  while (1){
  //  Serial.println("start....");
   unsigned long now = micros();

    // === 1. 产生50%占空比方波 ===
    if (now - lastToggleTime2 >= halfPeriodUs2) {
        state2 = !state2;
        digitalWrite(PWM_OUT_PIN2, state2);
        lastToggleTime2 = now;
    }


    if (millis() - lastFreqUpdate2 >= CHANGE_INTERVAL) {
        lastFreqUpdate2 = millis();

        if (increasing2) {
            current_freq2 += STEP2;
            if (current_freq2 >= MAX_FREQ2) increasing2 = false;
        } else {
            current_freq2 -= STEP2;
            if (current_freq2 <= MIN_FREQ2) increasing2 = true;
        }

        // 重新计算半周期
        halfPeriodUs2 = 1000000 / (2 * current_freq2);
        Serial.println(halfPeriodUs2);
        // vTaskDelay(pdMs_TO_TICKS(1)); 
        // vTaskDelay(pdMS_TO_TICKS(1)); 
        // esp_task_wdt_reset();
        // yield(); 
        
    }
  }
}




void setup() {
    pinMode(PWM_OUT_PIN1, OUTPUT);
    digitalWrite(PWM_OUT_PIN1, LOW);
     pinMode(PWM_OUT_PIN2, OUTPUT);
   
    digitalWrite(PWM_OUT_PIN2, LOW);
    Serial.begin(115200);
    
    // xTaskCreatePinnedToCore(task2, "task2", 2048, NULL, 1, NULL, 0);
    // xTaskCreatePinnedToCore(task1, "task1", 2048, NULL, 1, NULL, 0);

}





void loop() {
  task2();
    // unsigned long now = micros();

    // // === 1. 产生50%占空比方波 ===
    // if (now - lastToggleTime >= halfPeriodUs) {
    //     state = !state;
    //     digitalWrite(PWM_OUT_PIN, state);
    //     lastToggleTime = now;
    // }

    // // === 2. 频率在500 ~ 1000Hz之间来回变化 ===
    // static unsigned long lastFreqUpdate = 0;
    // if (millis() - lastFreqUpdate >= CHANGE_INTERVAL) {
    //     lastFreqUpdate = millis();

    //     if (increasing) {
    //         current_freq += STEP;
    //         if (current_freq >= MAX_FREQ) increasing = false;
    //     } else {
    //         current_freq -= STEP;
    //         if (current_freq <= MIN_FREQ) increasing = true;
    //     }

    //     // 重新计算半周期
    //     halfPeriodUs = 1000000 / (2 * current_freq);
    // }

    // // === 3. 在输入引脚上检测方波实际周期 ===
    // static bool last_input_state = LOW;
    // bool input_state = digitalRead(PWM_IN_PIN);

    // // 检测上升沿
    // if (input_state == HIGH && last_input_state == LOW) {
    //     unsigned long now_edge = micros();
    //     unsigned long period = now_edge - lastEdgeTime;
    //     lastEdgeTime = now_edge;
    //     if (period > 0) {
    //         measured_freq = 1000000UL / period;  // 计算测得的频率
    //     }
    // }
    // last_input_state = input_state;

    // // === 4. 每500ms 打印一次 ===
    // if (millis() - lastMeasureTime >= 500) {
    //     lastMeasureTime = millis();
    //     Serial.print("Target Frequency: ");
    //     Serial.print(current_freq);
    //     Serial.print(" Hz | Measured Frequency (on PIN ");
    //     Serial.print(PWM_IN_PIN);
    //     Serial.print("): ");
    //     Serial.print(measured_freq);
    //     Serial.println(" Hz");
    // }
}
