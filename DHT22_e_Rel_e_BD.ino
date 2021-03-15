// INCLUSÃO DE BIBLIOTECAS
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "time.h"

//#include "arduino_secrets.h"

// DEFINIÇÕES
#define LED 2

unsigned long millisTarefa1 = millis();
int i       = 0;

////// Parte do sensor de Temperatura
const int DHTPIN   =              12;   // Pino que lê a temperatura
#define DHTTYPE    DHT22     // DHT 22 

///////////////// DECLARAÇÃO DE VARIÁVEIS GLOBAIS //////////////////////////////////////////
int Contador = 0;
///////////////////////// Define Variáveis do DHT
DHT dht(DHTPIN, DHTTYPE);
float humidity;     float humidity_atual;
float temperature;  float temperature_atual;


// DECLARAÇÃO DE VARIÁVEIS PARA WIFI
char ssid[] = "Pedro";        // your network SSID (name)
char pass[] = "funciona";        // your network password
//int status = WL_IDLE_STATUS;      // status

/*// DECLARAÇÃO DE VARIÁVEIS PARA MySQL
char user[] = "perucelo";              // MySQL user login username
char password[] = "eletromidia";          // MySQL user login password*/

/// Clever Cloud
IPAddress server_addr(185, 42, 117, 115);  // IP of the MySQL *server* here
char user[] = "u4gnbqjncr8kcznp";              // MySQL user login username
char password[] = "clFpGRTokoJfirSUAj9f";          // MySQL user login password

char INSERT_SQL[] = "INSERT INTO bzayjfy8zw7gkcajc7aj.Temperatura (ID_SENSOR, TEMPERATURA, Porta_um, Porta_dois,RPM_um,RPM_dois,RPM_tres,RPM_quatro,RPM_cinco,RPM_seis,RPM_sete,RPM_oito,RPM_nove,RPM_dez,RPM_onze) VALUES ('%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d')";
char query[128];

// INSTANCIANDO OBJETOS
WiFiClient client;
MySQL_Connection conn((Client *)&client);

// ***************** INÍCIO DO SETUP *************************
void setup() {
  Serial.begin(115200);
  Serial.println("Passou aqui");
  pinMode(LED, OUTPUT);
  Serial.println("Passou aqui  2");
  digitalWrite(LED, LOW);
  Serial.println("Passou aqui  3");
  dht.begin();
  Serial.println("Passou aqui  4");
  while (!Serial) {
    ; // ESPERA O SERIAL ESTAR PRONTO
  }
  Serial.println("Passou aqui  5");
  WiFi.begin(ssid, pass); 
  Serial.println("Passou aqui  6");
   while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Conectado a rede wi-fi");
  Serial.println(ssid);
  Serial.print("IP obtido: ");
  Serial.println(WiFi.localIP());
}
// ***************** FIM DO SETUP ***************************

// ***************** INÍCIO DO LOOP *************************
void loop() {
      
    if(((millis() - millisTarefa1) > 200) && i == 0){
    Serial.println("Passou aqui  7");
    enviaDados();
    Serial.println("Passou aqui  8");
    i = i+1;
    Serial.println(i);
  }

  if((millis() - millisTarefa1) > 10000){
    millisTarefa1 = millis();
    i = 0;
  }
    

   // delay(10000);

     // Leitura do DHT ******************************************************************
 // humidity =    dht.readHumidity();
 // temperature = dht.readTemperature();

  // Atuaização do DHT LCD ***********************************************************
 /* if (isnan(humidity) || isnan(temperature)) {
   humidity_atual = 0;  temperature_atual = 0;
      Serial.print("Temp.");  
      Serial.print("Deu Ruim");
      Serial.print("°");
      Serial.println("C");

      Serial.print("Umid.");
      Serial.print("Deu Ruim");
      Serial.println("%");
      Serial.println("====================");
   
  }
  else if (humidity_atual != humidity || temperature_atual != temperature) {
      humidity_atual = humidity;  temperature_atual = temperature;
      Serial.print("Temp.");  
      Serial.print(temperature);
      Serial.print("°");
      Serial.println("C");

      Serial.print("Umid.");
      Serial.print(humidity);
      Serial.println("%");
      Serial.println("====================");
    }*/
}
// ***************** FIM DO LOOP ***************************
  

void enviaDados() {
 
  MySQL_Cursor *cur = new MySQL_Cursor(&conn);

 // delay(100); 
  if (conn.connected()) {
    digitalWrite(LED, HIGH);
    sprintf(query, INSERT_SQL,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
    Serial.println("Enviado");
    ///////////////////// Parte do Código que lê temperatura////////////
   // temperature = dht.readTemperature();
    ////////////////////////////////////////////////////////////////////
    cur->execute(query); 
    delete cur; 
  } else {
    conn.close();
    digitalWrite(LED, LOW);
    Serial.println("Connecting...");
    //delay(200); //
    if (conn.connect(server_addr,3306,user,password)) {
      //delay(500);
      Serial.println("Successful reconnect!");
      digitalWrite(LED, HIGH);
    } else {
      Serial.println("Cannot reconnect! Drat.");
      digitalWrite(LED, LOW);
      ESP.restart();
    }
  }
}
