# Dron-Czołg

Projekt oprogramowania dla hybrydowego pojazdu łączącego cechy drona (quadcopter) i czołgu (pojazd gąsienicowy). Kod steruje silnikami, serwami oraz stabilizacją lotu w oparciu o czujniki i sygnały z aparatury RC.

## Zasada Działania Systemu (Overview)

System działa w pętli czasu rzeczywistego na mikrokontrolerze (ESP32). Główny cykl sterowania realizowany jest w następujących krokach:

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

## Struktura Projektu i Szczegółowy Opis Plików

Projekt został podzielony na moduły odpowiedzialne za konkretne aspekty działania pojazdu. Poniżej znajduje się przewodnik po plikach, który pomoże zrozumieć przepływ danych i sterowania.

### 1. Konfiguracja i Ustawienia

To najważniejsze pliki dla osoby chcącej dostosować drona do swojego sprzętu bez ingerencji w głęboką logikę kodu.

- **`include/config.h`**

  - **Rola:** Główny plik konfiguracyjny całego systemu.
  - **Co zawiera:**
    - Definicje pinów mikrokontrolera (przypisanie silników, serw, czujników).
    - Limity sygnałów sterujących (np. `MIN_THROTTLE`, `MAX_THROTTLE`).
    - Stałe fizyczne i kalibracyjne (np. zakresy sygnału PWM/DShot).
  - **Kiedy edytować:** Gdy zmieniasz piny, zmieniasz ramę drona lub chcesz zmienić ogólne zachowanie (np. deadzone drążków).

- **`include/pid_configuration.h`**
  (to jest plik, który będzie używany docelowo do konfiguracji PID, natomiast teraz paraetry są wpisane jako domyślne wartości w pliku `pids.h`)
  - **Rola:** Wydzielona konfiguracja dla algorytmów stabilizacji.
  - **Co zawiera:** Wartości wzmocnień `Kp`, `Ki`, `Kd` oraz limity całkujące (anti-windup) dla każdej osi (Roll, Pitch, Yaw) oraz dla obu trybów (wewnętrzna i zewnętrzna pętla PID).
  - **Kiedy edytować:** Podczas strojenia (tuningu) drona, gdy jest niestabilny, wpada w oscylacje lub reaguje zbyt wolno.

### 2. Główna Pętla Programu

- **`src/main.cpp`**
  - **Rola:** Punkt wejścia (Entry Point) i orkiestrator systemu.
  - **Działanie:**
    1.  `setup()`: Inicjalizuje port szeregowy, magistralę I2C, czujniki, silniki i radio.
    2.  `loop()`: Główna pętla czasu rzeczywistego.
        - Oblicza czas cyklu (`dt`).
        - Pobiera dane z radia (`Sbus_reader`).
        - Pobiera dane z IMU (`Mpu6050_Sensor`).
        - Przekazuje te dane do kontrolera silników (`Motor_controller`), który decyduje o wysterowaniu napędu.
  - **Kluczowe funkcje:**
    - `setup()`: Konfiguracja początkowa.
    - `loop()`: Pętla główna programu.

### 3. Sterowanie Silnikami i Logika Ruchu (Serce Systemu)

- **`src/motor.cpp` / `include/motor.h`**

  - **Rola:** "Mózg" sterowania ruchem. Klasa `Motor_controller`.
  - **Działanie:**
    - **Zarządzanie stanem:** Obsługuje uzbrajanie (Arming) i rozbrajanie silników ze względów bezpieczeństwa.
    - **Mikser (Mixer):** Przelicza żądania pilota (Throttle, Yaw, Pitch, Roll) na moc poszczególnych silników w zależności od trybu:
      - _Tryb Czołgu:_ Używa sterowania różnicowego (lewa/prawa gąsienica).
      - _Tryb Drona:_ Używa algorytmu quadcoptera (zmienia obroty śmigieł, aby uzyskać przechył lub wznoszenie).
    - Integruje wyniki z regulatorów PID z komendami pilota.
  - **Kluczowe funkcje:**
    - `update_motors(mpu, dt)`: Główna funkcja sterująca. Oblicza PID i miesza sygnały dla silników.
    - `update_mode(mode)`: Przełącza logikę sterowania (Czołg vs Dron).
    - `arm_motors()` / `disarm_motors()`: Zarządza bezpieczeństwem (blokada silników).
    - `safety_land()`: Procedura awaryjnego lądowania (np. przy utracie sygnału). Należy przetestować i dobrać odpowiednie wartości i ewentualnie zmienić podejście.

