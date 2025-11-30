# Dron-Czołg

Projekt oprogramowania dla hybrydowego pojazdu łączącego cechy drona (quadcopter) i czołgu (pojazd gąsienicowy). Kod steruje silnikami, serwami oraz stabilizacją lotu w oparciu o czujniki i sygnały z aparatury RC.

## Zasada Działania Systemu (Overview)

System działa w pętli czasu rzeczywistego na mikrokontrolerze Teensy. Główny cykl sterowania realizowany jest w następujących krokach:

1.  **Odczyt Stanu:**

    - System pobiera dane z czujnika IMU (MPU6050), aby określić aktualne przechylenie i prędkość kątową pojazdu.
    - Odbiera komendy pilota z aparatury RC (protokół SBUS), interpretując wychylenia drążków jako żądaną prędkość lub kąt pochylenia.

2.  **Przetwarzanie:**

    - **PID:** Algorytmy regulatorów porównują _wartość zadaną_ (od pilota) z _wartością mierzoną_ (z czujników) i obliczają korektę niezbędną do stabilizacji.
    - **Mikser:** Przelicza wyjścia regulatorów na konkretne obroty silników w zależności od aktywnego trybu:
      - _Tryb Czołgu:_ Sterowanie różnicowe gąsienicami.
      - _Tryb Drona:_ Sterowanie ciągiem i momentami obrotowymi 4 śmigieł.

3.  **Wykonanie:**
    - Obliczone wartości są wysyłane do regulatorów silników (ESC) przy użyciu szybkiego protokołu cyfrowego **DShot**.
    - Serwomechanizmy odpowiadają za fizyczną transformację ramion pojazdu przy zmianie trybu.

## Architektura Projektu

Projekt wykorzystuje architekturę warstwową z wzorcem **Dependency Injection (DI)**, co umożliwia łatwe testowanie jednostkowe i podmianę implementacji sprzętowych.

```
┌─────────────────────────────────────────────────────────────┐
│                      src/main.cpp                          │
│              (Punkt wejścia, kompozycja DI)                │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    lib/CoreLogic/                          │
│   (Logika biznesowa - niezależna od sprzętu)              │
│   SbusParser, PidController, MotorLogic, ImuLogic,        │
│   ServoLogic, Utils                                        │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    lib/HardwareIf/                         │
│   (Interfejsy abstrakcyjne - kontrakty dla sprzętu)       │
│   IImuSensor, IMotorDriver, IPidTimer, ISbusReader,       │
│   IServoDriver                                             │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                   lib/TeensyDrivers/                       │
│   (Implementacje sprzętowe dla Teensy)                    │
│   TeensyMpuSensor, TeensyDShotDriver, TeensyPidTimer,     │
│   TeensySbusReader, TeensyServoDriver                      │
└─────────────────────────────────────────────────────────────┘
```

### Pipeline Przetwarzania Danych

Pipeline to sekwencyjny przepływ danych w głównej pętli programu, realizowany w `loop()` w `src/main.cpp`. Każdy cykl trwa około 10-20 ms (w zależności od obciążenia), zapewniając stabilność w czasie rzeczywistym. Kroki pipeline'u są następujące:

1. **Odczyt z Radia:** `SbusParser` parsuje kanały SBUS na wartości throttle/yaw/pitch/roll oraz status uzbrojenia i trybu.
2. **Odczyt z IMU:** `ImuLogic` pobiera dane z sensora i oblicza orientację (roll, pitch, yaw).
3. **Obliczenie PID:** `PidController` porównuje setpointy (od pilota) z pomiarami (z IMU) i generuje sygnały korekcyjne.
4. **Aktualizacja Silników:** `MotorLogic` konwertuje wartości throttle na pakiety DShot i wysyła do ESC.
5. **Aktualizacja Serw:** `ServoLogic` ustawia kąty serwomechanizmów dla transformacji ramion.
6. **Powtórzenie:** Pętla się powtarza, zapewniając ciągłą korektę.

## Struktura Projektu i Szczegółowy Opis Plików

Projekt został podzielony na warstwy odpowiedzialne za konkretne aspekty działania pojazdu.

