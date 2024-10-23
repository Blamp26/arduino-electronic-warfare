#define TX_PIN 23 // Пин для передачи данных (D23)

// Параметры генерации сигнала
int minHighDuration = 50;  // Минимальная длина HIGH
int maxHighDuration = 400; // Максимальная длина HIGH
int minLowDuration = 50;   // Минимальная длина LOW
int maxLowDuration = 400;  // Максимальная длина LOW

// Функция для генерации случайных длительностей импульсов
int getRandomDelay(int minDelay, int maxDelay) {
  return random(minDelay, maxDelay);
}

// Функция для динамической амплитудной модуляции (AM)
void amplitudeModulation(int cycles) {
  Serial.println("=== Начало амплитудной модуляции (AM) ===");
  Serial.print("Количество циклов модуляции: ");
  Serial.println(cycles);
  
  for (int i = 0; i < cycles; i++) {
    int highDuration = getRandomDelay(minHighDuration, maxHighDuration);
    int lowDuration = getRandomDelay(minLowDuration, maxLowDuration);

    Serial.print("Цикл ");
    Serial.print(i + 1);
    Serial.print(": HIGH на ");
    Serial.print(highDuration);
    Serial.print(" мкс, LOW на ");
    Serial.print(lowDuration);
    Serial.println(" мкс");

    analogWrite(TX_PIN, 255); // Максимальная мощность
    delayMicroseconds(highDuration);
    analogWrite(TX_PIN, 127); // Половинная мощность (модуляция)
    delayMicroseconds(lowDuration);
  }
  Serial.println("=== Конец амплитудной модуляции (AM) ===");
}

// Функция для генерации паттернов помех с динамической мощностью и задержками
void generateAdvancedNoise() {
  int noiseDuration = random(3000, 7000); // Длительность шума (в мс)
  Serial.println("=== Генерация сложных паттернов помех ===");
  Serial.print("Длительность шума: ");
  Serial.print(noiseDuration);
  Serial.println(" мс");
  
  for (int i = 0; i < noiseDuration; i++) {
    int highDuration = random(minHighDuration, maxHighDuration);
    int lowDuration = random(minLowDuration, maxLowDuration);

    Serial.print("Цикл ");
    Serial.print(i + 1);
    Serial.print(": HIGH на ");
    Serial.print(highDuration);
    Serial.print(" мкс, LOW на ");
    Serial.print(lowDuration);
    Serial.println(" мкс");

    digitalWrite(TX_PIN, HIGH);
    delayMicroseconds(highDuration);

    digitalWrite(TX_PIN, LOW);
    delayMicroseconds(lowDuration);

    // Меняем параметры каждые 200 циклов
    if (i % 200 == 0) {
      minHighDuration = random(30, 100);
      maxHighDuration = random(300, 600);
      minLowDuration = random(30, 100);
      maxLowDuration = random(300, 600);
      Serial.println("Изменение параметров HIGH/LOW длительностей");
    }
  }
  Serial.println("=== Конец генерации помех ===");
}

// Функция для изменения амплитуды сигнала
void advancedSignalModulation() {
  int modulationCycles = random(5, 15); // Количество циклов модуляции
  Serial.println("=== Начало сложной модуляции сигнала (АМ) ===");
  Serial.print("Количество циклов модуляции: ");
  Serial.println(modulationCycles);
  
  for (int i = 0; i < modulationCycles; i++) {
    Serial.print("Амплитудная модуляция, цикл ");
    Serial.println(i + 1);
    
    // Применение амплитудной модуляции
    amplitudeModulation(random(500, 1000));

    // Паузы между сериями
    int delayDuration = random(500, 1000);
    Serial.print("Пауза между сериями: ");
    Serial.print(delayDuration);
    Serial.println(" мс");
    
    delay(delayDuration);
  }
  Serial.println("=== Конец сложной модуляции сигнала (АМ) ===");
}

void setup() {
  pinMode(TX_PIN, OUTPUT);  // Настраиваем пин D23 как выход
  randomSeed(analogRead(0)); // Инициализация случайного генератора
  
  Serial.begin(115200); // Инициализируем последовательный порт для вывода данных
  Serial.println("=== Инициализация завершена ===");
}

void loop() {
  Serial.println("=== Новый цикл работы ===");
  
  // Генерация сложных паттернов помех
  generateAdvancedNoise();
  int noisePause = random(1000, 3000); // Пауза между сериями
  Serial.print("Пауза перед следующей серией помех: ");
  Serial.print(noisePause);
  Serial.println(" мс");
  
  delay(noisePause);

  // Сложная модуляция сигнала (АМ)
  advancedSignalModulation();
  int modPause = random(1000, 3000); // Пауза перед следующим циклом
  Serial.print("Пауза перед следующим циклом модуляции: ");
  Serial.print(modPause);
  Serial.println(" мс");
  
  delay(modPause);
}
