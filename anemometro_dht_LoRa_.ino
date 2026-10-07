#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>

// Definição dos pinos do ESP32-S3 conectados ao LoRa
#define SCK     12
#define MISO    13
#define MOSI    11
#define SS      10
#define RST     9
#define DIO0    14

#define DHTPIN 18
#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE);

//int contadorDePacotes = 0; // Variável para contar os pacotes enviados

// --- Constantes ---
const float pi = 3.14159265;     //Número de pi
int period = 5000;               //Tempo de medida(miliseconds)
int delaytime = 2000;            //Invervalo entre as amostras (miliseconds)
int radius = 147;                //Raio do anemometro(mm)
const int pinSensor = 2;         // Pino do sensor no ESP32-S3
const float mps_to_mph = 2.23694; // 1 m/s = 2.23694 mph


// --- Variáveis Globais ---
volatile unsigned int counter = 0;        //Contador para o sensor  
unsigned int RPM = 0;            //Rotações por minuto
float speedwind = 0;             //Velocidade do vento (m/s)
float windspeed = 0;             //Velocidade do vento (km/h)
float speedwindM = 0;             //Velocidade do vento (milhas/h)


// Função de interrupção (precisa estar na memória RAM)
void IRAM_ATTR addcount() {
  counter++;
}

// --- Configurações Inicias ---
void setup()
{
   dht.begin();
 // Configura o pino com resistor de pull-up interno do ESP32
  pinMode(pinSensor, INPUT_PULLUP); 
    
   
  Serial.begin(115200);       //inicia serial em 115200 baud rate
  while (!Serial); // Aguarda a porta serial abrir

  Serial.println("Inicializando Teste de Transmissão LoRa...");

  // Configura os pinos SPI personalizados do ESP32-S3
  SPI.begin(SCK, MISO, MOSI, SS);
  
  // Informa a biblioteca LoRa sobre os pinos de controle
  LoRa.setPins(SS, RST, DIO0);

  // Inicializa o rádio em 915 MHz (Frequência comum no Brasil)
  if (!LoRa.begin(915E6)) {
    Serial.println("Falha ao iniciar o módulo LoRa. Verifique as conexões!");
    while (1); // Trava o código aqui se der erro
  }
  
  // (Opcional) Configura a potência de saída (de 2 a 20 dBm)
  LoRa.setTxPower(14); 
  
  Serial.println("LoRa iniciado com sucesso! Começando a transmitir...\n");
 
} //end setup


// --- Loop Infinito ---
void loop(){
  
  
  windvelocity();
  RPMcalc();
  SpeedWind();
  String dados = "&KmVelocity=" + (String)speedwind + "&temperatureC=" + (String)dht.readTemperature() + "&humidity=" + (String)dht.readHumidity();


//Transmissão via rede LoRa
  // Inicia a montagem do pacote de dados
  LoRa.beginPacket();
  
  // Envia a mensagem (pode ser texto, números, variáveis...)
  LoRa.print(dados);
  
  LoRa.endPacket();

  //contadorDePacotes++; // Incrementa o contador

  delay(delaytime);                        //taxa de atualização
  
}



//Função para medir velocidade do vento
void windvelocity()
{
  speedwind = 0;
  windspeed = 0;
  speedwindM = 0;

  
  
  counter = 0;  
  attachInterrupt(digitalPinToInterrupt(pinSensor), addcount, RISING);
  unsigned long startTime = millis();
  // Loop de espera liberando o processador (evita pânico do Watchdog)
  while(millis() < startTime + period) {
    yield(); 
  }
  
  // Desativa a interrupção para processar os dados com segurança
  detachInterrupt(digitalPinToInterrupt(pinSensor));
}


//Função para calcular o RPM
void RPMcalc()
{
  RPM=((counter)*60)/(period/1000);  // Calculate revolutions per minute (RPM)
}


//Velocidade do vento em m/s
void WindSpeed()
{
  windspeed = ((4 * pi * radius * RPM)/60) / 1000;  //Calcula a velocidade do vento em m/s
 
} //end WindSpeed


//Velocidade do vento em km/h
void SpeedWind()
{
  speedwind = (((4 * pi * radius * RPM)/60) / 1000)*3.6;  //Calcula velocidade do vento em km/h
 
} //end SpeedWind

//Velocidade do vento em milhas/h
void SpeedWindM()
{
  speedwindM = (((4 * pi * radius * RPM)/60) / 1000)*mps_to_mph;  //Calcula velocidade do vento em milhas/h
 
} //end SpeedWind
