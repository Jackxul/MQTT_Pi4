#include <IRremote.h>


#define PIN_SEND 3
const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;
IRsend mysend;
void setup(){
  Serial.begin(9600);
  IrSender.begin(PIN_SEND);
  irrecv.enableIRIn();
  irrecv.blink13(true);
}

void loop(){
  IrSender.sendNEC(0x8737C1A0, 32);
  delay(100);
  IrSender.sendNEC(0xF9FC6A3F, 32);
  //mysend.send(PANASONIC,0x8737C1A0,32);
  delay(100);
  
  //mysend.send(PANASONIC,0xF9FC6A3F,32);
  if (irrecv.decode(&results)){
        Serial.println(results.value, HEX);
        Serial.println(results.bits);
        switch (results.decode_type){
            case NEC: 
              Serial.println("NEC"); 
              break ;
            case SONY: 
              Serial.println("SONY");
              break ;
            case RC5:
              Serial.println("RC5");
              break ;
            case RC6:
              Serial.println("RC6");
              break ;
            case SHARP: 
              Serial.println("SHARP"); 
              break ;
            case JVC: 
              Serial.println("JVC"); 
              break ;
            case SAMSUNG: 
              Serial.println("SAMSUNG"); 
              break ;
            case LG: 
              Serial.println("LG"); 
              break ;
            case WHYNTER: 
              Serial.println("WHYNTER"); 
              break ;
            case PANASONIC: 
              Serial.println("PANASONIC"); 
              break ;
            case DENON: 
              Serial.println("DENON"); 
              break ;
          default:
            case UNKNOWN: 
            Serial.println("UNKNOWN"); 
            break ;
          }
        irrecv.resume();
  }
}