- **`src/DShot.cpp` / `include/DShot.h`**

  - **Rola:** Sterownik warstwy sprzętowej dla ESC (Electronic Speed Controllers).
  - **Działanie:** Implementuje cyfrowy protokół DShot (prawdopodobnie DShot600). W przeciwieństwie do analogowego PWM, wysyła cyfrowe ramki danych do regulatorów, co zapewnia wyższą precyzję i brak konieczności kalibracji zakresu gazu.
  - **Kluczowe funkcje:**
    - `sendThrottle(value)`: Wysyła zadaną wartość ciągu do konkretnego silnika.
    - `sendCommand(cmd)`: Wysyła komendy specjalne (np. beep, reset).

- **`src/servo_controller.cpp` / `include/servo_controller.h`**
  - **Rola:** Obsługa mechanizmu transformacji.
  - **Działanie:** Steruje serwomechanizmami, które zmieniają geometrię pojazdu (np. rozkładają ramiona do lotu lub składają je do jazdy).
  - **Kluczowe funkcje:**
    (w obu przypadkach należy dostosować kąty dla obu trybów)
    - `set_servo_tank_mode()`: Ustawia ramiona w pozycji do jazdy.
    - `set_servo_copter_mode()`: Ustawia ramiona w pozycji do lotu.
    - `write_angle(angle)`: Bezpośrednie ustawienie kąta serwa.

### 4. Czujniki i Stabilizacja (IMU & PID)

- **`src/mpu6050_sensor.cpp` / `include/mpu6050_sensor.h`**
  (tu powinno wszystko działać, ale też do testów, można potencjalnie zastosować bardziej zaawansowany filtr)

  - **Rola:** Zmysł równowagi drona.
  - **Działanie:**
    - Komunikuje się z układem MPU6050 po I2C.
    - Odczytuje surowe dane z akcelerometru i żyroskopu.
    - Dokonuje fuzji sensorów (np. filtrem komplementarnym), aby obliczyć aktualne kąty pochylenia drona w przestrzeni. Bez tego stabilny lot nie byłby możliwy.
  - **Kluczowe funkcje:**
    - `init()`: Konfiguracja akcelerometru i żyroskopu.
    - `compute_orientation()`: Przelicza surowe dane na kąty (Roll, Pitch) używając filtra komplementarnego.
    - `get_roll()`, `get_pitch()`, `get_yaw()`: Zwraca aktualną orientację.

- **`src/pids.cpp` / `include/pids.h`**
  (powinno działać)

  - **Rola:** Matematyczna implementacja pojedynczego regulatora PID.
  - **Działanie:** Oblicza błąd (`setpoint - measurement`) i na jego podstawie generuje sygnał korekcyjny, biorąc pod uwagę część proporcjonalną (P), całkującą (I) i różniczkującą (D).
  - **Kluczowe funkcje:**
    - `compute(meas, sp, dt)`: Oblicza sygnał sterujący na podstawie błędu.
    - `reset()`: Zeruje stan regulatora (np. część całkującą).

- **`include/pids3d.h`**
  - **Rola:** Kontener dla regulatorów.
  - **Działanie:** Grupuje trzy obiekty PID (dla osi X, Y, Z / Roll, Pitch, Yaw) w jedną klasę, ułatwiając zarządzanie stabilizacją trójwymiarową w `motor.cpp`.
  - **Kluczowe funkcje:**
    - `init_all()`: Inicjalizuje regulatory dla wszystkich osi.

### 5. Komunikacja Radiowa (RC)

(to działa)

- **`src/sbus_reader.cpp` / `include/sbus_reader.h`**
  - **Rola:** Tłumacz sygnałów z aparatury sterującej.
  - **Działanie:**
    - Odbiera strumień danych z odbiornika RC w protokole SBUS (szeregowa transmisja odwrócona).
    - Dekoduje ramki danych na wartości poszczególnych kanałów (16 kanałów).
    - **Normalizacja:** Przelicza surowe wartości cyfrowe (np. 172-1811) na format zrozumiały dla reszty systemu (np. zakres 0-1000, wartości float lub flagi logiczne dla przełączników).
    - Obsługuje Failsafe (wykrywa utratę zasięgu).
  - **Kluczowe funkcje:**
    - `Read_Sbus()`: Pobiera i dekoduje ramkę danych z odbiornika.
    - `get_throttle()`, `get_yaw()`, `get_mode()`...: Zwraca wartości konkretnych kanałów.
    - `NormalizeChannel()`: Przelicza surowe dane (172-1811) na użyteczne wartości (np. 0-1000).
