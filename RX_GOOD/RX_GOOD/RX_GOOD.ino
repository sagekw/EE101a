#include <TimerOne.h>

/* Pin Delarations */
#define RX_PIN  2

/* Constants */
#define T_BIT           500 // microseconds
#define BITS_PER_BYTE   8

#define BUF_SIZE        256
#define I_MASK          0xFF
#define SAMPS_PER_BIT   8
#define G_ERROR         0.01
#define N_MAP           18
#define MAP_MASK        0x1F
#define REPEAT_MASK     0x07
#define REPEAT_OFFSET   5

/* This is for decoding our compression algorithm
 * The algorithm uses 5 bits to represent all possible characters.
 * The remaining 3 bits are how many consecutive instances that character appears.
*/
char char_map[] = {' ', 'Z', '$', 'I', '+', '\n', 'M', ',', 'O', '.', 'N', '~', '7', '8', ':', '=', '?', 'D'};

const unsigned long SYNC_WORD = 0xFF00FF00;
const unsigned long SYNC_MASK = 0xFFFFFFFF;

/* Derived Constants */
const unsigned int T_SAMP = T_BIT/SAMPS_PER_BIT;

int n_read = 0;
byte samp_buf[BUF_SIZE];
int n_out_bits = 0;
byte out_byte = 0;
byte i_write = 0;
byte i_read = 0;
byte last_samp = 0;
byte curr_samp = 0;
byte _i_write = 0;
float n_samps_per_bit = SAMPS_PER_BIT;
int i_samp = SAMPS_PER_BIT/2;
int n_update_bits = 0;
int n_update_samps = 0;
unsigned long sync_buf = 0;
bool have_sync = false;
int n_repeat;
char enc_char;

/* Timer Interrupt Service Routine
 *  We gutted the ISR so that interrupts wouldn't build up and the serial print would not block future interrupts.
*/
void buffer_sample()
{
  samp_buf[i_write] = digitalRead(RX_PIN);
  i_write = (i_write + 1) & I_MASK;
}


/* Initializations */
void setup()
{
  pinMode(RX_PIN, INPUT);
  Serial.begin(115200); 

  /* Set Timer1 to run receiveNextSample() every samplePeriod microseconds. */
  Timer1.initialize(T_SAMP);
  Timer1.attachInterrupt(buffer_sample); 
}

/*
 * process_bit manages 2 running buffers. 
 * 1) sync_buf is 32 bits and looks for the syncronization that is sent at the beginning of execution of the transmitter
 * 2) out_byte is 8 bits and contains the data bits
 * 
 */
void process_bit(byte b)
{
  if (have_sync) {
    // b is the most recent bit
    // shift b into lowest bit of current byte
    out_byte = (out_byte << 1) | b;
    n_out_bits++;

    // If we have a byte, then decode it and print it
    if (n_out_bits == BITS_PER_BYTE) {
      if ((out_byte == 0xFF)) {
          Serial.print('\n');
      }
      else {
        enc_char = out_byte & MAP_MASK;
        if (enc_char < N_MAP) {
          n_repeat = (out_byte >> REPEAT_OFFSET) & REPEAT_MASK;
          for (int n = 0; n < n_repeat + 1; n++) {
            Serial.print(char_map[enc_char]);
          }
        }
        // If the byte is not recognized, we got an error and are out of sync, so need to re-sync.
        else {
            have_sync = false;
        }
      }
      n_out_bits = 0;
    }
  }
  // Not in sync, wait for sync message (0xFF, 0x00, repeating)
  else {
    sync_buf = (sync_buf << 1) | b;
    have_sync = ((sync_buf & SYNC_MASK) == SYNC_WORD);
    n_out_bits = 0;
  }
}


/* Main function */
void loop() 
{

  // Copying variable that can be changed in an interrupt into a variable that is only changed in this function
  noInterrupts();
  _i_write = i_write;
  interrupts();
  
  while (i_read != _i_write) {
    
    curr_samp = samp_buf[i_read];
    i_read = (i_read + 1) & I_MASK;
    
    // if sample is in middle of period, use as decoded symbol 
    if ((n_update_samps - round(n_update_bits*n_samps_per_bit)) == i_samp) {
      process_bit(curr_samp);
      n_update_bits++;
    }
    
    // if detected a change in sample value, update period estimate
    // this piece of code is what keeps the receiver locked onto the symbol clock
    if (curr_samp ^ last_samp) {
      if (n_update_bits > 0) {
        n_samps_per_bit -= G_ERROR*(n_samps_per_bit - float(n_update_samps)/n_update_bits);
      }
      i_samp = round(n_samps_per_bit/2);
      n_update_samps = 0;
      n_update_bits = 0;
    }

    
    n_update_samps++;
    last_samp = curr_samp;
   }
}

