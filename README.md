# Leitor RFID RC522 com Arduino: Leitura de UID de Chaves, Controle de Acesso com LEDs e Servo Motor

## 1. Introdução

Este documento fornece um guia completo para utilizar o módulo leitor RFID RC522 com um Arduino, focando na leitura do UID (Unique Identifier) de cartões e tags RFID, na implementação de um sistema básico de controle de acesso visualizado por LEDs e na atuação de um servo motor. O RFID (Radio-Frequency Identification) é uma tecnologia que utiliza campos eletromagnéticos para identificar automaticamente e rastrear tags anexadas a objetos. O módulo RC522 é um leitor/gravador de RFID de baixo custo e amplamente utilizado em projetos com microcontroladores como o Arduino.

O objetivo deste projeto é demonstrar como configurar o hardware e o software para ler o UID de qualquer tag RFID compatível com o padrão MIFARE Classic e, com base nesse UID, indicar acesso permitido (LED verde e servo motor gira) ou negado (LED vermelho e servo motor permanece parado).

## 2. Componentes Necessários

Para replicar este projeto, você precisará dos seguintes componentes:

*   **Arduino Uno** (ou qualquer outro modelo compatível)
*   **Módulo Leitor RFID RC522**
*   **Cartão/Tag RFID** (geralmente acompanha o módulo RC522)
*   **LED Verde**
*   **LED Vermelho**
*   **Resistores de 220 Ohm** (dois, para os LEDs)
*   **Servo Motor** (ex: SG90 ou similar)
*   **Fios Jumper** (macho-fêmea e macho-macho)
*   **Protoboard** (opcional, para facilitar as conexões)
*   **Cabo USB** para conectar o Arduino ao computador

## 3. Conexões (Pinout)

O módulo RC522 comunica-se com o Arduino através da interface SPI (Serial Peripheral Interface). Os LEDs e o servo motor são conectados a pinos digitais separados. As conexões são as seguintes:

| Pino RC522 | Pino Arduino Uno | Descrição                               |
| :--------- | :--------------- | :-------------------------------------- |
| VCC        | 3.3V             | Alimentação (3.3V)                      |
| RST        | D9               | Reset                                   |
| GND        | GND              | Terra                                   |
| MISO       | D12              | Master In Slave Out (SPI)               |
| MOSI       | D11              | Master Out Slave In (SPI)               |
| SCK        | D13              | Serial Clock (SPI)                      |
| SDA (SS)   | D10              | Slave Select (SPI)                      |
| **LED Verde** | D7               | Acesso Permitido (com resistor de 220Ω) |
| **LED Vermelho**| D6               | Acesso Negado (com resistor de 220Ω)    |
| **Servo Motor** | D5               | Sinal de controle do Servo Motor        |

**Nota:** As portas SPI no Arduino Uno são fixas: D11 (MOSI), D12 (MISO) e D13 (SCK). Os pinos RST e SDA (SS) podem ser configurados para outras portas digitais, mas D9 e D10 são as convenções mais comuns. Os LEDs devem ser conectados com um resistor de 220 Ohm em série para limitar a corrente e protegê-los. O servo motor geralmente tem três fios: um para alimentação (VCC, geralmente 5V), um para terra (GND) e um para o sinal (conectado ao pino D5 neste caso).

## 4. Instalação da Biblioteca

Para interagir com o módulo RC522 e o servo motor, é necessário instalar as bibliotecas `MFRC522` e `Servo` no seu ambiente de desenvolvimento Arduino IDE. A biblioteca `Servo` geralmente já vem pré-instalada. Siga os passos:

1.  Abra o Arduino IDE.
2.  Vá em `Sketch > Incluir Biblioteca > Gerenciar Bibliotecas...`.
3.  Na caixa de pesquisa, digite `MFRC522`.
4.  Procure pela biblioteca `MFRC522` de `Miguel Balboa` e clique em `Instalar`.
5.  A biblioteca `Servo` já deve estar disponível. Se não estiver, procure por ela e instale.

## 5. Explicação do Código Arduino

O código fornecido (`rfid_reader_code.ino`) realiza a inicialização do módulo RC522, configura os pinos dos LEDs e do servo motor e, em seguida, entra em um loop para detectar, ler o UID de tags RFID e validar o acesso. Abaixo está uma explicação detalhada de cada parte do código:

