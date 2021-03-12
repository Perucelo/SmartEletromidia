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
int           rpm, rpm2, rpm3, rpm4, rpm5, rpm6, rpm7, rpm8, rpm9, rpm10, rpm11;
volatile byte pulsos, pulsos2, pulsos3, pulsos4, pulsos5, pulsos6, pulsos7, pulsos8, pulsos9, pulsos10, pulsos11;
unsigned long timeold, timeold2, timeold3, timeold4, timeold5, timeold6, timeold7, timeold8, timeold9, timeold10, timeold11;
float         porcentagem, porcentagem2, porcentagem3, porcentagem4, porcentagem5, porcentagem6, porcentagem7, porcentagem8, porcentagem9, porcentagem10, porcentagem11;

//Quantidade de pulsos por volta da ventoinha
unsigned int pulsos_por_volta = 3;

//Variáveis do contador de segundos
unsigned long millisTarefa1 = millis();
int i       = 0;
boolean cont1 = true ;
boolean cont2 = true ;
boolean cont3 = true ;

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
  pinMode(23, INPUT);
  pinMode(32, INPUT);
  pinMode(33, INPUT);
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(14, INPUT);
  pinMode(13, INPUT);
  pinMode(21, INPUT);
  pinMode(15, INPUT);
  pinMode(16, INPUT);
  pinMode(17, INPUT); 
//Interrupcao 0 - pino digital 2
//Aciona o contador a cada pulso
  attachInterrupt(digitalPinToInterrupt(23), contador, RISING);
  attachInterrupt(digitalPinToInterrupt(32), contador2, RISING);
  attachInterrupt(digitalPinToInterrupt(33), contador3, RISING);
  attachInterrupt(digitalPinToInterrupt(34), contador4, RISING);
  attachInterrupt(digitalPinToInterrupt(35), contador5, RISING);
  attachInterrupt(digitalPinToInterrupt(14), contador6, RISING);
  attachInterrupt(digitalPinToInterrupt(13), contador7, RISING);
  attachInterrupt(digitalPinToInterrupt(21), contador8, RISING);
  attachInterrupt(digitalPinToInterrupt(15), contador9, RISING);
  attachInterrupt(digitalPinToInterrupt(16), contador10, RISING);
  attachInterrupt(digitalPinToInterrupt(17), contador11, RISING);
  rpm, rpm2, rpm3, rpm4, rpm5, rpm6, rpm7, rpm8, rpm9, rpm10, rpm11                                             = 0;
  pulsos, pulsos2, pulsos3, pulsos4, pulsos5, pulsos6, pulsos7, pulsos8, pulsos9, pulsos10, pulsos11            = 0;
  timeold, timeold2, timeold3, timeold4, timeold5, timeold6, timeold7, timeold8, timeold9, timeold10, timeold11 = 0;
  
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

// ------------------------- Interrupção -----------------------------------
///////////// Ventoinha 1/////////////////////
void contador()
{
  //Incrementa contador
  pulsos++;
}

///////////// Ventoinha 2/////////////////////
void contador2()
{
  //Incrementa contador
  pulsos2++;
}

///////////// Ventoinha 3/////////////////////
void contador3()
{
  //Incrementa contador
  pulsos3++;
}

///////////// Ventoinha 4/////////////////////
void contador4()
{
  //Incrementa contador
  pulsos4++;
}
///////////// Ventoinha 5/////////////////////
void contador5()
{
  //Incrementa contador
  pulsos5++;
}

///////////// Ventoinha 6/////////////////////
void contador6()
{
  //Incrementa contador
  pulsos6++;
}

///////////// Ventoinha 7/////////////////////
void contador7()
{
  //Incrementa contador
  pulsos7++;
}

///////////// Ventoinha 8/////////////////////
void contador8()
{
  //Incrementa contador
  pulsos8++;
}

///////////// Ventoinha 9/////////////////////
void contador9()
{
  //Incrementa contador
  pulsos9++;
}

///////////// Ventoinha 10/////////////////////
void contador10()
{
  //Incrementa contador
  pulsos10++;
}

