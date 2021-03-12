// Bibliotecas
#include <Adafruit_Sensor.h>
#include "DHT.h"
// Bibliotecas
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "time.h"

/**PINOS DIGITAIS DE Entradas sensores ********************************************************************************************************/
const int DHTPIN   =  12;                                 // Sensor DHT11 Pode ser usado as
const int Magneto  =  18;
                                                                  
/***INSTANCIANDO OBJETOS***********************************************************************************************************************************/
// Define Modelo do DHT
#define DHTTYPE    DHT22     // DHT 22 

// Define Variáveis do DHT
DHT dht(DHTPIN, DHTTYPE);
float         humidity;     float humidity_atual;
float         temperature;  float temperature_atual;

// Define Variáveis leitura ventoinhas
int           rpm;
volatile byte pulsos;
unsigned long timeold;
float         porcentagem;


//Quantidade de pulsos por volta da ventoinha
unsigned int pulsos_por_volta = 3;

//Variáveis do contador de segundos
unsigned long millisTarefa1 = millis();
int i       = 0;
boolean cont1 = true ;

/*** DEFINE ACESSO A REDE ********************************************************************************************************/
const char* host = "ESP 32";
const char* ssid = "ELETROMIDIA";
const char* password = "s6mdKGD#EO";

int contador_ms = 0;
WebServer server(80);

/*** Configurações do Relógio ********************************************************************************************************/

const char* NTP = "pool.ntp.org";    //Site que busca horários
const long  gmtOffset_sec = -3*3600;

//nao esqueca de ajustar o fuso
const int   daylightOffset_sec = -3600*3;

struct tm timeinfo;

/*** Pagina WEB ********************************************************************************************************/

const char* loginIndex = 
  "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 - identifique-se</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<td>Login:</td>"
        "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Senha:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Identificar'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='Eletromidia' && form.pwd.value==Eletromidia')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Login ou senha inválidos')"
    "}"
    "}"
"</script>";

const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>Progresso: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('Progresso: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('Sucesso!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";
  


/*** SETUP DO PROGRAMA ***********************************************************************************************************************************/
void setup() {
  Serial.begin(115200);

  // Pino que acende a luz e aciona os Relés
  pinMode(2, OUTPUT); 
  pinMode(4, OUTPUT);   

  // Nome da variável de leitura do sensor magnético de abertura de porta
  pinMode(Magneto, INPUT);

  dht.begin();
 
  //Pinos e variáveis da Fan pinos 23, 32, 33, 34, 35, 14, 13, 21, 15, 16, 17
  pinMode(17, INPUT);    
  attachInterrupt(digitalPinToInterrupt(17), contador, RISING);
  pulsos  = 0;
  rpm     = 0;
  timeold = 0;  

  // Inicia o WiFI

  WiFi.begin(ssid, password);  
  server.begin();  

  // Inicia o Relógio

  configTime(gmtOffset_sec, daylightOffset_sec, NTP);
  
} 



/***função piscar LED ***********************************************************************************************************************************/
void Pisca(){

  if (dht.readTemperature()>25){
     
     digitalWrite(4, HIGH);   // Ventoinha 12V ON
     digitalWrite(2, HIGH);   // Ventoinha 220V ON
  }
  
  else{
      digitalWrite(4, LOW);    // Ventoinha 12V OFF
      digitalWrite(2, LOW);    // Ventoinha 220V OFF
  }
}

/*** Função de temperatura e Humidade ***********************************************************************************************************************************/
void TempHumi(){
   // Leitura do DHT ******************************************************************
  humidity =    dht.readHumidity();
  temperature = dht.readTemperature();

  // Atuaização do DHT LCD ***********************************************************
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler DHT!!");
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
    }
}

/*** Função de Contagem de RPM da Fan ***********************************************************************************************************************************/

void contador()
{
  //Incrementa contador
  pulsos++;
}

void Fan(){
  //Atualiza contador a cada segundo
  if (millis() - timeold >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold) * pulsos;
    porcentagem = (pulsos/pulsos_por_volta)*60;
    timeold = millis();
    pulsos = 0;
   
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM = ");
    Serial.println(rpm, DEC);
    //Serial.print("Porcentagem = ");
    //Serial.print(porcentagem);
    //Serial.println("%");
    //Habilita interrupcao
    //attachInterrupt(0, contador, RISING);
  }
}

/*** Função sensor de abertura de Porta***********************************************************************************************************************************/

void  AberturaDePorta(){
  
  int Val = digitalRead(Magneto);
  if( Val == LOW )
  {
    Serial.print("Porta Fechada");
    Serial.println(" deu certo Uhuuuu");
    Serial.println(" ");
  }
  
  if(Val == HIGH)
  {
    Serial.print("Porta Aberta");
    Serial.println(" deu certo Uhuuuu");
    Serial.println(" ");
  }
  
  Serial.println("====================");
  delay(1000);
}

/*** Função do relógio Online ***********************************************************************************************************************************/

void printLocalTime()
{

  if(!getLocalTime(&timeinfo)){
    Serial.println("Falha ao obter a hora");
    return;    
  }
    Serial.println(&timeinfo,"%A,%B %d %Y %H:%M:%S");
}

/*** Função que chama o OTA Local ***********************************************************************************************************************************/

