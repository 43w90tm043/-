#include <GyverOLED.h>
#include <GyverDS3231.h>
#include <GyverBME280.h>
#include <GyverHTU21D.h>
#include <GTimer.h>
#include <Wire.h>
#include "paj7620.h" // Make sure you have this library header file
#include <math.h>    // Include for isnan() check

// --- ИЗМЕНЕНИЕ: Используем режим буферизации для устранения мерцания ---
// GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled; // Старый вариант без буфера
GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;    // Новый вариант с буфером

GyverDS3231 rtc;
GyverBME280 bme;
GyverHTU21D htu;
GTimer<millis> tmrDisplay; // Timer for continuous display updates

// --- New variables ---
// Define screen states
#define SCREEN_HOME 0
#define SCREEN_HUMIDITY 1
#define SCREEN_PRESSURE 2

int currentScreen = SCREEN_HOME; // Variable to track the current screen
const int totalScreens = 3;      // Total number of screens to cycle through
const int DISPLAY_UPDATE_INTERVAL_MS = 1000; // Update display every 1 second (1000 ms)

// Function prototypes for display screens
void displayHome();
void displayHumidity();
void displayPressure();
void updateDisplay(); // Function to call the correct display function

// --- Setup Function ---
void setup() {
    Serial.begin(9600);
    Serial.println("Starting setup...");
    Wire.begin();

    Serial.print("Initializing PAJ7620... ");
    if (paj7620Init() == 0) {
        Serial.println("PAJ7620 Init OK");
    } else {
        Serial.println("PAJ7620 Init Failed!");
    }

    Serial.print("Initializing BME280... ");
    if (bme.begin()) {
        Serial.println("BME280 OK");
    } else {
        Serial.println("BME280 Failed!");
    }

    Serial.print("Initializing HTU21D... ");
    if (htu.begin()) {
        Serial.println("HTU21D OK");
    } else {
        Serial.println("HTU21D Failed!");
    }

    Serial.print("Initializing DS3231 RTC... ");
    rtc.begin();
     rtc.setBuildTime(); // Раскомментировать ОДИН РАЗ для установки времени компиляции
    Serial.println("RTC OK");

    Serial.print("Initializing OLED... ");
    oled.init();
    // В режиме буферизации clear() очищает буфер в памяти, а не экран
    oled.clear();
    // update() отправит пустой буфер на экран при первой инициализации
    oled.update();
    Serial.println("OLED OK");

    tmrDisplay.setMode(GTMode::Interval);
    tmrDisplay.setTime(DISPLAY_UPDATE_INTERVAL_MS);
    tmrDisplay.start();

    Serial.println("Setup complete. Displaying Home screen.");
    updateDisplay(); // Display the initial screen (Home)
 
    delay(500);
}

// --- Main Loop ---
void loop() {
    // --- Handle Gesture Input ---
    uint8_t data = 0, data1 = 0, error;
    error = paj7620ReadReg(0x43, 1, &data);

    if (!error && data != 0)
    {
        switch (data) {
            case GES_RIGHT_FLAG: Serial.println("Right"); break;
            case GES_LEFT_FLAG:  Serial.println("Left"); break;
            case GES_UP_FLAG:
                Serial.println("Up");
                currentScreen = (currentScreen + 1) % totalScreens;
                // Не обновляем дисплей здесь, таймер сделает это
                break;
            case GES_DOWN_FLAG:
                Serial.println("Down");
                currentScreen = SCREEN_HOME;
                 // Не обновляем дисплей здесь, таймер сделает это
                break;
            case GES_FORWARD_FLAG: Serial.println("Forward"); break;
            case GES_BACKWARD_FLAG: Serial.println("Backward"); break;
            case GES_CLOCKWISE_FLAG: Serial.println("Clockwise"); break;
            case GES_COUNT_CLOCKWISE_FLAG: Serial.println("Anti-clockwise"); break;
            default:
                error = paj7620ReadReg(0x44, 1, &data1);
                if (!error && data1 == GES_WAVE_FLAG) {
                    Serial.println("Wave");
                }
                break;
        }
        // Возможно, потребуется очистка флагов жестов здесь
        delay(50); // Небольшая задержка после обработки жеста
    }

    // --- Handle Continuous Display Update ---
    if (tmrDisplay.tick()) { // Проверяем, пора ли обновлять дисплей
        updateDisplay();     // Обновляем экран текущими данными
    }
}

// --- Display Update Function ---
// Теперь эта функция рисует в буфер памяти
void updateDisplay() {
    // 1. Очистить БУФЕР в памяти микроконтроллера
    oled.clear();

    // 2. Нарисовать нужный экран В БУФЕР
    switch (currentScreen) {
        case SCREEN_HOME:     displayHome();     break;
        case SCREEN_HUMIDITY: displayHumidity(); break;
        case SCREEN_PRESSURE: displayPressure(); break;
    }

    // 3. Отправить готовый кадр из буфера на дисплей ОДНОЙ операцией
    oled.update();
}

// --- Screen Display Functions ---
// Эти функции теперь рисуют в буфер памяти

// Display Humidity Screen
void displayHumidity() {
    oled.setCursor(5, 3);
    oled.setScale(2);
    oled.print("Hum:");
    float humidity = htu.getHumidityWait();
    if (isnan(humidity)) {
        oled.print(" ERR");
    } else {
        oled.print(humidity, 1);
        oled.print("%");
    }
    oled.setScale(1);
}

// Display Home Screen (Date and Temperature)
void displayHome() {
    oled.setCursor(0, 0);
    oled.setScale(1);
    Datime dt = rtc.getTime();
    if (dt.hour < 10) oled.print("0"); oled.print(dt.hour); oled.print(":");
    if (dt.minute < 10) oled.print("0"); oled.print(dt.minute); oled.print(":");
    if (dt.second < 10) oled.print("0"); oled.print(dt.second);

    oled.setCursor(2, 2);
    oled.setScale(2);
    oled.print(dt.year); oled.print("/");
    if (dt.month < 10) oled.print("0"); oled.print(dt.month); oled.print("/");
    if (dt.day < 10) oled.print("0"); oled.print(dt.day);

    oled.setCursor(50, 5);
    oled.setScale(2);
    float temperature = bme.readTemperature();
    if (isnan(temperature)) {
        oled.print("ERR");
    } else {
        oled.print(temperature, 1);
        oled.setScale(1); oled.print(" "); oled.print("*");
        oled.setScale(2); oled.print("C");
    }
    oled.setScale(1);
}

// Display Pressure Screen
void displayPressure() {
    oled.setCursor(1, 2);
    oled.setScale(2);
    oled.print("Pres:");
    oled.setCursor(0, 5);
    float pressurePa = bme.readPressure();
    if (isnan(pressurePa)) {
        oled.print(" ERR");
    } else {
        float pressureMmHg = pressurePa * 0.00750061683;
        oled.print(pressureMmHg, 1);
        oled.setScale(2);
        oled.print(" mmHg");
    }
    oled.setScale(1);
}