///////////// Ventoinha 11/////////////////////
void contador11()
{
  //Incrementa contador
  pulsos11++;
}
// ========================================================================================================
void Fan(){
 /////////////////// VENTOINHA 1 ///////////////////////////////////////////////////
  if (millis() - timeold >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold) * pulsos;
    timeold = millis();
    pulsos = 0;
   
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM1 = ");
    Serial.println(rpm, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador, RISING);
  }
  
///////////////////// VENTOINHA 2 //////////////////////////////////////////////  

  if (millis() - timeold2 >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm2 = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold2) * pulsos2;
    timeold2 = millis();
    pulsos2 = 0;
   
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM2 = ");
    Serial.println(rpm2, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador2, RISING);
  }

///////////////////// VENTOINHA 3 //////////////////////////////////////////////  

  if (millis() - timeold3 >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm3 = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold3) * pulsos3;
    porcentagem3 = (pulsos/pulsos_por_volta)*60;
    timeold3 = millis();
    pulsos3 = 0;
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM3 = ");
    Serial.println(rpm3, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador3, RISING);
  }

///////////////////// VENTOINHA 4 //////////////////////////////////////////////  

  if (millis() - timeold4 >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm4 = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold4) * pulsos4;
    timeold4 = millis();
    pulsos4 = 0;
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM4 = ");
    Serial.println(rpm4, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador4, RISING);
  }

///////////////////// VENTOINHA 5 //////////////////////////////////////////////  

  if (millis() - timeold5 >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm5 = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold5) * pulsos5;
    timeold5 = millis();
    pulsos5 = 0;
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM5 = ");
    Serial.println(rpm5, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador5, RISING);
  }

///////////////////// VENTOINHA 6 //////////////////////////////////////////////  

  if (millis() - timeold6 >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm6 = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold6) * pulsos6;
    timeold6 = millis();
    pulsos6 = 0;
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM6 = ");
    Serial.println(rpm6, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador6, RISING);
  }

///////////////////// VENTOINHA 7 //////////////////////////////////////////////  

  if (millis() - timeold7 >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm7 = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold7) * pulsos7;
    timeold7 = millis();
    pulsos7 = 0;
   
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM7 = ");
    Serial.println(rpm7, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador7, RISING);
  }

///////////////////// VENTOINHA 8 //////////////////////////////////////////////  

  if (millis() - timeold8 >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm8 = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold8) * pulsos8;
    timeold8 = millis();
    pulsos8 = 0;
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM8 = ");
    Serial.println(rpm8, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador8, RISING);
  }

///////////////////// VENTOINHA 9 //////////////////////////////////////////////  

  if (millis() - timeold9 >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm9 = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold9) * pulsos9;
    timeold9 = millis();
    pulsos9 = 0;
   
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM9 = ");
    Serial.println(rpm9, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador9, RISING);
  }

///////////////////// VENTOINHA 10 //////////////////////////////////////////////  

  if (millis() - timeold10 >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm10 = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold10) * pulsos10;
    timeold10 = millis();
    pulsos10 = 0;
   
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM10 = ");
    Serial.println(rpm10, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador10, RISING);
  }

///////////////////// VENTOINHA 11 //////////////////////////////////////////////  

  if (millis() - timeold11 >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm11 = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold11) * pulsos11;
    timeold11 = millis();
    pulsos11 = 0;
   
    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM11 = ");
    Serial.println(rpm11, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador11, RISING);
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
    TempHumi();
    Serial.println("Acende LED1");
    cont1 = false;
    i = i+1;
    Serial.println(i);
  }

  if(((millis() - millisTarefa1) > 400) && i == 1){
    Fan();
    Serial.println("Acende LED2");
    cont2 = false;
    i = i+1;
    Serial.println(i);
  }

   if(((millis() - millisTarefa1) > 400) && i == 1){
    AberturaDePorta();
    Serial.println("Acende LED2");
    cont3 = false;
    i = i+1;
    Serial.println(i);
  }

  if((millis() - millisTarefa1) > 30000){
    millisTarefa1 = millis();
    i       = 0;
    cont1 = true ;
    cont2 = true ;
    cont3 = true ;
  }

}
/*** Função que imprime Resultados ***********************************************************************************************************************************/


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
   Tempo();

   delay(1);
}
