#include <fstream>
#include <iostream>
#include <cstddef>

#define MULTIPLES 0xf
#define CHAR_ENCODE 0xf0

#define LOW_THREE 0x7
#define COMMA_MASK 0xf8

using namespace std;

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

void decode(const char *filename) {
    ifstream input;
    input.open(filename, ios::in|ios::binary);
    char *oData = 0;

    input.seekg(0, ios::end);
    int size = input.tellg();
    input.seekg(0, ios::beg);
    
    oData = new char[ size+1 ]; //  for the '\0'
    input.read( oData, size );
    oData[size] = '\0' ; // set '\0' 


    // setup output file
    ofstream output;
    output.open("decoded_output");
    for(int i = 0; i < size; i++) {
        char datum = oData[i] ^ 0xfe;
        if(datum == 0) {
            output << endl;
        } else {
            int count;
            char ch;
            // first check if it's a comma
            if((oData[i] & COMMA_MASK) == COMMA_MASK) {
                count = (oData[i] & LOW_THREE)+1;
                ch = 0xf8;
            } else {
                count = (oData[i] & MULTIPLES)+1; // lower 3
                ch = (oData[i] & CHAR_ENCODE);  // upper 5
            }
            for(int j = 0; j < count; j++)
                output << decode_char(ch);
        }
    }

    output.close();
    input.close();
}

int main(int argc, char *argv[]) {
    
    decode(argv[1]);
    return 0;
}
