#include <SoftwareSerial.h>
#include <String.h>
SoftwareSerial myGsm(7,8);//initiate communication with GSM card using TX-RX
float temp=19;//temperature
float pH= 5;//pH
//int tempPin = 0;
//boolean state, lastState;
int temptrig;//trigger when temperature crosses limit value
int pHtrig;//trigger when pH crosses limit value

// measurement parameters
char temp_str[12] = "temperature";
char pH_str[3] = "pH";

char sms_num[25] = "AT+CMGS=\"+1xxxxxxxxxx\""; // phone number used for sms, must include country code
char thingspeak_key[72]= "GET https://api.thingspeak.com/update?api_key=***KEY***=0";

void(* resetFunc) (void) = 0; // resets the Arduino

void setup(){
    Serial.begin(9600); //Begin serial communication with Arduino and Arduino IDE
    Serial.println("Initializing..."); 
    delay(1000);

    updateSerial();

    if (temp>20){ //limit value condition for temperature
        temptrig=1;// temperature trigger set
    }

    if (temptrig==1 ){
        sendSMS(temp_str);//Call SMS function when temperature limit is crossed
    }

    if (pH<5){ //limit value condition for pH
        pHtrig=1;
    }
    if (pHtrig==1 ){
        sendSMS(pH_str);//Call SMS function when pH limit is crossed
    }

    // thinkspeak_setup();
}

void loop(){
// Add some sort of time trigger
//resetFunc(); //resetting Arduino
}

void updateSerial(){
  delay(500);
  while (Serial.available()) 
    myGsm.write(Serial.read());//Forward what Serial received to Software Serial Port
  while(myGsm.available()) 
    Serial.write(myGsm.read());//Forward what Software Serial received to Serial Port
}

void setup_SMS(){
    myGsm.begin(9600); //Begin communication with Arduino and SIM900
    updateSerial();

    myGsm.println("AT"); //AT commands for communication with SIM900
    updateSerial();

    myGsm.println("AT+CPIN?");//Enter PIN, remove all PINs from SIM card e.g. SIM PIN/PUK
    updateSerial();

    myGsm.println("AT+CMGF=1");//select SMS mode, 0 means PDU mode, 1 means text, always set to 1
    updateSerial();

    myGsm.println(sms_num);//send SMS, you can change the number, adding country code is a must
    updateSerial();
}

void sendSMS(char parameter[]){
    Serial.print(parameter);
    Serial.println(" limit crossed");//message shown in serial monitor of Arduino
    updateSerial();

    setup_SMS();

    myGsm.print(parameter);
    myGsm.println(" limit crossed ");//message to be sent
    updateSerial();

    myGsm.write( 0x1a ); // ctrl+Z character, necessary command after sending an SMS
    updateSerial();
}

void thinkspeak_setup(){

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
    
    //setting the APN,2nd parameter empty works for all networks
    //Ultra is the VPN name setted up for SIM card, it is reconfigurable
    myGsm.println("AT+CSTT=\"Ultra\",\"\",\"\"");
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
    
    //Send data to ThingSpeak.com, api_key is obtained from ThingSpeak.com,
    //it is must to send data in string format, e.g. String(temp)
    String data= thingspeak_key + String(temp)+"&field2=" +String(pH);
    myGsm.println(data);
    delay(10000);
    printSerialData();
    
    myGsm.println((char)26);//This signifies end of sending data
    delay(10000);
    printSerialData(); 
    
    myGsm.println("AT+CIPSHUT");// Deactivate GPRS PDP context
    delay(3000);
    printSerialData();
}

void printSerialData()
{
 while(myGsm.available()!=0)
    Serial.write(myGsm.read());
}
