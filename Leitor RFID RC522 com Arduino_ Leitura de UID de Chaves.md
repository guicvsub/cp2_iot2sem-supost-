# Leitor RFID RC522 com Arduino: Leitura de UID de Chaves

## 1. Introdução

Este documento fornece um guia completo para utilizar o módulo leitor RFID RC522 com um Arduino, focando na leitura do UID (Unique Identifier) de cartões e tags RFID. O RFID (Radio-Frequency Identification) é uma tecnologia que utiliza campos eletromagnéticos para identificar automaticamente e rastrear tags anexadas a objetos. O módulo RC522 é um leitor/gravador de RFID de baixo custo e amplamente utilizado em projetos com microcontroladores como o Arduino.

O objetivo deste projeto é demonstrar como configurar o hardware e o software para ler o UID de qualquer tag RFID compatível com o padrão MIFARE Classic, que é o tipo de tag mais comum utilizado com o RC522.

## 2. Componentes Necessários

Para replicar este projeto, você precisará dos seguintes componentes:

*   **Arduino Uno** (ou qualquer outro modelo compatível)
*   **Módulo Leitor RFID RC522**
*   **Cartão/Tag RFID** (geralmente acompanha o módulo RC522)
*   **Fios Jumper** (macho-fêmea)
*   **Protoboard** (opcional, para facilitar as conexões)
*   **Cabo USB** para conectar o Arduino ao computador

## 3. Conexões (Pinout)

O módulo RC522 comunica-se com o Arduino através da interface SPI (Serial Peripheral Interface). As conexões são as seguintes:

| Pino RC522 | Pino Arduino Uno | Descrição                               |
| :--------- | :--------------- | :-------------------------------------- |
| VCC        | 3.3V             | Alimentação (3.3V)                      |
| RST        | D9               | Reset                                   |
| GND        | GND              | Terra                                   |
| MISO       | D12              | Master In Slave Out (SPI)               |
| MOSI       | D11              | Master Out Slave In (SPI)               |
| SCK        | D13              | Serial Clock (SPI)                      |
| SDA (SS)   | D10              | Slave Select (SPI)                      |

**Nota:** As portas SPI no Arduino Uno são fixas: D11 (MOSI), D12 (MISO) e D13 (SCK). Os pinos RST e SDA (SS) podem ser configurados para outras portas digitais, mas D9 e D10 são as convenções mais comuns.

## 4. Instalação da Biblioteca

Para interagir com o módulo RC522, é necessário instalar a biblioteca `MFRC522` no seu ambiente de desenvolvimento Arduino IDE. Siga os passos:

1.  Abra o Arduino IDE.
2.  Vá em `Sketch > Incluir Biblioteca > Gerenciar Bibliotecas...`.
3.  Na caixa de pesquisa, digite `MFRC522`.
4.  Procure pela biblioteca `MFRC522` de `Miguel Balboa` e clique em `Instalar`.

## 5. Explicação do Código Arduino

O código fornecido (`rfid_reader_code.ino`) realiza a inicialização do módulo RC522 e, em seguida, entra em um loop para detectar e ler o UID de tags RFID. Abaixo está uma explicação detalhada de cada parte do código:

```arduino
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Wait for serial port to connect. Needed for native USB port only
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  Serial.println("Aproxime o seu cartão RFID do leitor...");
  Serial.println();
}

void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Show some details of the PICC (that is: the tag/card)
  Serial.print("UID do cartão: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Halt PICC
  mfrc522.PICC_HaltA();

  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}
```

