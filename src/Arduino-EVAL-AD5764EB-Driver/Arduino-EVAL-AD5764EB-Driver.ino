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

/** \brief Procède la fonction procedureSerialInput(); de manière continue
 *
 */
void loop() {
  procedureSerialInput();
}

/** \brief Configuration du CNA AD5764EB
 *
 */
void initialisationCNA() {
  writeRegister(FUNCTION_REG, 1, 0x00);
  writeRegister(COARSE_GAIN_REG, ALL_DAC, 0x00);
  writeRegister(OFFSET_REG, ALL_DAC, 0x00);
}

/** \brief Acquisition et vérification de la commande de contrôle selon le format A7B7B\n Où ch: Le canal utilisé, soit A, B, C, D valeur: La valeur 16bit en hexadécimal majuscule souhaité. Exemple : 7FFF pour 10V retour: Une nouvelle ligne ou un retour de chariot
 *
 */
void procedureSerialInput() {
  static String command;  // Déclaration de la variable command de manière statique pour que sa valeur persiste entre les itérations de la boucle

  while (Serial.available()) {
    char rx = Serial.read();

    if (rx == '\r' || rx == '\n') {
      if (command.length() != 0) {
        command.trim();

        Serial.println("---------------");
        Serial.println(command);

        procedureCommande(command);

        // Accuser réception de la commande
        Serial.println("Commande reçue: " + command);
      }
      command = "";  // Réinitialise la commande string lorsque le format de la commande est valide.
    } else {
      command.concat(rx);
    }
  }
}

/** \brief Traitement de la chaine de caractère command
  *
  * \param[in] command const String&   Pointeur sur la chaine de caractère command
  *
  */
void procedureCommande(const String& command) {
  char c = command.charAt(0);

  Serial.print("c: ");
  Serial.println(c);

  if ((c >= 'a' && c <= 'd') || (c >= 'A' && c <= 'D') || (c >= '0' && c <= '3')) {
    int canalValeur = (c >= 'a' && c <= 'd') ? (c - 'a') : ((c >= 'A' && c <= 'D') ? (c - 'A') : (c - '0'));

    String valeur = command.substring(1, 5);
    int16_t valeurdecimale = strtol(valeur.c_str(), NULL, 16);

    Serial.print("valeur décimale: ");
    Serial.println(valeurdecimale);

    writeRegister(DATA_REG, canalValeur, valeurdecimale);

  } else {
    // Gère les mauvaises commandes
    Serial.println("Commande invalide");
  }
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
  SPI.transfer((valeurReg >> 8) & 0xFF);
  SPI.transfer(valeurReg & 0xFF);
  digitalWrite(SYNC_PIN, HIGH);
}