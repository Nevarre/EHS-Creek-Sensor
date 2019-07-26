#include <SoftwareSerial.h>
#include <String.h>
SoftwareSerial myGsm(7,8);//initiate communication with GSM card using TX-RX
float temp=19;//temperature
float pH= 5;//pH
//int tempPin = 0;
//boolean state, lastState;
int temptrig;//trigger when temperature crosses limit value
int pHtrig;//trigger when pH crosses limit value

char sms_num[25] = "AT+CMGS=\"+xxxxxxxxxx\"";
char thingspeak_key[72]= "GET https://api.thingspeak.com/update?api_key=***KEY***=0";

void setup()
{
if (temp>20)//limit value condition for temperature
{
  temptrig=1;// temperature trigger set
  }
  if (temptrig==1)
  {
  myGsm.begin(9600);
  Serial.begin(9600);//sets baud rate = 9600 for serial communication
  myGsm.println("AT+CMGF=1");//select SMS mode, 0 means PDU mode, 1 means text, always set to 1
  delay(1000);
  }

 if (pH<5)//limit value condition for pH
{
  pHtrig=1;
  }
  if (pHtrig==1)// pH trigger set
  {
  myGsm.begin(9600);
  Serial.begin(9600);//sets baud rate = 9600 for serial communication
  myGsm.println("AT+CMGF=1");//select SMS mode, 0 means PDU mode, 1 means text, always set to 1
  delay(1000);
  }

  
  delay(500);
  
 myGsm.begin(9600);  
 Serial.begin(9600);  
 delay(500);
 
myGsm.println("AT");//AT commands, AT means Attention
 delay(1000);
 printSerialData();

myGsm.println("AT+CPIN?");//Enter PIN, remove all PINs from SIM card e.g. SIM PIN/PUK
 delay(1000);
 printSerialData();

 myGsm.println("AT+CREG?");// Network registration
 delay(1000);
 printSerialData();

 myGsm.println("AT+CGATT?");// Attach or detach to GPRS service
 delay(1000);
 printSerialData();
 
myGsm.println("AT+CIPSHUT");// Deactivate GPRS PDP context
 delay(1000);
 printSerialData();

myGsm.println("AT+CIPSTATUS");// Query current connection status
 delay(1000);
 printSerialData();


 myGsm.println("AT+CIPMUX=0");//Start Up multi IP connection
 delay(2000);
 printSerialData();


 myGsm.println("AT+CGATT=1");//CGATT=1 means GPRS is attached
 delay(2000);
 printSerialData();
 
  
 myGsm.println("AT+CSTT=\"Ultra\",\"\",\"\"");//setting the APN,2nd parameter empty works for all networks, Ultra is the VPN name setted up for SIM card, it is reconfigurable
 delay(5000);
 printSerialData();
 
 myGsm.println();
 myGsm.println("AT+CIICR");// Bring UP wireless connection with GPRS
 delay(6000);
 printSerialData();
 
 myGsm.println("AT+CIFSR"); //init the HTTP request, get local IP address
 delay(2000); 
 printSerialData();
 
 myGsm.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");// Start Up TCP/UDP connection
 delay(5000);
 printSerialData();
 delay(5000);
 
 myGsm.println("AT+CIPSEND");// Send data through TCP/UDP connection
 delay(2000);
 printSerialData();

String data= thingspeak_key + String(temp)+"&field2=" +String(pH);//Send data to ThingSpeak.com, api_key is obtained from ThingSpeak.com, it is must to send data in string format, e.g. String(temp)
 myGsm.println(data);
 delay(10000);
 printSerialData();



 myGsm.println((char)26);//This signifies end of sending data
 delay(10000);
 printSerialData();


myGsm.println("AT+CIPSHUT");// Deactivate GPRS PDP context
delay(3000);
printSerialData();

  if (temptrig==1 ) {
    sendTempSMS();//Call SMS function when temperature limit is crossed
  }
  
  delay(500);

  if (pHtrig==1 ) {
    sendpHSMS();//Call SMS function when pH limit is crossed
  }
  
}

void loop()
{
    while(myGsm.available()) {
    Serial.write(myGsm.read());
  }


  
  /*if (temptrig==1 ) {
    sendTempSMS();//Call SMS function when temperature limit is crossed
  }
  
  delay(500);

  if (pHtrig==1 ) {
    sendpHSMS();//Call SMS function when pH limit is crossed
  }
  
  delay(500);*/
}


void sendTempSMS() {
  Serial.print("Temperature limit crossed");//message shown in serial monitor of Arduino
  myGsm.println(sms_num);//send SMS, you can change the number, adding country code is a must
  delay(500);

  myGsm.print("Temperature limit crossed ");//message to be sent
  myGsm.write( 0x1a ); // ctrl+Z character, necessary command after sending an SMS
  delay(500);
}

void sendpHSMS() {
  Serial.print("pH limit crossed");//message shown in serial monitor of Arduino
  myGsm.println(sms_num);//send SMS, you can change the number, adding country code is a must
  delay(500);

  myGsm.print("pH limit crossed ");//message to be sent
  myGsm.write( 0x1a ); // ctrl+Z character, necessary command after sending an SMS
  delay(500);
}

void printSerialData()
{
 while(myGsm.available()!=0)
 Serial.write(myGsm.read());
}