```arduino
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_VERDE 7 // Pino para o LED verde (acesso permitido)
#define LED_VERMELHO 6 // Pino para o LED vermelho (acesso negado)
#define SERVO_PIN 5 // Pino para o servo motor

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Cria uma instância do objeto MFRC522
Servo myServo; // Cria um objeto servo para controlar o servo motor

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

  myServo.attach(SERVO_PIN); // Anexa o servo ao pino definido
  myServo.write(0); // Garante que o servo comece na posição fechada (0 graus)

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
    myServo.write(180); // Gira o servo para 180 graus (abre)
    delay(3000); // Mantém aberto por 3 segundos
    myServo.write(0); // Gira o servo de volta para 0 graus (fecha)
    digitalWrite(LED_VERDE, LOW);
  } else {
    Serial.println("Acesso NEGADO!");
    digitalWrite(LED_VERMELHO, HIGH);
    delay(2000);
    digitalWrite(LED_VERMELHO, LOW);
    // O servo permanece na posição 0 (fechado)
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
```

*   **`#include <Servo.h>`:** Inclui a biblioteca necessária para controlar o servo motor.
*   **`#define SERVO_PIN 5`:** Define o pino digital do Arduino conectado ao sinal do servo motor.
*   **`Servo myServo;`:** Cria um objeto `Servo` chamado `myServo`.
*   **`void setup()`:**
    *   `myServo.attach(SERVO_PIN);`: Anexa o objeto `myServo` ao pino definido (`SERVO_PIN`), indicando ao Arduino que este pino controlará o servo.
    *   `myServo.write(0);`: Define a posição inicial do servo para 0 graus (posição "fechada").
*   **`void loop()`:**
    *   **Validação de Acesso (Acesso LIBERADO!):**
        *   `myServo.write(180);`: Se o acesso for liberado, o servo gira para 180 graus (posição "aberta").
        *   `delay(3000);`: O servo permanece nesta posição por 3 segundos.
        *   `myServo.write(0);`: Após o atraso, o servo retorna para 0 graus (posição "fechada").
    *   **Validação de Acesso (Acesso NEGADO!):**
        *   O servo motor não é acionado e permanece na posição inicial de 0 graus.

## 6. Como Usar

1.  **Conecte o Hardware:** Siga o diagrama de pinagem na seção 3 para conectar o módulo RC522, os LEDs (com resistores) e o servo motor ao seu Arduino Uno.
2.  **Instale as Bibliotecas:** Certifique-se de que as bibliotecas `MFRC522` e `Servo` estejam instaladas no seu Arduino IDE (seção 4).
3.  **Obtenha o UID do Cartão Mestre:**
    *   Carregue o código `rfid_reader_code.ino` para o seu Arduino (mesmo com o `masterCard` de exemplo).
    *   Abra o Monitor Serial e aproxime o cartão que você deseja que seja o "mestre".
    *   Anote o UID que aparece no Monitor Serial (ex: `04 1A 2B 3C`).
4.  **Atualize o Código com o UID Mestre:** Edite a linha `byte masterCard[] = {0x04, 0x00, 0x00, 0x00};` no código `rfid_reader_code.ino`, substituindo os valores de exemplo pelo UID que você anotou. Por exemplo, se o UID for `04 1A 2B 3C`, a linha deve ser `byte masterCard[] = {0x04, 0x1A, 0x2B, 0x3C};`.
5.  **Carregue o Código Atualizado:** Faça o upload do código modificado para o seu Arduino.
6.  **Abra o Monitor Serial:** Após o upload, abra o Monitor Serial no Arduino IDE e defina a taxa de transmissão para 9600 baud.
7.  **Teste o Acesso:** Aproxime o cartão mestre (LED verde acende e servo gira) e outros cartões (LED vermelho acende e servo não gira) para testar a funcionalidade.

## 7. Considerações Finais

Este projeto serve como um sistema básico de controle de acesso. Para aplicações mais robustas, considere armazenar múltiplos UIDs permitidos (por exemplo, num array ou EEPROM), adicionar um buzzer para feedback sonoro, ou integrar com um sistema de registo de eventos. Lembre-se de que a segurança de sistemas baseados apenas no UID pode ser limitada, e para aplicações críticas, métodos de autenticação mais robustos devem ser considerados.

## 8. Referências

*   [RFID Arduino - Read RFID Tag\'s UID with RC522](https://www.diyengineers.com/2021/04/15/learn-how-to-read-an-rfid-tag-with-rc522-and-arduino/) - DIY Engineers
*   [Interfacing RC522 RFID Module With Arduino](https://www.instructables.com/Interfacing-RC522-RFID-Module-With-Arduino/) - Instructables
*   [Arduino RFID Tutorial: Complete RC522 Guide with Code](https://circuitdigest.com/microcontroller-projects/interfacing-rfid-reader-module-with-arduino) - Circuit Digest
