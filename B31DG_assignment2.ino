#include <B31DGMonitor.h>
#include <Ticker.h>
#include <Arduino.h>

B31DGCyclicExecutiveMonitor monitor;


#define OUTPUT_PIN_1 26
#define OUTPUT_PIN_2 27
#define INPUT_PIN_F1  25
#define INPUT_PIN_F2  33 
#define LED_PIN 17 
#define BUTTON_PIN 16
#define DEBOUNCE_DELAY 100  

unsigned long startTimeTask3, endTimeTask3, periodTask3,startTimeTask4, endTimeTask4, periodTask4;
unsigned long F1,F2,F;
boolean ledState;
volatile uint64_t lastButtonInterruptTime = 0;
Ticker ticker1, ticker2, ticker3, ticker4, ticker5;
// --- Cyclic Executive ---
Ticker majorCycle;
// 用于频率测量
unsigned long lastF1EdgeTime = 0;
unsigned long lastF2EdgeTime = 0;
// unsigned long measured_freq = 0;
// unsigned long lastF1MeasureTime = 0;
// int current_freq = 500;


void task1(){
    monitor.jobStarted(1);
    digitalWrite(OUTPUT_PIN_1, HIGH);  
    delayMicroseconds(250);  // HIGH for 250μs

    digitalWrite(OUTPUT_PIN_1, LOW);   
    delayMicroseconds(50);   // LOW for 50μs

    digitalWrite(OUTPUT_PIN_1, HIGH);  
    delayMicroseconds(300);  // HIGH for 300μs

    digitalWrite(OUTPUT_PIN_1, LOW); 
    // Serial.println(12345);
    monitor.jobEnded(1);   
}

void task2(){
    monitor.jobStarted(2);
    digitalWrite(OUTPUT_PIN_2, HIGH);  
    delayMicroseconds(100);  
    digitalWrite(OUTPUT_PIN_2, LOW);   
    delayMicroseconds(50); 
    digitalWrite(OUTPUT_PIN_2, HIGH);  
    delayMicroseconds(200); 
    digitalWrite(OUTPUT_PIN_2, LOW); 
    // Serial.println(123456);
    monitor.jobEnded(2);
   }
// void task3(){
//   monitor.jobStarted(3);
//       // Wait for a rising edge (LOW → HIGH transition)
//     while (digitalRead(INPUT_PIN_F1) == LOW);  
//     startTimeTask3 = micros();  // Record the time of the first rising edge

//     // Wait for the next rising edge
//     while (digitalRead(INPUT_PIN_F1) == HIGH);  // Ensure we exit the previous HIGH state
//     while (digitalRead(INPUT_PIN_F1) == LOW);  
//     endTimeTask3 = micros();  // Record the time of the second rising edge

//     // Calculate the period (time difference between two rising edges)
//     periodTask3 = endTimeTask3 - startTimeTask3;

//     // Calculate frequency: f = 1 / period (convert microseconds to seconds)
//     F1 = (int)round(1000000.0 / periodTask3);  // Frequency in Hz
//     F = F1+F2;
//     if (F>1600){
//         digitalWrite(LED_PIN, HIGH);
//         ledState = true;
//     }else{
//       digitalWrite(LED_PIN, LOW);
//       ledState = false;
//     }
//     Serial.printf("F1: ");
//     Serial.println(F1);
//     monitor.jobEnded(3);

// }

void task3(){  
    monitor.jobStarted(3);
    static bool last_F1_input_state = LOW;
    int count = 0;
    while (1){
      bool input_state = digitalRead(INPUT_PIN_F1);
      if (last_F1_input_state==LOW && input_state==LOW){
        last_F1_input_state = input_state;
        continue;
      }
      else if (last_F1_input_state == HIGH && input_state == HIGH){
        last_F1_input_state = input_state;
        continue;
      }
      else if (last_F1_input_state == HIGH && input_state == LOW){
        last_F1_input_state = input_state;
        continue;
      }    
      else if (input_state == HIGH && last_F1_input_state == LOW) {
        count= count+1;
        last_F1_input_state = input_state;
        unsigned long now_edge = micros();
        unsigned long period = now_edge - lastF1EdgeTime;
        lastF1EdgeTime = now_edge;       
        if (period > 0 && count>1) {
          F1 = 1000000UL / period;  // 计算测得的频率
          Serial.println(F1);
          F = F1+F2;
          if (F>1600){
              digitalWrite(LED_PIN, HIGH);
              ledState = true;
          }else{
            digitalWrite(LED_PIN, LOW);
            ledState = false;
          }
          break;                    
        }       
    }    
  } 
  monitor.jobEnded(3);
}

