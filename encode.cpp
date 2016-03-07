#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stdlib.h>
#include <cstddef>

using namespace std;

void encode(map<char,char>& map, const char *filename) {
    
    ifstream input;
    input.open(filename, ios::in);
    char *oData = 0;

    string line;
    ofstream output;
    output.open("encoded_output");
    while(getline(input,line)) {
        char cur = line[0];
        int count = 1;
        for(int i = 1; i < line.length(); i++) {
            count++;
            if(line[i] == cur) {
                if(count == 16) {
                    char val = map[cur] | (count-1);
                    output << val;
                    cur = line[i+1];
                    count = 0;
                }
            } else {
                char val = map[cur] | (count-2);
                output << val;
                cur = line[i];
                count = 1;
            }       
        }
        if(count != 0) {
            char val = map[cur] | (count - 1);
            output << val;
        }
        char line_end = 0xfe;
        output << line_end;
    }
    input.close();
    output.close();
}


int main(int argc, char *argv[]) {

    map<char, char> encoding_map;
    encoding_map[','] =0xf8;
    encoding_map[':'] =0xf  << 4;
    encoding_map['='] =0xe  << 4;
    encoding_map['Z'] =0xd  << 4;
    encoding_map['~'] =0xc  << 4;
    encoding_map['8'] =0xb  << 4;
    encoding_map['7'] =0xa  << 4;
    encoding_map['O'] =0x9  << 4;
    encoding_map['N'] =0x8  << 4;
    encoding_map['.'] =0x7  << 4;
    encoding_map['M'] =0x6  << 4;
    encoding_map['?'] =0x5  << 4;
    encoding_map['+'] =0x4  << 4;
    encoding_map['I'] =0x3  << 4;
    encoding_map['D'] =0x2  << 4;
    encoding_map['$'] =0x1  << 4;
    encoding_map[' '] =0x0  << 4;
    
    encode(encoding_map, argv[1]);
    return 0;
}
