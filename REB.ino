#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

// Піни для підключення модуля nRF24L01 до ESP32
// CE_PIN - використовується для активації радіомодуля
#define CE_PIN 22  // Підключіть CE до GPIO 22 на ESP32
// CSN_PIN - використовується для вибору модуля nRF24L01 під час обміну даними по SPI
#define CSN_PIN 21 // Підключіть CSN до GPIO 21 на ESP32

// Ініціалізація модуля RF24 з вказаними пинами CE та CSN
RF24 radio(CE_PIN, CSN_PIN);

#define CHANNELS 64  // Кількість каналів для сканування
int channel[CHANNELS]; // Масив для збереження даних про активність на каналах
int line;              // Лічильник ліній для виводу результатів у консоль
char grey[] = " .:-=+*aRW"; // Символи для відображення рівнів сигналу

// Регістри модуля nRF24L01 для налаштування параметрів
#define _NRF24_CONFIG 0x00
#define _NRF24_EN_AA 0x01
#define _NRF24_RF_CH 0x05
#define _NRF24_RF_SETUP 0x06
#define _NRF24_RPD 0x09

const uint8_t num_channels = 64; // Кількість каналів, що сканується
int values[num_channels];       // Масив для збереження результатів сканування
int channels = 0;               // Поточний канал
const byte address[6] = "00001"; // Адреса для передачі даних
const int num_reps = 50;        // Кількість повторень
bool jamming = true;            // Прапорець для активації режиму глушіння

// Функція для читання значення регістру модуля nRF24L01
byte getRegister(byte r)
{
  byte c;
  digitalWrite(CSN_PIN, LOW);          // Активуємо CSN для SPI-зв'язку
  c = SPI.transfer(r & 0x1F);          // Передаємо адресу регістру
  c = SPI.transfer(0);                 // Читаємо дані з регістру
  digitalWrite(CSN_PIN, HIGH);         // Відключаємо CSN
  return c;
}

void setup()
{
  Serial.begin(115200);               // Ініціалізація серійного зв’язку
  radio.begin();                      // Ініціалізація модуля RF24
  radio.startListening();             // Переходимо в режим прийому
  radio.stopListening();              // Завершуємо режим прийому

  SPI.begin();                        // Ініціалізація SPI
  pinMode(CE_PIN, OUTPUT);            // Налаштування CE як вихід
  pinMode(CSN_PIN, OUTPUT);           // Налаштування CSN як вихід
  disable();                          // Деактивація модуля

  powerUp();                          // Активація модуля
  setRegister(_NRF24_EN_AA, 0x0);     // Вимкнення автоматичного підтвердження (ACK)
  setRegister(_NRF24_RF_SETUP, 0x0F); // Встановлення максимальної потужності передавача

  Serial.println("Запуск сканера на 2.4ГГц...");
  Serial.println();
  Serial.println("Відображення каналів");
  printChannels();                     // Вивід каналів у консоль
}

// Функція для встановлення значення в регістр модуля nRF24L01
void setRegister(byte r, byte v)
{
  digitalWrite(CSN_PIN, LOW);          // Активуємо CSN для SPI-зв'язку
  SPI.transfer((r & 0x1F) | 0x20);     // Передаємо адресу регістру для запису
  SPI.transfer(v);                     // Записуємо значення в регістр
  digitalWrite(CSN_PIN, HIGH);         // Відключаємо CSN
}

// Вмикаємо живлення модуля
void powerUp(void)
{
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) | 0x02);
  delayMicroseconds(130);              // Затримка для стабілізації
}

// Вимикаємо живлення модуля
void powerDown(void)
{
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) & ~0x02);
}

// Активація модуля
void enable(void)
{
  digitalWrite(CE_PIN, HIGH);
}

// Деактивація модуля
void disable(void)
{
  digitalWrite(CE_PIN, LOW);
}

// Перемикання модуля в режим прийому
void setRX(void)
{
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) | 0x01);
  enable();
  delayMicroseconds(100);
}

// Сканування каналів для пошуку активності
void scanChannels(void)
{
  disable();
  for (int j = 0; j < 255; j++)
  {
    for (int i = 0; i < channels; i++)
    {
      setRegister(_NRF24_RF_CH, (128 * i) / channels); // Встановлюємо канал
      setRX();                                         // Перемикаємося в режим прийому
      delayMicroseconds(40);                           // Затримка
      disable();                                       // Деактивація
      if (getRegister(_NRF24_RPD) > 0) channel[i]++;   // Перевірка наявності сигналу
    }
  }
}

// Функція для виведення активності на кожному каналі
void outputChannels(void)
{
  int norm = 0;
  for (int i = 0; i < CHANNELS; i++)
    if (channel[i] > norm) norm = channel[i]; // Визначаємо максимальне значення

  Serial.print('|');
  for (int i = 0; i < CHANNELS; i++)
  {
    int pos;
    if (norm != 0) pos = (channel[i] * 10) / norm;
    else pos = 0;
    if (pos == 0 && channel[i] > 0) pos++;
    if (pos > 9) pos = 9;
    Serial.print(grey[pos]);
    channel[i] = 0;
  }
  Serial.print("| ");
  Serial.println(norm);
}

// Функція для виведення інформації про канали
void printChannels(void)
{
  Serial.println(">      1 2  3 4  5  6 7 8  9 10 11 12 13  14                     <");
}

// Режим глушіння, який передає завадовий сигнал
void jammer()
{
  const char text[] = {random(0, 256), random(0, 256), random(0, 256), random(0, 256)};
  for (int i = 0; i < CHANNELS; i++)
  {
    if (channel[i] > 0)  // Глушим только активные каналы
    {
      radio.setChannel(i);
      for (int repeat = 0; repeat < 5; repeat++)  // Повторяем шум
      {
        radio.write(&text, sizeof(text));
        delayMicroseconds(20); // Краткая пауза для предотвращения перегрева
      }
    }
  }
}

// Головний цикл програми
void loop()
{
  Serial.print("Сканування каналу: ");
  Serial.println(channels + 1);
  if (jamming)
  {
    Serial.print("ГЛУШЕННЯ КАНАЛУ ");
    Serial.println(channels + 1);
    radio.setPALevel(RF24_PA_HIGH); // Встановлюємо високу потужність сигналу
    radio.setDataRate(RF24_2MBPS);  // Встановлюємо швидкість передачі 2 Мбіт/с
  }
  while (jamming)
  {
    jammer();                       // Активуємо глушіння
  }

  scanChannels();                   // Виконуємо сканування каналів
  outputChannels();                 // Виводимо результат сканування

  if (line++ > 12)
  {
    printChannels();                // Виводимо назви каналів
    line = 0;
  }
}
