#define TX_PIN 23

int minHighDuration = 50;  
int maxHighDuration = 400; 
int minLowDuration = 50;   
int maxLowDuration = 400;  

int getRandomDelay(int minDelay, int maxDelay) {
  return random(minDelay, maxDelay);
}

void amplitudeModulation(int cycles) {
  Serial.println("=== Початок амплітудної модуляції (AM) ===");
  Serial.print("Кількість циклів модуляції: ");
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

    analogWrite(TX_PIN, 255);
    delayMicroseconds(highDuration);
    analogWrite(TX_PIN, 127);
    delayMicroseconds(lowDuration);
  }
  Serial.println("=== Кінець амплітудної модуляції (AM) ===");
}

void generateAdvancedNoise() {
  int noiseDuration = random(3000, 7000); 
  Serial.println("=== Генерація складних патернів перешкод ===");
  Serial.print("Тривалість перешкод: ");
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

    if (i % 200 == 0) {
      minHighDuration = random(30, 100);
      maxHighDuration = random(300, 600);
      minLowDuration = random(30, 100);
      maxLowDuration = random(300, 600);
      Serial.println("Зміна параметрів тривалості HIGH/LOW");
    }
  }
  Serial.println("=== Кінець генерації перешкод ===");
}

void advancedSignalModulation() {
  int modulationCycles = random(5, 15); 
  Serial.println("=== Початок складної модуляції сигналу (AM) ===");
  Serial.print("Кількість циклів модуляції: ");
  Serial.println(modulationCycles);
  
  for (int i = 0; i < modulationCycles; i++) {
    Serial.print("Амплітудна модуляція, цикл ");
    Serial.println(i + 1);
    
    amplitudeModulation(random(500, 1000));

    int delayDuration = random(500, 1000);
    Serial.print("Пауза між серіями: ");
    Serial.print(delayDuration);
    Serial.println(" мс");
    
    delay(delayDuration);
  }
  Serial.println("=== Кінець складної модуляції сигналу (AM) ===");
}

void setup() {
  pinMode(TX_PIN, OUTPUT);
  randomSeed(analogRead(0));
  
  Serial.begin(115200);
  Serial.println("=== Ініціалізація завершена ===");
}

void loop() {
  Serial.println("=== Новий цикл роботи ===");
  
  generateAdvancedNoise();
  int noisePause = random(1000, 3000); 
  Serial.print("Пауза перед наступною серією перешкод: ");
  Serial.print(noisePause);
  Serial.println(" мс");
  
  delay(noisePause);

  advancedSignalModulation();
  int modPause = random(1000, 3000); 
  Serial.print("Пауза перед наступним циклом модуляції: ");
  Serial.print(modPause);
  Serial.println(" мс");
  
  delay(modPause);
}
