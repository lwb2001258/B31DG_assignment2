#include <B31DGMonitor.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "esp_timer.h"
#include "esp_task_wdt.h"
#define OUTPUT_PIN_1 26
#define OUTPUT_PIN_2 27
#define INPUT_PIN_F1 25
#define INPUT_PIN_F2 33
#define LED_PIN 17
#define BUTTON_PIN 16
#define DEBOUNCE_DELAY 100
B31DGCyclicExecutiveMonitor monitor;
#define OUTPUT_PIN_1 26
#define OUTPUT_PIN_2 27
#define INPUT_PIN_F1 25
#define INPUT_PIN_F2 33
#define LED_PIN 17
#define BUTTON_PIN 16
#define BUTTON_LED_PIN 32
#define DEBOUNCE_DELAY 500

unsigned long F1, F2, F, lastF1EdgeTime, lastF2EdgeTime;
volatile uint64_t lastButtonInterruptTime = 0;
static bool last_F1_input_state = LOW;
static bool last_F2_input_state = LOW;
unsigned long startTimeTask3, periodTask3, endTimeTask3, startTimeTask4, periodTask4, endTimeTask4 = 0;
volatile bool ButtonLedState = false;
SemaphoreHandle_t mutex;
TaskHandle_t jobHandles[5];
volatile int jobIndex = 10;
volatile bool jobDone = true;



int slackTimes[5] = { 3400, 2650, 7200, 7800, 4500 };
int jobCounts[5] = { 0, 0, 0, 0, 0 };
int executeTimes[5] = { 600, 350, 2800, 2200, 500 };
int cycleList[5] = { 4000, 3000, 10000, 10000, 5000 };
bool doneList[5] = { false, false, false, false, false };
int count = 0;
void frame() {
  count += 1;
  unsigned long now = micros();
  unsigned long totalTime = now - monitor.getTimeStart();
  for (int i = 0; i < 5; i++) {
    if ((int)(totalTime / cycleList[i]) + 1 > jobCounts[i]) {
      doneList[i] = false;
      slackTimes[i] = cycleList[i] - totalTime % cycleList[i];
    }
  }
  digitalWrite(BUTTON_LED_PIN, ButtonLedState ? HIGH : LOW);
}

void IRAM_ATTR buttonPressedHandle() {
  unsigned long currentTime = millis();
  if (currentTime - lastButtonInterruptTime > DEBOUNCE_DELAY) {
    ButtonLedState = !ButtonLedState;
    monitor.doWork();
    //update the lastEnableInterruptTime value
    lastButtonInterruptTime = currentTime;
  }
}

void frameTask(void *pvParameters) {
  const TickType_t interval = pdMS_TO_TICKS(1);  // 1ms
  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true) {
    frame();
    vTaskDelayUntil(&lastWakeTime, interval);
  }
}
// volatile int ex_num=0;

void schedulerTask(void *param) {
  // put your main code here, to run repeatedly:
  while (1) {
    //  Serial.println(1);
    if (ButtonLedState) {
      digitalWrite(BUTTON_LED_PIN, HIGH);
    } else {
      digitalWrite(BUTTON_LED_PIN, LOW);
    }
    int jobIndex = 10;


    for (int i = 0; i < 5; i++) {
      if (!doneList[i]) {
        if (slackTimes[i] < slackTimes[jobIndex]) {
          jobIndex = i;
        }
      }
    }
    // Serial.println(jobIndex);
    switch (jobIndex) {
      case 0: JobTask1(); break;
      case 1: JobTask2(); break;
      case 2: JobTask3(); break;
      case 3: JobTask4(); break;
      case 4: JobTask5(); break;
      default: break;
    }
  }
}

// void schedulerTask(void *param) {
//   while (1) {
//     // ex_num++;
//     // Serial.printf("st:");
//     // Serial.println(ex_num);
//     // Serial.println("schedulerTask..");
//     // ButtonLedState = !ButtonLedState;

//     int currentMinDeadline = INT32_MAX;
//     xSemaphoreTake(mutex, pdMS_TO_TICKS(10));
//     // Serial.println("get Mutex..");
//     if (!jobDone){

//       xSemaphoreGive(mutex);
//       taskYIELD();
//        continue;
//     }
//     for (int i = 0; i < 5; i++) {
//       if (!doneList[i] && slackTimes[i] < currentMinDeadline) {
//         jobIndex = i;
//         currentMinDeadline = slackTimes[i];
//       }
//     }
//     jobDone = false;
//     xSemaphoreGive(mutex);


//   }
// }