void task4(){
    monitor.jobStarted(4);
    static bool last_F2_input_state = LOW;
    int count = 0;
    while (1){
      bool input_state = digitalRead(INPUT_PIN_F2);
      if (last_F2_input_state==LOW && input_state==LOW){
        last_F2_input_state = input_state;
        continue;
      }
      else if (last_F2_input_state == HIGH && input_state == HIGH){
        last_F2_input_state = input_state;
        continue;
      }
      else if (last_F2_input_state == HIGH && input_state == LOW){
        last_F2_input_state = input_state;
        continue;
      }    
      else if (input_state == HIGH && last_F2_input_state == LOW) {
        count= count+1;
        last_F2_input_state = input_state;
        unsigned long now_edge = micros();
        unsigned long period = now_edge - lastF2EdgeTime;
        lastF2EdgeTime = now_edge;       
        if (period > 0 && count>1) {
          F2 = 1000000UL / period;  // 计算测得的频率
          Serial.printf("F2:");
          Serial.println(F2);
          F = F2+F2;
          if (F>1600){
              digitalWrite(LED_PIN, HIGH);
              ledState = true;
          }else{
            digitalWrite(LED_PIN, LOW);
            ledState = false;
          }
          break;                    
        }       
    }    
  } 
  monitor.jobEnded(4);
}

// void task4(){
//   monitor.jobStarted(4);
//     // Wait for a rising edge (LOW → HIGH transition)
//     while (digitalRead(INPUT_PIN_F2) == LOW);  
//     startTimeTask4 = micros();  // Record the time of the first rising edge
//     while (digitalRead(INPUT_PIN_F2) == HIGH);  // Ensure we exit the previous HIGH state
//     while (digitalRead(INPUT_PIN_F2) == LOW);  
//     endTimeTask4 = micros();  // Record the time of the second rising edge
//     periodTask4 = endTimeTask4 - startTimeTask4;

//     // Calculate frequency: f = 1 / period (convert microseconds to seconds)
//     F2 = (int)round(1000000.0 / periodTask4);  // Frequency in Hz
//     F = F1+F2;
//     if (F>1600){
//         digitalWrite(LED_PIN, HIGH);
//         ledState = true;
//     }else{
//       digitalWrite(LED_PIN, LOW);
//       ledState = false;
//     }
//     Serial.printf("F2: ");
//     Serial.println(F2);
//     monitor.jobEnded(4);
// }

void task5(){
  monitor.jobStarted(5);
  monitor.doWork();
  monitor.jobEnded(5);

}

void IRAM_ATTR buttonPressedHandle() {
  unsigned long currentTime = millis();
  if (currentTime - lastButtonInterruptTime > DEBOUNCE_DELAY) {
    // toggle the enable state when the enable button is pressed
    if (ledState){
      digitalWrite(LED_PIN, LOW);
    }else{
      digitalWrite(LED_PIN, HIGH);
    }
    ledState = !ledState;
    //update the lastEnableInterruptTime value
    lastButtonInterruptTime = currentTime;
  }
}

void majorCycleISR(){
  task1();
  task2();
  task3();
  task4();
  task5();
}

void setup(void)
{
  Serial.begin(115200); // Starts the serial communication and sets baud rate to 9600
  pinMode(OUTPUT_PIN_1, OUTPUT);
  pinMode(OUTPUT_PIN_2, OUTPUT);
  pinMode(INPUT_PIN_F1, INPUT);
  pinMode(INPUT_PIN_F2, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressedHandle, RISING);
   delay(500);
    monitor.startMonitoring();
    // majorCycle.attach_ms(1, majorCycleISR);
  
  // monitor.startMonitoring(); // all the tasks should be released after this, this time can be queried using getTimeStart()
  // ticker1.attach_us(4000, task1);
  // ticker1.attach_us(3000, task2);
  // ticker1.attach_us(10000, task3);
  // ticker1.attach_us(10000, task4);
  // ticker1.attach_us(5000, task5);


}

// void taskA() {
//    monitor.jobStarted(i);  // i between [1..5], i.e. 1 is digital output, 5 is serial print
//    // ... perform taskA
//   //  Serial.println("i");
//    monitor.jobEnded(i);
// }

// void taskB() {
//    monitor.jobStarted(i);
//    // ... perform taskB
//   //  Serial.println("i");
//    monitor.jobEnded(i);
// }

// simple example of cyclic executive (version not using Ticker)
void loop(void) 
{
  //  task1();
  //  task2();
   task3();
   task4();
  //  task5();
   delay(1000);
}
