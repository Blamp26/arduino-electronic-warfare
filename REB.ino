#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

// Пины для nRF24L01
#define CE_PIN 22  // Замените на нужный пин для ESP32
#define CSN_PIN 21 // Замените на нужный пин для ESP32

RF24 radio(CE_PIN, CSN_PIN);

#define CHANNELS 64
int channel[CHANNELS];
int line;
char grey[] = " .:-=+*aRW";

#define _NRF24_CONFIG 0x00
#define _NRF24_EN_AA 0x01
#define _NRF24_RF_CH 0x05
#define _NRF24_RF_SETUP 0x06
#define _NRF24_RPD 0x09

const uint8_t num_channels = 64;
int values[num_channels];
int channels = 0;
const byte address[6] = "00001";
const int num_reps = 50;
bool jamming = true;

byte getRegister(byte r)
{
  byte c;
  digitalWrite(CSN_PIN, LOW);
  c = SPI.transfer(r & 0x1F);
  c = SPI.transfer(0);
  digitalWrite(CSN_PIN, HIGH);
  return c;
}

void setup()
{
  Serial.begin(115200);
  radio.begin();
  radio.startListening();
  radio.stopListening();

  SPI.begin();
  pinMode(CE_PIN, OUTPUT);
  pinMode(CSN_PIN, OUTPUT);
  disable();

  powerUp();
  setRegister(_NRF24_EN_AA, 0x0);
  setRegister(_NRF24_RF_SETUP, 0x0F);

  Serial.println("Starting 2.4GHz Scanner...");
  Serial.println();
  Serial.println("Channel Layout");
  printChannels();
}

void setRegister(byte r, byte v)
{
  digitalWrite(CSN_PIN, LOW);
  SPI.transfer((r & 0x1F) | 0x20);
  SPI.transfer(v);
  digitalWrite(CSN_PIN, HIGH);
}

void powerUp(void)
{
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) | 0x02);
  delayMicroseconds(130);
}

void powerDown(void)
{
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) & ~0x02);
}

void enable(void)
{
  digitalWrite(CE_PIN, HIGH);
}

void disable(void)
{
  digitalWrite(CE_PIN, LOW);
}

void setRX(void)
{
  setRegister(_NRF24_CONFIG, getRegister(_NRF24_CONFIG) | 0x01);
  enable();
  delayMicroseconds(100);
}

void scanChannels(void)
{
  disable();
  for (int j = 0; j < 255; j++)
  {
    for (int i = 0; i < channels; i++)
    {
      setRegister(_NRF24_RF_CH, (128 * i) / channels);
      setRX();
      delayMicroseconds(40);
      disable();
      if (getRegister(_NRF24_RPD) > 0) channel[i]++;
    }
  }
}

void outputChannels(void)
{
  int norm = 0;
  for (int i = 0; i < CHANNELS; i++)
    if (channel[i] > norm) norm = channel[i];

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

void printChannels(void)
{
  Serial.println(">      1 2  3 4  5  6 7 8  9 10 11 12 13  14                     <");
}

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

void loop()
{
  Serial.print("Scanning channel: ");
  Serial.println(channels + 1);
  if (jamming)
  {
    Serial.print("JAMMING CHANNEL ");
    Serial.println(channels + 1);
    radio.setPALevel(RF24_PA_HIGH);
    radio.setDataRate(RF24_2MBPS);
  }
  while (jamming)
  {
    jammer();
  }

  scanChannels();
  outputChannels();

  if (line++ > 12)
  {
    printChannels();
    line = 0;
  }
}
