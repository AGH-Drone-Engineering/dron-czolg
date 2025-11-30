# Dron-Czołg

Projekt oprogramowania dla hybrydowego pojazdu łączącego cechy drona (quadcopter) i czołgu (pojazd gąsienicowy). Kod steruje silnikami, serwami oraz stabilizacją lotu w oparciu o czujniki i sygnały z aparatury RC.

## Zasada Działania Systemu (Overview)

System działa w pętli czasu rzeczywistego na mikrokontrolerze Teensy. Główny cykl sterowania realizowany jest w następujących krokach:

1.  **Odczyt Stanu:**

    - System pobiera dane z czujnika IMU (MPU6050), aby określić aktualne przechylenie i prędkość kątową pojazdu.
    - Odbiera komendy pilota z aparatury RC (protokół SBUS), interpretując wychylenia drążków jako żądaną prędkość lub kąt pochylenia.

2.  **Przetwarzanie:**

    - **PID:** Kaskadowe regulatory PID (zewnętrzna pętla: kąt→rate, wewnętrzna pętla: rate→output) porównują wartość zadaną z mierzoną i obliczają korektę.
    - **Mikser:** Przelicza wyjścia regulatorów na konkretne obroty silników w zależności od aktywnego trybu:
      - _Tryb Czołgu (TankMixer):_ Sterowanie różnicowe gąsienicami.
      - _Tryb Drona (CopterMixer):_ Sterowanie ciągiem i momentami obrotowymi 4 śmigieł (konfiguracja X).

3.  **Wykonanie:**
    - Obliczone wartości są wysyłane do regulatorów silników (ESC) przy użyciu szybkiego protokołu cyfrowego **DShot600**.
    - Serwomechanizmy odpowiadają za fizyczną transformację ramion pojazdu przy zmianie trybu.

## Architektura Projektu

Projekt wykorzystuje architekturę warstwową z wzorcem **Dependency Injection (DI)** oraz **Single Responsibility Principle (SRP)**, co umożliwia łatwe testowanie jednostkowe i podmianę implementacji sprzętowych.

```
┌─────────────────────────────────────────────────────────────┐
│                      src/main.cpp                          │
│              (Punkt wejścia, kompozycja DI)                │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                   FlightController                         │
│              (Główny orkiestrator systemu)                 │
└─────────────────────────────────────────────────────────────┘
          │           │           │           │
          ▼           ▼           ▼           ▼
┌──────────────┐ ┌──────────────┐ ┌──────────────┐ ┌──────────────┐
│ ModeManager  │ │ArmingManager │ │SafetyControl │ │Stabilization │
│  (tryby)     │ │ (arm/disarm) │ │ (failsafe)   │ │ Controller   │
└──────────────┘ └──────────────┘ └──────────────┘ └──────────────┘
                                                         │
                            ┌────────────────────────────┤
                            ▼                            ▼
                     ┌──────────────┐           ┌──────────────┐
                     │  TankMixer   │           │ CopterMixer  │
                     └──────────────┘           └──────────────┘
                            │                            │
                            └────────────┬───────────────┘
                                         ▼
                                  ┌──────────────┐
                                  │ MotorOutput  │
                                  │  (DShot ESC) │
                                  └──────────────┘
```

### Warstwy systemu

