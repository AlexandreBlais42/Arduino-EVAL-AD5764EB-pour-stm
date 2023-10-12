/**
 *
 * \file Arduino_Evaluation_AD5764EB_driver.ino
 * \brief Pilote pour le CNA AD5764EB contrôler par un Raspberry Pi
 * \author David Croteau repris de https://github.com/jscastonguay/Arduino-EVAL-AD5764EB-Driver
 * \version 1.0
 * \date 02\10\2023
 */

#include <SPI.h>
#include <stdint.h>

const uint8_t SYNC_PIN = 10;
const uint8_t LDAC_PIN = 9;
const uint8_t CLR_PIN = 8;
const uint8_t RSTIN = 7;
const uint8_t BIN_2COMP = 6;

const uint8_t FUNCTION_REG = 0;
const uint8_t DATA_REG = 2;
const uint8_t COARSE_GAIN_REG = 3;
const uint8_t FINE_GAIN_REG = 4;
const uint8_t OFFSET_REG = 5;
const uint8_t ALL_DAC = 4;

uint8_t channel = 0;        // Le numéro du channel à écrire le voltage
int16_t voltage = 0;   // La valeure du voltage à écrire sur 16 bits signés

/** \brief Configuration et initisalisation du protocole SPI
 *
 */
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

  initialisationCNA();
}

/** \brief Lit constamment le port sériel en attente d'une 
 * commande et l'écrit dans un registre du DAC quand elle est reçue
 *
 */
void loop() {
  if (Serial.available() >= 5) { // 5 charactères à cause du format : A1234 (5 octets)
    channel = Serial.read() - 'A';

    // Boucle déroulée pour avoir une vitesse d'éxécution maximale;
    voltage += hexCharToInt(Serial.read());
    voltage <<= 4;
    voltage += hexCharToInt(Serial.read());
    voltage <<= 4;
    voltage += hexCharToInt(Serial.read());
    voltage <<= 4;
    voltage += hexCharToInt(Serial.read());

    writeRegister(DATA_REG, channel, voltage); 

    voltage = 0; // reset de la valeur pour la prochaine ittération
  }
}

/** \brief Prend un charactère hexadécimal et retourne sa valeur
 *  \param hexChar Le charatère hexadécimal ([0-9A-F])
 *  \return La valeur décimale (entre 0 et 15)
 */
uint8_t hexCharToInt(const uint8_t hexChar){
  if (hexChar >= '0' && hexChar <= '9'){
    return hexChar - '0';
  }
  return hexChar - 'A' + 10;
}

/** \brief Configuration du CNA AD5764EB
 *
 */
void initialisationCNA() {
  writeRegister(FUNCTION_REG, 1, 0x00);
  writeRegister(COARSE_GAIN_REG, ALL_DAC, 0x00);
  writeRegister(OFFSET_REG, ALL_DAC, 0x00);
}

/** \brief Écriture des valeurs vers le CNA AD5764EB à l'aide du protocole SPI
 *
 * \param[out] function uint8_t
 * \param[out] addr uint8_t
 * \param[out] valeurReg int16_t
 *
 */
void writeRegister(uint8_t function, uint8_t addr, int16_t valeurReg) {
  function &= 0x07;
  addr &= 0x07;

  digitalWrite(SYNC_PIN, LOW);
  SPI.transfer((function << 3) | addr);
  SPI.transfer((valeurReg >> 8));
  SPI.transfer(valeurReg & 0xFF);
  digitalWrite(SYNC_PIN, HIGH);
}
