#include <TimerOne.h>

/* Pin Delarations */
#define TX_PIN  13
#define DEBUG_PIN 12

/* Other Constants */
#define TIMER_PERIOD   350 // microseconds, originally 10000
#define BITS_PER_BYTE      8


/* Globals Variables */
byte message[] = "__It sure is nice when things are working the way they should.";
byte messageLength = sizeof(message);
int byteIndex = 0;
int bitIndex = 0;
int debug = 0;


/* Initializations */
void setup() 
{                
  pinMode(TX_PIN, OUTPUT); 
  pinMode(DEBUG_PIN, OUTPUT);
  Serial.begin(9600);  
  
  /* Set Timer1 to run transmitNextBit() every TIMER_PERIOD microseconds */
  Timer1.initialize(TIMER_PERIOD);         
  Timer1.attachInterrupt(transmitNextBit); 
  
  /* Intialize start of message sequence. */
  message[0] = 0b11111111;
  message[1] = 0b11111111;
}


/* Main function (unused in this program) */
void loop() {}


/* Timer Interrupt Service Routine */
void transmitNextBit()
{
  /* Determine next bit to transmit. */
  int bit = bitRead(message[byteIndex], bitIndex);
  bitIndex++;
  
  /* Check if we've completed the current byte. */
  if(bitIndex == BITS_PER_BYTE)
  {
    bitIndex = 0;
    byteIndex++;
    if(byteIndex == messageLength)
      byteIndex = 0;
  }
  
  /* Trasmit the next bit */
  digitalWrite(TX_PIN, bit);
  digitalWrite(DEBUG_PIN, debug);
  debug ^= 1;
}  