```
┌─────────────────────────────────────────────────────────────┐
│                    lib/CoreLogic/                          │
│   (Logika biznesowa - niezależna od sprzętu)              │
│   FlightController, StabilizationController, Pid,         │
│   ModeManager, ArmingManager, SafetyController,           │
│   TankMixer, CopterMixer, MotorOutput, SbusParser,        │
│   ImuLogic, ServoLogic                                     │
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

Pipeline to sekwencyjny przepływ danych w głównej pętli programu, realizowany w `FlightController.update()`. Każdy cykl trwa około 20 ms, zapewniając stabilność w czasie rzeczywistym:

1. **Odczyt z Radia:** `SbusParser` parsuje kanały SBUS na wartości throttle/yaw/pitch/roll oraz status uzbrojenia i trybu.
2. **Odczyt z IMU:** `ImuLogic` pobiera dane z sensora i oblicza orientację (roll, pitch, yaw) oraz prędkości kątowe (rate) za pomocą filtra komplementarnego.
3. **Sprawdzenie Failsafe:** `SafetyController` wykrywa utratę sygnału RC i inicjuje bezpieczne lądowanie.
4. **Aktualizacja Trybu:** `ModeManager` zarządza przełączaniem TANK↔COPTER z kontrolowaną tranzycją serw.
5. **Sprawdzenie Arming:** `ArmingManager` kontroluje uzbrojenie silników z warunkami bezpieczeństwa.
6. **Obliczenie PID:** `StabilizationController` realizuje kaskadowy PID (angle→rate→output).
7. **Miksowanie:** `TankMixer` lub `CopterMixer` przelicza wyjścia PID na wartości dla 6 silników.
8. **Wysyłka do ESC:** `MotorOutput` konwertuje wartości na pakiety DShot i wysyła do ESC.

## Struktura Projektu i Szczegółowy Opis Plików

Projekt został podzielony na warstwy odpowiedzialne za konkretne aspekty działania pojazdu.

### 1. Konfiguracja i Punkt Wejścia (`src/`)

- **`src/config.h`**

  - **Rola:** Główny plik konfiguracyjny całego systemu.
  - **Co zawiera:**
    - Piny serw i silników
    - Parametry czasowe (LOOP_DT, DEBUG_INTERVAL, TRANSITION_TIME)
    - Konfiguracja I2C i IMU (I2C_CLOCK_SPEED, IMU_FILTER_ALPHA, skale sensora)
    - **Wzmocnienia PID** dla wszystkich osi (KP, KI, KD, limity)
    - Współczynniki RC dla obu trybów (THROTTLE_COEF, STEER_COEF, etc.)
    - Stałe SBUS i DShot
    - Parametry bezpieczeństwa (SAFETY_LAND_REDUCTION_STEP)
  - **Kiedy edytować:** Gdy zmieniasz piny, stroisz PID, lub zmieniasz parametry lotu.

- **`src/main.cpp`**
  - **Rola:** Punkt wejścia (Entry Point) i kompozycja Dependency Injection.
  - **Działanie:**
    1.  Tworzy instancje sterowników sprzętowych (TeensyDrivers).
    2.  Wstrzykuje je do `FlightController` przez konstruktory.
    3.  W `loop()` wywołuje `parser.parse()`, `imu.update()` i `flightController.update(dt)`.
  - **Kluczowe elementy:**
    - Instancje sprzętowe: `TeensySbusReader`, `TeensyPidTimer`, `TeensyDShotDriver`, `TeensyMpuSensor`, `TeensyServoDriver`.
    - `FlightController` - główny orkiestrator całej logiki sterowania.

### 2. Logika Biznesowa (`lib/CoreLogic/`)

Warstwa zawierająca czystą logikę niezależną od sprzętu. Wszystkie klasy przyjmują zależności przez konstruktor (DI).

#### Główny kontroler

- **`FlightController.h`**
  - **Rola:** Główny orkiestrator systemu - łączy wszystkie komponenty.
  - **Zależności:** `ImuLogic`, `SbusParser`, `ServoLogic`, `MotorOutput`, `IPidTimer`
  - **Kluczowe metody:**
    - `init()`: Inicjalizacja wszystkich komponentów.
    - `update(float dt)`: Główna pętla sterowania - failsafe, tryb, arming, PID, mixer, output.
    - `setPidGains(...)`: Konfiguracja wzmocnień PID.
  - **Zawiera wewnętrznie:** `ModeManager`, `ArmingManager`, `SafetyController`, `StabilizationController`, `TankMixer`, `CopterMixer`.

#### Kontrola stanu

- **`ModeManager.h` / `ModeManager.cpp`**

  - **Rola:** Zarządzanie trybami pracy (TANK, COPTER, TRANSITION).
  - **Kluczowe metody:**
    - `update(float modeSwitch)`: Aktualizacja trybu na podstawie przełącznika RC.
    - `isInTransition()`: Sprawdza czy trwa transformacja.
  - **Obsługuje:** Płynną tranzycję serw przez określony czas (TRANSITION_TIME).

- **`ArmingManager.h`**

  - **Rola:** Bezpieczne uzbrajanie/rozbrajanie silników.
  - **Kluczowe metody:**
    - `canArm(throttle, inTransition)`: Sprawdza warunki bezpieczeństwa.
    - `arm()`, `disarm()`: Zmiana stanu uzbrojenia.
  - **Warunki arming:** Niski throttle + nie w tranzycji.

- **`SafetyController.h`**
  - **Rola:** Obsługa sytuacji awaryjnych (failsafe).
  - **Kluczowe metody:**
    - `checkFailsafe(sbusFailsafe, sbusLostFrame)`: Wykrywanie utraty sygnału.
    - `safetyLand(outputs, isTankMode)`: Stopniowe zmniejszanie throttle.

#### Stabilizacja

- **`StabilizationController.h`**

  - **Rola:** Kaskadowy PID dla stabilizacji lotu.
  - **Architektura:** Zewnętrzna pętla (angle→rate) + wewnętrzna pętla (rate→output).
  - **Kluczowe metody:**
    - `computeCopter(imu, throttle, yaw, pitch, roll, dt)`: Oblicza wyjścia dla trybu copter.
    - `computeTank(imu, throttle, yaw, dt)`: Oblicza wyjścia dla trybu tank.
    - `setInnerGains()`, `setOuterGains()`: Konfiguracja PID.

- **`Pid.h` / `Pid.cpp`**
  - **Rola:** Pojedynczy regulator PID z anti-windup.
  - **Kluczowe metody:**
    - `compute(measurement, setpoint, dt)`: Oblicza sygnał sterujący.
    - `reset()`: Zeruje stan (integral, prevError).
  - **Struktura `PidGains`:** kp, ki, kd, outMin, outMax.

#### Miksery

- **`IMixer.h`**

  - **Rola:** Interfejs dla mikserów silników.
  - **Struktura `MixerInput`:** throttle, yaw, pitch, roll.
  - **Metoda:** `mix(input, outputs)` - przelicza na wartości silników.

- **`TankMixer.h`**

  - **Rola:** Mikser dla trybu czołgu (sterowanie różnicowe).
  - **Formuły:** `tl = throttle + yaw`, `tr = throttle - yaw`.

- **`CopterMixer.h`**
  - **Rola:** Mikser dla trybu copter (konfiguracja X).
  - **Formuły:** Standardowy mikser quadcopter z throttle ± pitch ± roll ± yaw.

#### Wyjście silników

- **`MotorOutput.h`**

  - **Rola:** Warstwa wyjściowa - wysyła wartości do ESC przez DShot.
  - **Kluczowe metody:**
    - `send(MotorOutputs)`: Konwertuje i wysyła do wszystkich silników.
    - `reset()`: Wysyła wartość 0 (zatrzymanie).
    - `sendMinThrottle()`: Wysyła minimalny throttle (przy armowaniu).

- **`MotorOutputs.h`**
  - **Rola:** Struktura danych dla 6 silników.
  - **Pola:** fl, fr, bl, br (copter) + tl, tr (tank).

#### Wejścia

- **`SbusParser.h` / `SbusParser.cpp`**

  - **Rola:** Parsowanie surowych kanałów SBUS na wartości sterujące.
  - **Zależność:** `ISbusReader` (interfejs czytnika SBUS).
  - **Kluczowe metody:**
    - `parse()`: Odczytuje i parsuje kanały.
    - `getThrottle()`, `getYaw()`, `getPitch()`, `getRoll()`: Wartości sterujące (znormalizowane).
    - `getArmStatus()`, `getMode()`: Status uzbrojenia i tryb pracy.
    - `isFailsafe()`, `isLostFrame()`: Informacje o stanie połączenia.

- **`ImuLogic.h` / `ImuLogic.cpp`**

  - **Rola:** Przetwarzanie danych z czujnika IMU na kąty orientacji.
  - **Zależność:** `IImuSensor`, `IPidTimer`.
  - **Algorytm:** Filtr komplementarny łączący żyroskop (szybka reakcja) z akcelerometrem (brak dryfu).
  - **Kluczowe metody:**
    - `update()`: Odczytuje dane z sensora i oblicza kąty.
    - `getPitch()`, `getRoll()`, `getYaw()`: Kąty orientacji (stopnie).
    - `getPitchRate()`, `getRollRate()`, `getYawRate()`: Prędkości kątowe (°/s).
    - `setAlpha(float)`: Ustawienie współczynnika filtra.

- **`ServoLogic.h` / `ServoLogic.cpp`**

  - **Rola:** Sterowanie serwomechanizmami transformacji ramion.
  - **Zależność:** `IServoDriver`.
  - **Kluczowe metody:**
    - `setTankMode()`, `setCopterMode()`: Predefiniowane pozycje.
    - `setAngle(int)`: Ustawienie dowolnego kąta (0-180).

- **`Utils.h`**
  - **Rola:** Funkcje pomocnicze.
  - **Co zawiera:** `constrain()`, `map()`.

### 3. Interfejsy Sprzętowe (`lib/HardwareIf/`)

Abstrakcyjne interfejsy definiujące kontrakty dla sterowników sprzętowych. Umożliwiają mockowanie w testach.

- **`IImuSensor.h`**

  - **Struktura `ImuData`:** Surowe i przeliczone dane (accelX/Y/Z, gyroX/Y/Z + wersje float).
  - **Metoda:** `readData(ImuData &data)` - odczyt danych z sensora.

- **`IMotorDriver.h`**

  - **Metoda:** `sendPacket(uint16_t packet)` - wysłanie pakietu DShot.

- **`IPidTimer.h`**

  - **Metoda:** `getMillis()` - pobranie aktualnego czasu w milisekundach.

- **`ISbusReader.h`**

  - **Metody:** `read()`, `getChannel(int)`, `isFailsafe()`, `isLostFrame()`.

- **`IServoDriver.h`**
  - **Metody:** `attach(pin)`, `detach(pin)`, `setAngle(pin, angle)`, `setMicroseconds(pin, us)`, `setPwm(pin, value)`.

### 4. Sterowniki Teensy (`lib/TeensyDrivers/`)

Implementacje interfejsów dla platformy Teensy. Zawierają kod specyficzny dla sprzętu.

- **`TeensyMpuSensor.h`**

  - **Implementuje:** `IImuSensor`
  - **Biblioteka:** Adafruit_MPU6050
  - **Konfiguracja:** ±8g akcelerometr, ±500°/s żyroskop, 400kHz I2C.

- **`TeensyDShotDriver.h`**

  - **Implementuje:** `IMotorDriver`
  - **Protokół:** DShot600 przez HardwareSerial.
  - **Działanie:** Serializuje 16-bitowy pakiet (11-bit throttle + 1-bit telemetry + 4-bit CRC).

- **`TeensyPidTimer.h`**

  - **Implementuje:** `IPidTimer`
  - **Działanie:** Wrapper dla `millis()` Arduino.

- **`TeensySbusReader.h`**

  - **Implementuje:** `ISbusReader`
  - **Biblioteka:** bfs::SbusRx
  - **Działanie:** Dekoduje ramki SBUS na 16 kanałów.

- **`TeensyServoDriver.h`**
  - **Implementuje:** `IServoDriver`
  - **Biblioteka:** Servo
  - **Działanie:** Obsługuje 2 serwa transformacji (lewe i prawe).

### 5. Stary Kod (`old_code/`)

Folder zawiera poprzednią implementację przed refaktoryzacją - zachowany jako referencja:

- `DShot.cpp` - stara implementacja protokołu DShot
- `motor.cpp` - stara logika sterowania silnikami (monolityczna)
- `mpu6050_sensor.cpp` - stara obsługa czujnika IMU
- `old_main.cpp` - poprzedni punkt wejścia
- `pids.cpp` - stara implementacja PID
- `sbus_reader.cpp` - stara obsługa SBUS
- `servo_controller.cpp` - stara obsługa serw

### 6. Testy (`test/`) - niedokończone

- **`test/mocks/`** - Mocki bibliotek Arduino i sprzętowych dla testów jednostkowych.
- **`test/test_DShot/`** - Testy dla protokołu DShot.
- **`test/test_sbus/`** - Testy dla parsera SBUS

## Konfiguracja

Główne parametry do dostrojenia znajdują się w `src/config.h`:

### Wzmocnienia PID

```cpp
// Wewnętrzna pętla (rate) - reakcja na prędkość kątową
#define PID_YAW_RATE_KP 1.0f
#define PID_PITCH_RATE_KP 1.0f
#define PID_ROLL_RATE_KP 1.0f

// Zewnętrzna pętla (angle) - konwersja błędu kąta na setpoint rate
#define PID_PITCH_ANGLE_KP 2.0f
#define PID_ROLL_ANGLE_KP 2.0f
```

### Filtr IMU

```cpp
// Współczynnik filtra komplementarnego (0.9-0.99)
// Wyższy = szybsza reakcja, więcej żyroskopu
// Niższy = stabilniejszy, więcej akcelerometru
#define IMU_FILTER_ALPHA 0.98f
```

### Bezpieczeństwo

```cpp
#define SAFETY_LAND_REDUCTION_STEP 50.0f  // Redukcja throttle przy safety land
#define TRANSITION_TIME 3000              // Czas transformacji [ms]
```
