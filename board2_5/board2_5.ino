#define PIN1 18   // 第一个方波输出
#define PIN2 19   // 第二个方波输出

// --- PIN1 参数 ---
unsigned int freq1 = 666;
bool increasing1 = true;
unsigned long lastToggle1 = 0;
bool state1 = LOW;

// --- PIN2 参数 ---
unsigned int freq2 = 833;
bool increasing2 = true;
unsigned long lastToggle2 = 0;
bool state2 = LOW;

void setup() {
    pinMode(PIN1, OUTPUT);
    pinMode(PIN2, OUTPUT);
}

// ========================
//      主循环控制方波
// ========================

void loop() {
    unsigned long now = micros();

    // ======== 方波1 ========
    unsigned long halfPeriod1 = 1000000UL / (2 * freq1); // 单位：us
    if (now - lastToggle1 >= halfPeriod1) {
        lastToggle1 = now;
        state1 = !state1;
        digitalWrite(PIN1, state1);
    }

    // 频率调节
    static unsigned long lastFreqUpdate1 = 0;
    if (millis() - lastFreqUpdate1 >= 10) { // 每10ms调节1Hz
        lastFreqUpdate1 = millis();
        if (increasing1) {
            freq1++;
            if (freq1 >= 1000) increasing1 = false;
        } else {
            freq1--;
            if (freq1 <= 666) increasing1 = true;
        }
    }

    // ======== 方波2 ========
    unsigned long halfPeriod2 = 1000000UL / (2 * freq2);
    if (now - lastToggle2 >= halfPeriod2) {
        lastToggle2 = now;
        state2 = !state2;
        digitalWrite(PIN2, state2);
    }

    // 频率调节
    static unsigned long lastFreqUpdate2 = 0;
    if (millis() - lastFreqUpdate2 >= 10) { // 每10ms调节1Hz
        lastFreqUpdate2 = millis();
        if (increasing2) {
            freq2++;
            if (freq2 >= 1500) increasing2 = false;
        } else {
            freq2--;
            if (freq2 <= 833) increasing2 = true;
        }
    }

    // ====== loop() 会自动不停执行，无需额外延时 ======
}
