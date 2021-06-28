#include <Arduino.h>
#include <SPI.h>
#include "SSD1306Spi.h"
#include <nrf24.h>

const int led = LED_BUILTIN;

#define OLED_RST_PIN 16
#define OLED_DC_PIN 4
#define OLED_CS_PIN 17

SSD1306Spi display(OLED_RST_PIN, OLED_DC_PIN, OLED_CS_PIN);

#define SPI_SCK_PIN 23
#define SPI_MOSI_PIN 27
#define SPI_MISO_PIN 26

const uint8_t channel = 6;

void drawFontFaceDemo()
{
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Hello world");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Hello world");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Hello world");
}

void displayReceivedData(const char *message, int rssi, int lqi)
{
  char buffer[64];
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawStringMaxWidth(0, 0, 128, message);
  display.setFont(ArialMT_Plain_16);
  display.drawStringf(0, 30, buffer, "RSSI: %d", rssi);
  display.drawStringf(0, 45, buffer, "LQI:  %d", lqi);
  display.display();
}

void dumpHex(uint8_t *data, uint32_t length)
{
  char buf[5];
  for (uint32_t i = 0; i < length; i++)
  {
    snprintf(buf, 5, "0x%02X", data[i]);
    Serial.print(buf);
    Serial.print(" ");
  }
  Serial.println();
}

bool packet_received = false;

void IRAM_ATTR isr_si4463()
{
  packet_received = true;
}

const uint8_t packet_size = 16;

uint8_t tx_address[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
uint8_t rx_address[5] = {0xD7, 0xD7, 0xD7, 0xD7, 0xD7};

void setup()
{
  pinMode(led, OUTPUT);

  // init SPI for Pico
  // SPI = arduino::MbedSPI(CC1101_MISO_PIN, CC1101_MOSI_PIN, CC1101_SCK_PIN);
  // SPI.begin();

  // init SPI for ESP32
  SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);
  SPI.setFrequency(1000000);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  drawFontFaceDemo();
  display.display();

  Serial.begin(115200);
  Serial.println("\n\nHello World!");

  delay(1000);
  randomSeed(millis());

  /* init hardware pins */
  nrf24_init();

  /* Channel #2 , payload length: 4 */
  nrf24_config(2, packet_size);

  /* Set the device addresses */
  nrf24_tx_address(tx_address);
  nrf24_rx_address(rx_address);
}

char message[64];
int n = 0;
uint32_t lastSend = 0;
uint32_t sendDelay = 500;
uint32_t lastRecv = 0;
uint8_t statusBuffer[32] = {0};
uint32_t now = millis();

void sendMessage()
{
  n = n + n / 5 + 1;
  n = n < 0 ? 1 : n;
  memset(message, 0, sizeof(message));
  snprintf(message, packet_size, "Hi %d", n);
  Serial.print("Sending message: ");
  Serial.println(message);
  uint32_t s = millis();

  /* Automatically goes to TX mode */
  nrf24_send((uint8_t *)message);

  /* Wait for transmission to end */
  while (nrf24_isSending())
    ;

  /* Make analysis on last tranmission attempt */
  uint8_t status = nrf24_lastMessageStatus();

  if (status == NRF24_TRANSMISSON_OK)
  {
    Serial.println("Tranmission went OK.");
  }
  else if (status == NRF24_MESSAGE_LOST)
  {
    Serial.println("Message is lost!");
  }

  /* Retranmission count indicates the tranmission quality */
  uint8_t count = nrf24_retransmissionCount();
  Serial.print("Retranmission count: ");
  Serial.println(count);

  /* Optionally, go back to RX mode ... */
  nrf24_powerUpRx();
  /* Or you might want to power down after TX */
  // nrf24_powerDown();

  Serial.print("Sending message use time(ms): ");
  Serial.println(millis() - s);
}

void loop()
{

  now = millis();
  if (now > lastSend + sendDelay + random(500))
  {
    lastSend = now;
    sendMessage();
  }

  now = millis();
  if (now > lastRecv + 10000)
  {
    // Serial.println("Reset Chip!");
  }

  if (nrf24_dataReady())
  {
    digitalWrite(led, HIGH);
    lastRecv = millis();
    memset(message, 0, sizeof(message));
    nrf24_getData((uint8_t *)message);
    Serial.print("Receive message: ");
    Serial.println(message);
    displayReceivedData(message, 0, 0);
    delay(100);
    digitalWrite(led, LOW);
  }
}
