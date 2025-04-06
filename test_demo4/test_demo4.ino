#include <Arduino.h>
#include <B31DGMonitor.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#define OUTPUT_PIN_1 26
#define OUTPUT_PIN_2 27
#define INPUT_PIN_F1 25
#define INPUT_PIN_F2 33
#define LED_PIN 17
#define BUTTON_PIN 16

B31DGCyclicExecutiveMonitor monitor;

TaskHandle_t taskHandles[5];
QueueHandle_t freqQueue;
SemaphoreHandle_t buttonSemaphore;

volatile uint64_t lastButtonInterruptTime = 0;
bool ledState = false;
bool last_F1_input_state = LOW;
bool last_F2_input_state = LOW;
unsigned long lastF1EdgeTime = 0;
unsigned long lastF2EdgeTime = 0;

// --- Interrupt ---
void IRAM_ATTR buttonISR() {
  unsigned long now = millis();
  if (now - lastButtonInterruptTime > 100) {
    lastButtonInterruptTime = now;
    xSemaphoreGiveFromISR(buttonSemaphore, NULL);
  }
}

// --- Task 1 ---
void task1(void *param) {
  const TickType_t xPeriod = pdMS_TO_TICKS(4);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    monitor.jobStarted(1);
    digitalWrite(OUTPUT_PIN_1, HIGH);
    delayMicroseconds(250);
    digitalWrite(OUTPUT_PIN_1, LOW);
    delayMicroseconds(50);
    digitalWrite(OUTPUT_PIN_1, HIGH);
    delayMicroseconds(300);
    digitalWrite(OUTPUT_PIN_1, LOW);
    monitor.jobEnded(1);
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}

// --- Task 2 ---
void task2(void *param) {
  const TickType_t xPeriod = pdMS_TO_TICKS(3);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    monitor.jobStarted(2);
    digitalWrite(OUTPUT_PIN_2, HIGH);
    delayMicroseconds(100);
    digitalWrite(OUTPUT_PIN_2, LOW);
    delayMicroseconds(50);
    digitalWrite(OUTPUT_PIN_2, HIGH);
    delayMicroseconds(200);
    digitalWrite(OUTPUT_PIN_2, LOW);
    monitor.jobEnded(2);
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}

// --- Task 3 ---
void task3(void *param) {
  const TickType_t xPeriod = pdMS_TO_TICKS(10);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    monitor.jobStarted(3);
    while (digitalRead(INPUT_PIN_F1) == last_F1_input_state);
    unsigned long now_edge = micros();
    if (lastF1EdgeTime > 0) {
      unsigned long period = now_edge - lastF1EdgeTime;
      if (period > 0) {
        int freq = 1000000 / period;
        xQueueSend(freqQueue, &freq, 0);
      }
    }
    lastF1EdgeTime = now_edge;
    last_F1_input_state = digitalRead(INPUT_PIN_F1);
    monitor.jobEnded(3);
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}

// --- Task 4 ---
void task4(void *param) {
  const TickType_t xPeriod = pdMS_TO_TICKS(10);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    monitor.jobStarted(4);
    while (digitalRead(INPUT_PIN_F2) == last_F2_input_state);
    unsigned long now_edge = micros();
    if (lastF2EdgeTime > 0) {
      unsigned long period = now_edge - lastF2EdgeTime;
      if (period > 0) {
        int freq = 1000000 / period;
        xQueueSend(freqQueue, &freq, 0);
      }
    }
    lastF2EdgeTime = now_edge;
    last_F2_input_state = digitalRead(INPUT_PIN_F2);
    monitor.jobEnded(4);
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}

// --- Task 5 ---
void task5(void *param) {
  const TickType_t xPeriod = pdMS_TO_TICKS(5);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    monitor.jobStarted(5);
    monitor.doWork();
    monitor.jobEnded(5);
    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}

// --- Button Handling Task ---
void buttonTask(void *param) {
  while (1) {
    if (xSemaphoreTake(buttonSemaphore, portMAX_DELAY) == pdTRUE) {
      monitor.doWork();
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
  }
}

// --- Frequency Monitor Task ---
void freqMonitorTask(void *param) {
  int f1 = 0, f2 = 0;
  while (1) {
    int freq;
    if (xQueueReceive(freqQueue, &freq, pdMS_TO_TICKS(10))) {
      if (freq >= 666 && freq <= 1000) {
        f1 = freq;
      } else if (freq >= 833 && freq <= 1500) {
        f2 = freq;
      }
      if (f1 + f2 > 1500) {
        digitalWrite(LED_PIN, HIGH);
        ledState = true;
      } else {
        digitalWrite(LED_PIN, LOW);
        ledState = false;
      }
    }
  }
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  pinMode(OUTPUT_PIN_1, OUTPUT);
  pinMode(OUTPUT_PIN_2, OUTPUT);
  pinMode(INPUT_PIN_F1, INPUT);
  pinMode(INPUT_PIN_F2, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, RISING);

  freqQueue = xQueueCreate(10, sizeof(int));
  buttonSemaphore = xSemaphoreCreateBinary();

  monitor.startMonitoring();

  xTaskCreatePinnedToCore(task1, "Task1", 2048, NULL, 1, &taskHandles[0], 0);
  xTaskCreatePinnedToCore(task2, "Task2", 2048, NULL, 1, &taskHandles[1], 0);
  xTaskCreatePinnedToCore(task3, "Task3", 2048, NULL, 1, &taskHandles[2], 1);
  xTaskCreatePinnedToCore(task4, "Task4", 2048, NULL, 1, &taskHandles[3], 1);
  xTaskCreatePinnedToCore(task5, "Task5", 2048, NULL, 1, &taskHandles[4], 0);
  // xTaskCreatePinnedToCore(buttonTask, "Button", 2048, NULL, 1, NULL, 1);
  // xTaskCreatePinnedToCore(freqMonitorTask, "FreqMon", 2048, NULL, 1, NULL, 1);
}

void loop() {
  // Not used in FreeRTOS mode
}
