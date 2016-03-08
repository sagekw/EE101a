make:
	g++ -ggdb encode.cpp -o encode
	g++ -ggdb decode.cpp -o decode

clean:
	rm encode.exe decode.exe
	rm encoded_output decoded_output
 
