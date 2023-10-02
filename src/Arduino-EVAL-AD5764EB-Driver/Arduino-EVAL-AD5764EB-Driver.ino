#include <SPI.h>
#include <stdint.h>

#define DEBUG(txt) Serial.print(txt)
#define DEBUGLN(txt) Serial.println(txt)

const int SYNC_PIN = 10;
const int LDAC_PIN = 9;
const int CLR_PIN = 8;
const int RSTIN = 7;
const int BIN_2COMP = 6;

const uint8_t FUNCTION_REG = 0;
const uint8_t DATA_REG = 2;
const uint8_t COARSE_GAIN_REG = 3;
const uint8_t FINE_GAIN_REG = 4;
const uint8_t OFFSET_REG = 5;
const uint8_t ALL_DAC = 4;

String command;

void setup() {
  Serial.begin(115200);

  pinMode(SYNC_PIN, OUTPUT);
  pinMode(LDAC_PIN, OUTPUT);
  pinMode(CLR_PIN, OUTPUT);
  pinMode(RSTIN, OUTPUT);
  pinMode(BIN_2COMP, OUTPUT);

  digitalWrite(SYNC_PIN, HIGH);
  digitalWrite(LDAC_PIN, LOW);
  digitalWrite(CLR_PIN, HIGH);
  digitalWrite(RSTIN, HIGH);
  digitalWrite(BIN_2COMP, HIGH);

  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));

  initializeDAC();
}

void loop() {
  processSerialInput();
}

void initializeDAC() {
  writeRegister(FUNCTION_REG, 1, 0x00);
  writeRegister(COARSE_GAIN_REG, ALL_DAC, 0x00);
  writeRegister(OFFSET_REG, ALL_DAC, 0x00);
}

void processSerialInput() {
  while (Serial.available()) {
    char rx = Serial.read();

    if (rx == '\r' || rx == '\n') {
      if (command.length() != 0) {
        command.trim();

        DEBUGLN("---------------");
        DEBUGLN(command);

        processCommand(command);

        // Acknowledge receipt of the command (optional)
        Serial.println("Command received: " + command);
      }
      command = "";
    } else {
      command.concat(rx);
    }
  }
}

void processCommand(const String& command) {
  char ch = command.charAt(0);

  DEBUG("ch: ");
  DEBUGLN(ch);

  if (ch >= 'a' && ch <= 'd') {
    ch -= 'a';
  } else if (ch >= 'A' && ch <= 'D') {
    ch -= 'A';
  } else if (ch >= '0' && ch <= '3') {
    ch -= '0';
  } else {
    // Handle invalid command
    DEBUGLN("Invalid command");
    return;
  }

  command.remove(0, 1);
  char valeur[5];
  command.toCharArray(valeur, 5);
  long valeurdecimale = strtol(valeur, NULL, 16);

  DEBUG("V: ");
  DEBUGLN(valeurdecimale);

  long bin = (valeurdecimale / 2) + 32768;

  DEBUG("bin: ");
  DEBUGLN(bin);

  writeRegister(DATA_REG, ch, bin);
}

void writeRegister(uint8_t function, uint8_t addr, int16_t regValue) {
  function &= 0x07;
  addr &= 0x07;

  digitalWrite(SYNC_PIN, LOW);
  SPI.transfer((function << 3) | addr);
  SPI.transfer((regValue >> 8) & 0xFF);
  SPI.transfer(regValue & 0xFF);
  digitalWrite(SYNC_PIN, HIGH);
}