#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>   
#include <HTTPClient.h>


// Mesma pinagem configurada no transmissor
#define SCK     12
#define MISO    13
#define MOSI    11
#define SS      10
#define RST     9
#define DIO0    14

const char* ssid = "1REDLAB 2844";
const char* password = "RedLabNet";

int contadorDePacotes = 0;

void setup() {
  Serial.begin(115200);

  while (!Serial);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  //Serial.println("Inicializando Teste de Recepção LoRa...");

  // Configura os pinos SPI personalizados do ESP32-S3
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  // Inicializa o rádio na mesma frequência do transmissor (915 MHz)
  if (!LoRa.begin(915E6)) {
    Serial.println("Falha ao iniciar o módulo LoRa. Verifique as conexões!");
    while (1);
  }
  
  Serial.println("LoRa Rx iniciado com sucesso! Aguardando mensagens...\n");
}

void loop() {
  // Verifica se um novo pacote chegou
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    //Pacote recebido
   // Serial.print("Pacote recebido: '");
    
    // Lê e imprime o conteúdo do pacote caractere por caractere
    //while (LoRa.available()) {
    //  Serial.print((char)LoRa.read());
    //}
    
    //Imprime o RSSI (Received Signal Strength Indicator)
    Serial.print(" | RSSI: ");
    Serial.println(LoRa.packetRssi());
    String pacote = LoRa.readString();
    String msg = "id="+ (String)contadorDePacotes + msg;
    msg.trim();

  //Envia os dados para a planilha
  String dados = "https://script.google.com/macros/s/AKfycbztv5itZEObHDr_4SUg-pufd0SnpCO6dgTg-PNIYBYyT-fTQtY4HKrrGAO0_yOcTSkmLA/exec?" + msg;
  PostMessage(dados);
  contadorDePacotes++;
  delay(10000);
  }
  
}



void PostMessage(String msg){
  HTTPClient http;

  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.begin(msg);


  http.addHeader("Content-Type", "application/json");
    // Send HTTP POST request
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
    //Serial.print(msg);
  }
  else{
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }


  // Free resources
  http.end();
}
