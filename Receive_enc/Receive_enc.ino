
#include <TimerOne.h>

/* Pin Delarations */
#define RX_PIN  2
#define DEBUG_PIN 13

/* Other Constants */
#define COUNTS 20
#define SAMPLE_PERIOD  300 // microseconds, originally 10000
#define TIMER_PERIOD   SAMPLE_PERIOD/COUNTS // microseconds
#define START_SIZE        16
#define BITS_PER_BYTE      8

/* Globals */
boolean set = false;
int numOnes = 0;
int bitIndex = 0;
char currentByte = 0;
int sendbit = 0;
int timerCounter = 0;


/* Initializations */
void setup()
{
  pinMode(RX_PIN, INPUT);
  //attachInterrupt(digitalPinToInterrupt(RX_PIN), edgeDetect, CHANGE);
  pinMode(DEBUG_PIN, OUTPUT);
  Serial.begin(115200); 

  /* Set Timer1 to run receiveNextBit() every TIMER_PERIOD microseconds */
  Timer1.initialize( TIMER_PERIOD );
        
  Timer1.attachInterrupt(receiveNextBit); 
}


/* Main function (unused in this version) */
void loop() {
}

/* Manage edge detection */
void edgeDetect() {
  timerCounter = 0;
  //sendbit = sendbit^0x1;
}


/* Timer Interrupt Service Routine */
void receiveNextBit()
{
  sendbit = sendbit^0x1;
  digitalWrite(DEBUG_PIN, sendbit);
  
  /*
   if( timerCounter == (COUNTS/2 - 1) ) {
    int _bit = digitalRead(RX_PIN);
    

    //processBit( _bit );
    //countOnes( _bit );
  }
  */
  /*
  if( timerCounter == (COUNTS-1) ) {
    timerCounter = 0;
  } else {
    timerCounter++;
  }
  */
}


/* Process the current bit. */
inline
void processBit(int _bit)
{
  if(set) {
    /* Write the bit into the current byte. */
    bitWrite(currentByte, bitIndex, _bit);
    bitIndex++;
    
    /* Process the current byte if it is complete. */
    if(bitIndex == BITS_PER_BYTE)
    {
      bitIndex = 0;
      processByte(currentByte);
    }
  }
}


/* 
 * Checks if the current byte is a printable character. 
 * A character is considered printable if it is either 
 * alphanumeric, white space or punctuation.
 */
inline 
void processByte(char b)
{
  if((b >= 32 && b <= 126) || b == '\n' || b == '\r' || b == '\t')
    Serial.print(b);
  else
    set = false;
    //sendbit = sendbit^0x1;
    //digitalWrite(DEBUG_PIN, sendbit);
}


/* 
 * Count consecutive 1's to check for the start of 
 * a new message. 
 */
inline
void countOnes(int _bit)
{
  if(_bit == 1)
  {
    numOnes++;
    if(numOnes == START_SIZE)
    {
      numOnes = 0;
      bitIndex = 0;
      set = true;
      Serial.println();
    }
  }
  else
    numOnes = 0;
}