// 每个 Job 的逻辑
void JobTask(void *param) {
  while (1) {
    // ex_num++;
    // Serial.printf("jt:");
    // Serial.println(ex_num);
    xSemaphoreTake(mutex, pdMS_TO_TICKS(10));
    if (jobDone) {
      xSemaphoreGive(mutex);
      // taskYIELD();
      continue;
    }
    switch (jobIndex) {
      case 0: JobTask1(); break;
      case 1: JobTask2(); break;
      case 2: JobTask3(); break;
      case 3: JobTask4(); break;
      case 4: JobTask5(); break;
      default: break;
    }
    // Serial.println("abc");
    jobDone = true;
    doneList[jobIndex] = true;
    jobCounts[jobIndex]++;
    xSemaphoreGive(mutex);
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(OUTPUT_PIN_1, OUTPUT);
  pinMode(OUTPUT_PIN_2, OUTPUT);
  pinMode(INPUT_PIN_F1, INPUT);
  pinMode(INPUT_PIN_F2, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUTTON_LED_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  // mutex = xSemaphoreCreateMutex();
  mutex = xSemaphoreCreateBinary();
  monitor.startMonitoring();
  xTaskCreatePinnedToCore(frameTask, "FrameTask", 2048, NULL, 1, NULL, 0);
  // xTaskCreatePinnedToCore(JobTask, "JobTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(schedulerTask, "schedulerTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
  delay(1);

  //   // put your main code here, to run repeatedly:
  // if (ButtonLedState){
  //   digitalWrite(BUTTON_LED_PIN, HIGH);
  // }else{
  //   digitalWrite(BUTTON_LED_PIN, LOW);
  // }

  // int jobIndex = 10;


  // for (int i = 0; i < 5; i++) {
  //   if (!doneList[i]) {
  //     if (slackTimes[i] < slackTimes[jobIndex]) {
  //       jobIndex = i;
  //     }
  //   }
  // }
  // // Serial.println(jobIndex);
  // switch (jobIndex) {
  //   case 0: JobTask1(); break;
  //   case 1: JobTask2(); break;
  //   case 2: JobTask3(); break;
  //   case 3: JobTask4(); break;
  //   case 4: JobTask5(); break;
  //   default: break;
  // }
}

void JobTask1(void) {
  monitor.jobStarted(1);
  digitalWrite(OUTPUT_PIN_1, HIGH);
  delayMicroseconds(250);  // HIGH for 250μs
  digitalWrite(OUTPUT_PIN_1, LOW);
  delayMicroseconds(50);  // LOW for 50μs
  digitalWrite(OUTPUT_PIN_1, HIGH);
  delayMicroseconds(300);  // HIGH for 300μs
  digitalWrite(OUTPUT_PIN_1, LOW);
  // doneList[0] = true;
  // jobCounts[0] = jobCounts[0] + 1;
  monitor.jobEnded(1);
}

// Task 2, takes 1.8ms
void JobTask2(void) {
  monitor.jobStarted(2);
  digitalWrite(OUTPUT_PIN_2, HIGH);
  delayMicroseconds(100);
  digitalWrite(OUTPUT_PIN_2, LOW);
  delayMicroseconds(50);
  digitalWrite(OUTPUT_PIN_2, HIGH);
  delayMicroseconds(200);
  digitalWrite(OUTPUT_PIN_2, LOW);
  // doneList[1] = true;
  // jobCounts[1] = jobCounts[1] + 1;
  monitor.jobEnded(2);
}


// Task 3, takes 1ms
void JobTask3(void) {
  monitor.jobStarted(3);
  int count = 0;
  // unsigned long start = micros();
  while (1) {
    // taskENTER_CRITICAL(&myMux);
    bool input_state = digitalRead(INPUT_PIN_F1);
    if (last_F1_input_state == LOW && input_state == LOW) {
      last_F1_input_state = input_state;
      continue;
    } else if (last_F1_input_state == HIGH && input_state == HIGH) {
      last_F1_input_state = input_state;
      continue;
    } else if (last_F1_input_state == HIGH && input_state == LOW) {
      last_F1_input_state = input_state;
      continue;
    } else if (input_state == HIGH && last_F1_input_state == LOW) {
      count = count + 1;
      last_F1_input_state = input_state;
      unsigned long now_edge = esp_timer_get_time();
      ;
      unsigned long period = now_edge - lastF1EdgeTime;
      lastF1EdgeTime = now_edge;
      if (period > 0 && count > 1) {
        F1 = 1000000UL / period;
        F = F1 + F2;
        if (F > 1600) {
          digitalWrite(LED_PIN, HIGH);
        } else {
          digitalWrite(LED_PIN, LOW);
        }
        // taskEXIT_CRITICAL(&myMux);
        break;
      }
    }
  }
  // if (jobCounts[2]%100==0){
  //   Serial.println(F2);
  // }
  // doneList[2] = true;
  // jobCounts[2] = jobCounts[2] + 1;
  monitor.jobEnded(3);
}

// Task 4, takes about 600-2200us
void JobTask4(void) {
  monitor.jobStarted(4);
  // unsigned long start = micros();
  int count = 0;
  while (1) {
    // taskENTER_CRITICAL(&myMux);
    bool input_state = digitalRead(INPUT_PIN_F2);
    if (last_F2_input_state == LOW && input_state == LOW) {
      last_F2_input_state = input_state;
      continue;
    } else if (last_F2_input_state == HIGH && input_state == HIGH) {
      last_F2_input_state = input_state;
      continue;
    } else if (last_F2_input_state == HIGH && input_state == LOW) {
      last_F2_input_state = input_state;
      continue;
    } else if (input_state == HIGH && last_F2_input_state == LOW) {
      count = count + 1;
      last_F2_input_state = input_state;
      unsigned long now_edge = esp_timer_get_time();
      unsigned long period = now_edge - lastF2EdgeTime;
      lastF2EdgeTime = now_edge;
      if (period > 0 && count > 1) {
        F2 = 1000000UL / period;  // 计算测得的频率
        F = F2 + F2;
        if (F > 1500) {
          digitalWrite(LED_PIN, HIGH);
        } else {
          digitalWrite(LED_PIN, LOW);
        }
        break;
      }
    }
  }
  // doneList[3] = true;
  // jobCounts[3] = jobCounts[3] + 1;
  monitor.jobEnded(4);
}


// Task 5, takes 0.5ms
void JobTask5(void) {
  monitor.jobStarted(5);
  monitor.doWork();
  // doneList[4] = true;
  // jobCounts[4] = jobCounts[4] + 1;
  monitor.jobEnded(5);
}