void wifiOTA()
{
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

  if(!MDNS.begin(host))
  {
    Serial.println("Erro ao configurar mDNS. O ESP32 vai reiniciar em 1s...");
    delay(1000);
    ESP.restart();  
  }

  Serial.println("mDNS configurado e incializado");

   // Conectando as páginas
  server.on ("/", HTTP_GET, []()
  {
    server.sendHeader("Conection","close");
    server.send(200, "text/html", loginIndex);
  });

    server.on( "/serverIndex", HTTP_GET,[]()
  {
    server.sendHeader("Conection", "close");
    server.send(200, "text/html", serverIndex);
  });

    server.on("/update", HTTP_POST, []()
    {
      server.sendHeader("Connection","close");
      server.send(200,"text/plain",(Update.hasError()) ? "FAIL" : "OK" );
      ESP.restart();
    },[](){
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START)
    {
      Serial.printf("Update: $\n", upload.filename.c_str());
      if(!Update.begin(UPDATE_SIZE_UNKNOWN))
      Update.printError(Serial);
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize)
      Update.printError(Serial);
    }
    else if(upload.status == UPLOAD_FILE_END)
    {
      if (Update.end(true))
      Serial.printf("Sucesso no Update de firmware: %u\nreiniciando ESP32...\n", upload.totalSize);
      else
        Update.printError(Serial);
    }
   });
}

/*** Função que tem o tempo ***********************************************************************************************************************************/

void Tempo(){

  //int cont = 0;
  //int i = 0;
  // Verifica se já passou 2 segundos
  if(((millis() - millisTarefa1) > 200) && i == 0){
    // Acende o led do pino 7
    //digitalWrite(7, HIGH);
    Serial.println("Acende LED1");
    cont1 = false;
    i = i+1;
    Serial.println(i);
  }

  if(((millis() - millisTarefa1) > 400) && i == 1){
    // Acende o led do pino 7
    //digitalWrite(7, HIGH);
    Serial.println("Acende LED2");
    cont2 = 1;
    i = i+1;
    Serial.println(i);
  }
    if(((millis() - millisTarefa1) > 600) && i == 2){
    // Acende o led do pino 7
    //digitalWrite(7, HIGH);
    Serial.println("Acende LED1");
    cont3 = 1;
    i = i+1;
    Serial.println(i);
  }
    if(((millis() - millisTarefa1) > 800) && i == 3){
    // Acende o led do pino 7
    //digitalWrite(7, HIGH);
    Serial.println("Acende LED1");
    cont4 = 1;
    i = i+1;
    Serial.println(i);
  }
    if(((millis() - millisTarefa1) > 1000) && i == 4){
    // Acende o led do pino 7
    //digitalWrite(7, HIGH);
    Serial.println("Acende LED1");
    cont5 = 1;
    i = i+1;
    Serial.println(i);
  }
    if(((millis() - millisTarefa1) > 1200) && i == 5){
    // Acende o led do pino 7
    //digitalWrite(7, HIGH);
    Serial.println("Acende LED1");
    cont6 = 1;
    i = i+1;
    Serial.println(i);
  }
    if(((millis() - millisTarefa1) > 1400) && i == 6){
    // Acende o led do pino 7
    //digitalWrite(7, HIGH);
    Serial.println("Acende LED1");
    cont7 = 1;
    i = i+1;
    Serial.println(i);
  }
    if(((millis() - millisTarefa1) > 1600) && i == 7){
    // Acende o led do pino 7
    //digitalWrite(7, HIGH);
    Serial.println("Acende LED1");
    cont8 = 1;
    i = i+1;
    Serial.println(i);
  }
    if(((millis() - millisTarefa1) > 1800) && i == 8){
    // Acende o led do pino 7
    //digitalWrite(7, HIGH);
    Serial.println("Acende LED1");
    cont9 = 1;
    i = i+1;
    Serial.println(i);
  }
    if(((millis() - millisTarefa1) > 2000) && i == 9){
    // Acende o led do pino 7
    //digitalWrite(7, HIGH);
    Serial.println("Acende LED1");
    cont10 = 1;
    i = i+1;
    Serial.println(i);
  }
    if(((millis() - millisTarefa1) > 2200) && i == 10){
    // Acende o led do pino 7
    //digitalWrite(7, HIGH);
    Serial.println("Acende LED1");
    cont11 = 1;
    i = i+1;
    Serial.println(i);
  }
  // Verifica se já passou 4 segundos
  if((millis() - millisTarefa1) > 30000){
    millisTarefa1 = millis();
    cont1  = 0;
    i = 0;
  }
}
/*** Função que imprime Resultados ***********************************************************************************************************************************/
void Impressao()
{
  
}



void loop() {
  
/// Função que entra no Wifi e que faz carregamento OTA
   wifiOTA();

/// Função que chama o relógio
   printLocalTime();

/// Função que incia o servidor
   server.handleClient();

///Função que aciona o Relé
   Pisca();

/// Chama as funções em tempos pré determinados
   Impressao();
///////// inicia as demais funções  
   /*if (Func == 0) // Inicio da Função acionamento de Relé
    {
      Pisca();
      Func = Func+1;
      if (Func ==1) // Inicio da Função leitura de temperatura
      {
            TempHumi();
            Func = Func + 1;
            if( Func == 2) // Inicío da Função leitura de RPM
            {
                Fan();
                Func = Func + 1;
                if( Func == 3) // Inicio do Sensor que idêntifica a abertura de prota
                {
                   AberturaDePorta();
                   Func = 0;
                }               
            }
      }
    }*/

    delay(1);
}