### 1. Konfiguracja i Punkt Wejścia (`src/`)

- **`src/config.h`**

  - **Rola:** Główny plik konfiguracyjny całego systemu.
  - **Co zawiera:**
    - Definicje pinów mikrokontrolera (przypisanie silników, serw, czujników).
    - Mapowanie portów szeregowych dla silników (`MOTOR_PORT_FL`, `MOTOR_PORT_FR`, itd.).
    - Stałe SBUS (`SBUS_MIN`, `SBUS_MAX`) i DShot (`DSHOT_THROTTLE_ACTIVE_MIN`, `DSHOT_THROTTLE_ACTIVE_MAX`).
    - Definicje trybów pracy (`MODE_TANK`, `MODE_COPTER`, `MODE_TRANSITION`).
    - Współczynniki sterowania dla obu trybów.
  - **Kiedy edytować:** Gdy zmieniasz piny, zmieniasz ramę drona lub chcesz zmienić ogólne zachowanie.

- **`src/main.cpp`**
  - **Rola:** Punkt wejścia (Entry Point) i kompozycja Dependency Injection.
  - **Działanie:**
    1.  Tworzy instancje sterowników sprzętowych (TeensyDrivers).
    2.  Wstrzykuje je do klas logiki (CoreLogic) przez konstruktory.
    3.  W `loop()` realizuje główny pipeline przetwarzania.
  - **Kluczowe elementy:**
    - Instancje sprzętowe: `TeensySbusReader`, `TeensyPidTimer`, `TeensyDShotDriver`, `TeensyMpuSensor`, `TeensyServoDriver`.
    - Obiekty logiki: `SbusParser`, `PidController`, `MotorLogic`, `ImuLogic`, `ServoLogic`.

### 2. Logika Biznesowa (`lib/CoreLogic/`)

Warstwa zawierająca czystą logikę niezależną od sprzętu. Wszystkie klasy przyjmują zależności przez konstruktor (DI).

- **`ImuLogic.h` / `ImuLogic.cpp`**

  - **Rola:** Przetwarzanie danych z czujnika IMU na kąty orientacji.
  - **Zależność:** `IImuSensor` (interfejs sensora).
  - **Kluczowe metody:**
    - `update()`: Odczytuje dane z sensora i oblicza kąty (roll, pitch, yaw).
    - `getPitch()`, `getRoll()`, `getYaw()`: Gettery dla obliczonych kątów.

- **`MotorLogic.h` / `MotorLogic.cpp`**

  - **Rola:** Konwersja wartości throttle na pakiety DShot i wysyłka do silników.
  - **Zależność:** `IMotorDriver` (interfejs sterownika silnika).
  - **Kluczowe metody:**
    - `setThrottle(float throttle)`: Konwertuje throttle na pakiet DShot i wysyła.
    - `createPacket(uint16_t value, bool telemetry)`: Funkcja pomocnicza tworzenia pakietu DShot.

- **`PidController.h` / `PidController.cpp`**

  - **Rola:** Implementacja algorytmu PID dla 4 osi (throttle, yaw, pitch, roll).
  - **Zależność:** `IPidTimer` (interfejs timera dla obliczenia dt).
  - **Kluczowe metody:**
    - `compute(setpoints, measurements)`: Oblicza sygnały sterujące na podstawie błędów.
  - **Struktura `PidGains`:** Przechowuje wzmocnienia `kp`, `ki`, `kd` dla każdej osi.

- **`SbusParser.h` / `SbusParser.cpp`**

  - **Rola:** Parsowanie surowych kanałów SBUS na wartości sterujące.
  - **Zależność:** `ISbusReader` (interfejs czytnika SBUS).
  - **Kluczowe metody:**
    - `parse()`: Odczytuje i parsuje kanały.
    - `getThrottle()`, `getYaw()`, `getPitch()`, `getRoll()`: Wartości sterujące.
    - `getArmStatus()`, `getMode()`: Status uzbrojenia i tryb pracy.

- **`ServoLogic.h` / `ServoLogic.cpp`**

  - **Rola:** Sterowanie serwomechanizmami transformacji ramion.
  - **Zależność:** `IServoDriver` (interfejs sterownika serw).
  - **Kluczowe metody:**
    - `setAngle(float angle)`: Mapuje kąt na PWM i wysyła do serw.

