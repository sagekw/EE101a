
#include <TimerOne.h>

/* Pin Delarations */
#define RX_PIN  2
#define DEBUG_PIN 13

/* Other Constants */
#define COUNTS 8
#define SAMPLE_PERIOD  350 // microseconds, originally 10000
#define TIMER_PERIOD   SAMPLE_PERIOD/COUNTS // microseconds
#define START_SIZE        16
#define BITS_PER_BYTE      8

#define LOWER_FOUR 0xf
#define UPPER_FOUR 0xf0
#define LOWER_THREE 0x7
#define UPPER_FIVE 0xf8


/* Globals */
boolean set = false;
int numOnes = 0;
int bitIndex = 0;
char currentByte = 0;
int sendbit = 0;
boolean foundEdge = false;
int timerCounter = 0;


/* Initializations */
void setup()
{
  pinMode(RX_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RX_PIN), edgeDetect, CHANGE);
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
  foundEdge = true;
}


/* Timer Interrupt Service Routine */
void receiveNextBit()
{
  if( timerCounter == (COUNTS/2 - 1) ) {
    /* Read the current bit. */
    int bit = digitalRead(RX_PIN);

    processBit( bit );
    countOnes( bit );  // this only happens before a new tom lee, not before a new line
  }
  if( timerCounter == (COUNTS-1) ) {
    timerCounter = 0;
  } else {
    timerCounter++;
  }
}


/* Process the current bit. */
inline
void processBit(int bit)
{
  if(set) {
    /* Write the bit into the current byte. */
    bitWrite(currentByte, bitIndex, bit);
    bitIndex++;
    
    /* Process the current byte if it is complete. */
    if(bitIndex == BITS_PER_BYTE)
    {
      bitIndex = 0;
      processByte(currentByte);
    }
  }
}

/* decoding map */
static inline char decode_char(unsigned char encoded_char) {
    switch (encoded_char) {
        case 0xf8:
            return ',';
        case 0xf0:
            return ':';
        case 0xe0:
            return '=';
        case 0xd0:
            return 'Z';
        case 0xc0:
            return '~';
        case 0xb0:
            return '8';
        case 0xa0:
            return '7';
        case 0x90:
            return 'O';
        case 0x80:
            return 'N';
        case 0x70:
            return '.';
        case 0x60:
            return 'M';
        case 0x50:
            return '?';
        case 0x40:
            return '+';
        case 0x30:
            return 'I';
        case 0x20:
            return 'D';
        case 0x10:
            return '$';
        case 0x0:
            return ' ';
        default:
            return ' ';
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
  unsigned char encoded_byte = b; // make sure it's interpretted unsigned
  char new_line = (encoded_byte ^ 0xfe); // 0xfe is the sequence for a line break
  if(new_line == 0) {
    Serial.println();
  } else {
    int count;
    unsigned char ch;
    if((encoded_byte & UPPER_FIVE) == UPPER_FIVE) { // encoding of comma
      count = (encoded_byte & LOWER_THREE) + 1;     // comma is specail because it's encoding is the five upper
      ch = 0xf8; // comma's encoding                // bits whereas everything else is only 4 bits. This is because
    } else {                                        // there are 17 chars (16 could be represented in just the upper
      count = (encoded_byte & LOWER_FOUR) + 1;      // 4 bits, and the 17th uses one additional bit). I chose the comma
      ch = (encoded_byte & UPPER_FOUR);             // to be the character with one additional bit because it's one
    }                                               // of the least frequently used character.
    int j;
    ch = decode_char(ch); // use map to get the actual character
    for(j = 0; j < count; j++) {
      Serial.print(ch);
    }
  }
/*
  if((b >= 32 && b <= 126) || b == '\n' || b == '\r' || b == '\t')
    Serial.print(b);
  else
    set = false;
*/
}


/* 
 * Count consecutive 1's to check for the start of 
 * a new message. 
 */
inline
void countOnes(int bit)
{
  if(bit == 1)
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

