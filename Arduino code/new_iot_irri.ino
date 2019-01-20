#include<SoftwareSerial.h>
#include <SPI.h>
#include<dht.h>
#define DEBUG "TRUE"
#define dht_apin A0 // Analog Pin to which the sensor is connected

const int sensor_pin = A1;                        /* Soil moisture sensor O/P pin */
int buzz=8;                                                  /* Buzzer Pin */
const int MOISTURE_LEVEL = 400;        /* Moisture level below which buzzer high*/


SoftwareSerial wifi(2,3); //rx, tx

dht DHT;

void setup() 
{
  
  Serial.begin(9600);
  pinMode(buzz,OUTPUT);// Initialize serial communications with the PC
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  Serial.println("Wifi Started");
  wifi.begin(9600);
  sendData("AT+RST\r\n",3000,DEBUG);
  sendData("AT+CIFSR\r\n",1000,DEBUG);
  
}

void loop()
{
  DHT.read11(dht_apin);
  int a=(int)DHT.temperature;
  int b=(int)DHT.humidity; 
  float c=soil();
  iot(a,b,c);
  delay(5000);
  
  
}
int soil()
{
  float moisture_percentage;
  int sensor_analog;
  sensor_analog = analogRead(sensor_pin);                 /* Read soil sensor value*/
  moisture_percentage = (100-((sensor_analog/1023.00)*100));     /* Calculate moisture %*/
  if(sensor_analog < MOISTURE_LEVEL)
  {                        /* Buzzer HIGH */
    digitalWrite(buzz,LOW);
  }
   
  else
  {                                                           /*Buzzer LOW*/
    digitalWrite(buzz,HIGH);   
  }
  return moisture_percentage;

}
void iot(int t,int h, float s) 
{
      sendData("AT+CIPMUX=1\r\n",100,DEBUG); //set multiplexer to multiple connections
      
      sendData("AT+CIPSTART=1,\"TCP\",\"192.168.43.37\",80\r\n",150,DEBUG); // set the host website while mentioning the host and port number and the connection number
      
      String cipstart="GET /web/increment.php?temperature=";
      cipstart+=(String)t;
      cipstart+="&humidity=";
      cipstart+=(String)h;
      cipstart+="&soil=";
      cipstart+=(String)s;
      cipstart+=" HTTP/1.1\r\n"; //GET request in a String containing the information of the RFID card tapped
      
      String HOST="HOST: 192.168.43.37\r\n"; //mentioning the host address in the massage to be sent
      
      String conn="CONNECTION: keep-alive\r\n\r\n\r\n";
      int l=cipstart.length()+HOST.length()+conn.length(); //finding the total length of the massage to be sent
      
      String cipsend="AT+CIPSEND=1,";
      cipsend+=(String)l;
      cipsend+="\r\n"; //Sending information about bytes to be sent over the connection
      
      sendData(cipsend,100,DEBUG);
      
      sendData(cipstart,100,DEBUG);
      
      sendData(HOST,100,DEBUG);
      
      sendData(conn,100,DEBUG);//self-explanatory, sending the above string to the esp through the function sendData(command,timeout,DEBUG);
      
      sendData("AT+CIPCLOSE=1\r\n",500,DEBUG); //closing the connection number 1;
  
}

String sendData(String command, const int timeout, boolean debug)
{
  String response="";
  wifi.print(command);
  delay(timeout);
  while(wifi.available())
  {
    char c=wifi.read();
    response+=c;
  }
  if(debug)
  {
    Serial.println(response);
  }
  
  return response;
}