*   **`#include <SPI.h>` e `#include <MFRC522.h>`:** Incluem as bibliotecas necessárias. `SPI.h` é para a comunicação SPI, e `MFRC522.h` é a biblioteca específica para o módulo RFID.
*   **`#define SS_PIN 10` e `#define RST_PIN 9`:** Definem os pinos digitais do Arduino conectados aos pinos SDA (SS) e RST do módulo RC522, respectivamente.
*   **`MFRC522 mfrc522(SS_PIN, RST_PIN);`:** Cria uma instância do objeto `MFRC522`, passando os pinos SS e RST como parâmetros.
*   **`void setup()`:**
    *   `Serial.begin(9600);`: Inicia a comunicação serial com o computador a uma taxa de 9600 bits por segundo, permitindo visualizar as leituras no Monitor Serial do Arduino IDE.
    *   `while (!Serial);`: Garante que a comunicação serial esteja pronta antes de prosseguir (útil para placas com porta USB nativa).
    *   `SPI.begin();`: Inicializa o barramento SPI.
    *   `mfrc522.PCD_Init();`: Inicializa o módulo RC522.
    *   `Serial.println(...)`: Mensagens de instrução para o utilizador.
*   **`void loop()`:**
    *   `if ( ! mfrc522.PICC_IsNewCardPresent()) { return; }`: Verifica se uma nova tag RFID está presente no campo do leitor. Se não houver, a função retorna e o loop continua.
    *   `if ( ! mfrc522.PICC_ReadCardSerial()) { return; }`: Se uma nova tag for detectada, tenta ler o seu número de série (UID). Se a leitura falhar, a função retorna.
    *   **Leitura e Impressão do UID:**
        *   `Serial.print("UID do cartão: ");`: Imprime um rótulo no Monitor Serial.
        *   O loop `for` itera sobre os bytes do UID (`mfrc522.uid.uidByte`) e os imprime em formato hexadecimal. A condição `mfrc522.uid.uidByte[i] < 0x10 ? 
" 0" : " ")` adiciona um zero à esquerda para bytes menores que 0x10, garantindo que cada byte seja exibido com dois dígitos hexadecimais para melhor legibilidade.
    *   `Serial.println();`: Adiciona uma nova linha após a impressão do UID.
    *   `mfrc522.PICC_HaltA();`: Coloca a tag RFID em estado de 
parada para que não seja lida novamente na mesma iteração do loop.
    *   `mfrc522.PCD_StopCrypto1();`: Interrompe a criptografia no leitor, se estiver ativa.

## 6. Como Usar

1.  **Conecte o Hardware:** Siga o diagrama de pinagem na seção 3 para conectar o módulo RC522 ao seu Arduino Uno.
2.  **Instale a Biblioteca:** Certifique-se de que a biblioteca `MFRC522` esteja instalada no seu Arduino IDE (seção 4).
3.  **Carregue o Código:** Abra o arquivo `rfid_reader_code.ino` no Arduino IDE, selecione a placa e a porta serial corretas e faça o upload do código para o seu Arduino.
4.  **Abra o Monitor Serial:** Após o upload, abra o Monitor Serial no Arduino IDE (Ferramentas > Monitor Serial) e defina a taxa de transmissão para 9600 baud.
5.  **Leia as Tags:** Aproxime um cartão ou tag RFID do módulo RC522. O UID da tag será exibido no Monitor Serial.

## 7. Considerações Finais

Este projeto serve como um ponto de partida para o uso do módulo RFID RC522 com Arduino. A partir da leitura do UID, é possível desenvolver diversas aplicações, como sistemas de controle de acesso, registo de presenças, ou até mesmo jogos interativos. Lembre-se de que a segurança de sistemas baseados apenas no UID pode ser limitada, e para aplicações críticas, métodos de autenticação mais robustos devem ser considerados.

## 8. Referências

*   [RFID Arduino - Read RFID Tag's UID with RC522](https://www.diyengineers.com/2021/04/15/learn-how-to-read-an-rfid-tag-with-rc522-and-arduino/) - DIY Engineers
*   [Interfacing RC522 RFID Module With Arduino](https://www.instructables.com/Interfacing-RC522-RFID-Module-With-Arduino/) - Instructables
*   [Arduino RFID Tutorial: Complete RC522 Guide with Code](https://circuitdigest.com/microcontroller-projects/interfacing-rfid-reader-module-with-arduino) - Circuit Digest
