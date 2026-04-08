#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_VERDE 7 // Pino para o LED verde (acesso permitido)
#define LED_VERMELHO 6 // Pino para o LED vermelho (acesso negado)

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Cria uma instância do objeto MFRC522

// Defina o UID do cartão mestre aqui. Substitua pelos valores do seu cartão.
// Para descobrir o UID do seu cartão, carregue este código uma vez, leia o seu cartão
// e anote o UID que aparece no Monitor Serial.
byte masterCard[] = {0x04, 0x00, 0x00, 0x00}; // Exemplo: Substitua por um UID válido do seu cartão

void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial com o PC
  while (!Serial);    // Espera a porta serial conectar. Necessário apenas para porta USB nativa
  SPI.begin();        // Inicializa o barramento SPI
  mfrc522.PCD_Init(); // Inicializa o módulo MFRC522

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  Serial.println("Aproxime o seu cartão RFID do leitor...");
  Serial.println();
}

void loop() {
  // Procura por novos cartões
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Seleciona um dos cartões
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Mostra alguns detalhes do PICC (tag/cartão)
  Serial.print("UID do cartão: ");
  String currentUid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    currentUid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Verifica se o UID lido corresponde ao cartão mestre
  if (compareUids(mfrc522.uid.uidByte, masterCard)) {
    Serial.println("Acesso LIBERADO!");
    digitalWrite(LED_VERDE, HIGH);
    delay(2000);
    digitalWrite(LED_VERDE, LOW);
  } else {
    Serial.println("Acesso NEGADO!");
    digitalWrite(LED_VERMELHO, HIGH);
    delay(2000);
    digitalWrite(LED_VERMELHO, LOW);
  }

  // Para o PICC
  mfrc522.PICC_HaltA();

  // Para a criptografia no PCD
  mfrc522.PCD_StopCrypto1();
}

// Função para comparar dois UIDs
bool compareUids(byte *uid1, byte *uid2) {
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (uid1[i] != uid2[i]) {
      return false;
    }
  }
  return true;
}
