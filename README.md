# Метеостанция на Arduino Mega 2560

## Описание
Этот проект представляет собой метеостанцию, построенную на базе Arduino Mega 2560. Устройство собирает данные о температуре, влажности, атмосферном давлении и отображает текущее время на OLED-дисплее. Управление осуществляется с помощью жестов, распознаваемых датчиком PAJ7620U2.

Цель проекта — продемонстрировать навыки работы с микроконтроллерами, датчиками и программированием на C++ в среде Arduino.

## Используемые компоненты
- **Arduino Mega 2560** — основная плата управления.
- **DS3231** — модуль реального времени (RTC) для отслеживания даты и времени.
- **OLED I2C** — дисплей для вывода данных.
- **BMP280** — датчик атмосферного давления и температуры.
- **HTU21** — датчик влажности и температуры.
- **PAJ7620U2** — датчик жестов для бесконтактного управления.

## Установка и настройка
1. Установите [Arduino IDE](https://www.arduino.cc/en/software).
2. Установите следующие библиотеки через менеджер библиотек Arduino:
   - `GyverOLED` — для работы с OLED-дисплеем.
   - `GyverDS3231` — для модуля RTC.
   - `GyverBME280` — для датчика BMP280.
   - `GyverHTU21D` — для датчика HTU21.
   - `paj7620` — для датчика жестов.
   - `Wire` — стандартная библиотека для I2C.
3. Подключите компоненты согласно схеме 
4. Откройте файл `meteo.ino` в Arduino IDE и загрузите его на плату.

## Использование
- После включения метеостанция отображает текущую дату, время и температуру на главном экране.
- Используйте жесты для переключения между экранами.

