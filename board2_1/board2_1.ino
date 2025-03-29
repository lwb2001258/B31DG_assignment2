#define PWM_OUT_PIN 13   // 输出方波的引脚
#define PWM_IN_PIN  25   // 作为输入的引脚，检测方波

#define MIN_FREQ    500
#define MAX_FREQ    1000
#define STEP        1
#define CHANGE_INTERVAL 10  // 每隔10ms改变1Hz

// 用于方波生成
volatile bool state = false;
unsigned long lastToggleTime = 0;
unsigned long halfPeriodUs = 1000000 / (2 * MIN_FREQ);
int current_freq = MIN_FREQ;
bool increasing = true;

// 用于频率测量
unsigned long lastEdgeTime = 0;
unsigned long measured_freq = 0;
unsigned long lastMeasureTime = 0;

void setup() {
    pinMode(PWM_OUT_PIN, OUTPUT);
    pinMode(PWM_IN_PIN, INPUT);
    digitalWrite(PWM_OUT_PIN, LOW);
    Serial.begin(115200);
}

void loop() {
    unsigned long now = micros();

    // === 1. 产生50%占空比方波 ===
    if (now - lastToggleTime >= halfPeriodUs) {
        state = !state;
        digitalWrite(PWM_OUT_PIN, state);
        lastToggleTime = now;
    }

    // === 2. 频率在500 ~ 1000Hz之间来回变化 ===
    static unsigned long lastFreqUpdate = 0;
    if (millis() - lastFreqUpdate >= CHANGE_INTERVAL) {
        lastFreqUpdate = millis();

        if (increasing) {
            current_freq += STEP;
            if (current_freq >= MAX_FREQ) increasing = false;
        } else {
            current_freq -= STEP;
            if (current_freq <= MIN_FREQ) increasing = true;
        }

        // 重新计算半周期
        halfPeriodUs = 1000000 / (2 * current_freq);
    }

    // === 3. 在输入引脚上检测方波实际周期 ===
    static bool last_input_state = LOW;
    bool input_state = digitalRead(PWM_IN_PIN);

    // 检测上升沿
    if (input_state == HIGH && last_input_state == LOW) {
        unsigned long now_edge = micros();
        unsigned long period = now_edge - lastEdgeTime;
        lastEdgeTime = now_edge;
        if (period > 0) {
            measured_freq = 1000000UL / period;  // 计算测得的频率
        }
    }
    last_input_state = input_state;

    // === 4. 每500ms 打印一次 ===
    if (millis() - lastMeasureTime >= 10) {
        lastMeasureTime = millis();
        Serial.print("Target Frequency: ");
        Serial.print(current_freq);
        Serial.print(" Hz | Measured Frequency (on PIN ");
        Serial.print(PWM_IN_PIN);
        Serial.print("): ");
        Serial.print(measured_freq);
        Serial.println(" Hz");
    }
}