- **`Utils.h`**
  - **Rola:** Funkcje pomocnicze.
  - **Co zawiera:**
    - `constrain(value, min, max)`: Ogranicza wartość do zadanego zakresu.
    - `map(value, fromLow, fromHigh, toLow, toHigh)`: Mapuje wartość z jednego zakresu na drugi.

### 3. Interfejsy Sprzętowe (`lib/HardwareIf/`)

Abstrakcyjne interfejsy definiujące kontrakty dla sterowników sprzętowych. Umożliwiają mockowanie w testach.

- **`IImuSensor.h`**

  - **Rola:** Interfejs dla czujników IMU.
  - **Struktura `ImuData`:** Przechowuje surowe dane (accelX/Y/Z, gyroX/Y/Z).
  - **Metoda:** `readData(ImuData &data)` - odczyt surowych danych.

- **`IMotorDriver.h`**

  - **Rola:** Interfejs dla sterowników silników (DShot).
  - **Metoda:** `sendPacket(uint16_t packet)` - wysłanie pakietu DShot.

- **`IPidTimer.h`**

  - **Rola:** Interfejs dla timera systemowego.
  - **Metoda:** `getMillis()` - pobranie aktualnego czasu w milisekundach.

- **`ISbusReader.h`**

  - **Rola:** Interfejs dla czytnika SBUS.
  - **Metody:**
    - `readChannels()` - odczyt kanałów z odbiornika.
    - `getChannel(int ch)` - pobranie wartości konkretnego kanału.

- **`IServoDriver.h`**
  - **Rola:** Interfejs dla sterownika serw.
  - **Metoda:** `setPwm(uint8_t pin, uint16_t pwmValue)` - ustawienie PWM na pinie.

### 4. Sterowniki Teensy (`lib/TeensyDrivers/`)

Implementacje interfejsów dla platformy Teensy. Zawierają kod specyficzny dla sprzętu.

- **`TeensyMpuSensor.h`**

  - **Implementuje:** `IImuSensor`
  - **Rola:** Komunikacja z MPU6050 po I2C.
  - **Działanie:** Odczytuje 14 bajtów rejestrów akcelerometru i żyroskopu.

- **`TeensyDShotDriver.h`**

  - **Implementuje:** `IMotorDriver`
  - **Rola:** Wysyłanie pakietów DShot przez UART.
  - **Działanie:** Serializuje 16-bitowy pakiet na 2 bajty i wysyła przez `HardwareSerial`.

- **`TeensyPidTimer.h`**

  - **Implementuje:** `IPidTimer`
  - **Rola:** Wrapper dla funkcji `millis()` Arduino.

- **`TeensySbusReader.h`**

  - **Implementuje:** `ISbusReader`
  - **Rola:** Odczyt SBUS przy użyciu biblioteki `bfs::SbusRx`.
  - **Działanie:** Dekoduje ramki SBUS na 16 kanałów.

- **`TeensyServoDriver.h`**
  - **Implementuje:** `IServoDriver`
  - **Rola:** Generowanie sygnału PWM dla serw.
  - **Działanie:** Używa `analogWrite()` do ustawienia PWM.

### 5. Stary Kod (`old_code/`)

Folder zawiera poprzednią implementację przed refaktoryzacją do architektury warstwowej:

- `DShot.cpp` - stara implementacja protokołu DShot
- `motor.cpp` - stara logika sterowania silnikami
- `mpu6050_sensor.cpp` - stara obsługa czujnika IMU
- `old_main.cpp` - poprzedni punkt wejścia
- `pids.cpp` - stara implementacja PID
- `sbus_reader.cpp` - stara obsługa SBUS
- `servo_controller.cpp` - stara obsługa serw

### 6. Testy (`test/`) - niedokończone

- **`test/mocks/`** - Mocki bibliotek Arduino i sprzętowych dla testów jednostkowych.
- **`test/test_DShot/`** - Testy dla protokołu DShot.
- **`test/test_sbus/`** - Testy dla parsera SBUS
