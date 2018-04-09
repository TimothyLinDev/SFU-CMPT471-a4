#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <istream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

//some of the code here is from www.linuxhowtos.orgt/C_C++/socket.htm
using namespace std;

char key[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' ', '.', ',', '?'};

void decrypt(int k, char* msg, int size){
	
	int i = k%5;
	int index = 0; // when reading from the message
	bool finished = false;
	
	char temp[4][4]; //where chars are stored to be transposed
	int place = 0; // when reading to the message

	//Transposition Decryption Schemes
	if(i == 0 || i == 2 || i == 4){
		while(index < size){
			for(int row = 0; row < 4; row++){//read to matrix
				for(int col = 0; col < 4; col++){
					temp[row][col] = msg[index];
					index++;
				}
			}
			while(place < index){ // read back to message
				for(int row = 0; row < 4; row++){
					for(int col = 0; col < 4; col++){
						msg[place] = temp[col][row];
						place++;
					}
				}
			}
		
		}
	
	}
	else{
		while(index < size){
			for(int row = 0; row < 4; row++){//read to matrix
				for(int col = 0; col < 4; col++){
					temp[row][col] = msg[index];
					index++;
				}
			}
			while(place < index){//read back to message
				for(int row = 3; row > -1; row--){
					for(int col = 3; col > -1; col--){
						msg[place] = temp[row][col];
						place++;
					}
				}
			}
		
		}
	
	}
	// Substitution Decryption Schemes
	if(i == 0 || i == 1){ // +2
		for(int h = 0; h < size; h++){
			index = 0;
			finished = false;
			while(finished == false || index < 30){
				if(msg[h] == key[index]){
					index = (index-2)%30; 
					msg[h] = key[index];
					finished = true;
				}
				index++;	
			}
		}
	}
	else if(i == 2 || i == 3){ //-2
		for(int h = 0; h < size; h++){
			index = 0;
			finished = false;
			while(finished == false || index < 30){
				if(msg[h] == key[index]){
					index = (index+2)%30;
					msg[h] = key[index];
					finished = true;
				}
				index++;	
			}
		}
	 	
	}
	else{
		for(int h = 0; h < size; h++){//+4
			index = 0;
			finished = false;
			while(finished == false || index < 30){
				if(msg[h] == key[index]){
					index = (index-4)%30;
					msg[h] = key[index];
					finished = true;
				}
				index++;	
			}
		}
	}
	
	
}

int main(int argc, char *argv[]) {
	
	//n and q
	int n = 3;
	int q = 7;

	//creates a socket
	int newsoc;
	newsoc = socket(PF_INET,SOCK_STREAM,0);
	if(newsoc < 0) // prints error if creation failed
		cout << "Socket Creation Error" << endl; 

	//create structs for server + client addresses
	struct sockaddr_in saddr;
	struct sockaddr_in caddr;
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = 7000;
	
	// binds socket
	int binding;
	binding = bind(newsoc, (struct sockaddr *)&saddr, sizeof(saddr));
	if(binding < 0)
		cout << "Binding Error" << endl;
	//start listening
	int listening;
	listening = listen(newsoc, 1);
	if(listening < 0)
		cout << "Listening Error" << endl;
	//create me socket for tcp connection
	int csocket;
	socklen_t client_len = sizeof(caddr);
	csocket = accept(newsoc, (struct sockaddr *)&caddr, &client_len);
	if (csocket < 0)
		cout << "Second Socket Creation Error" <<endl;

	char msg[256];

	//generate xb
	srand(time(NULL));
	int xb = rand() % 9 + 1;
	cout << "The private number xb is: " << xb << endl;
	
	//receive ya
	int recvya;
	recvya = recv(csocket, msg, 255, 0);
	if(recvya < 0)
		cout << "ya Receiving Error" << endl;

	int ya = atoi(msg);
	cout << "The public number ya is: " << ya << endl;

	//calculate yb
	int temp = pow(n,xb);
	int yb = (int)temp % q;
	cout << "The public number yb is: " << yb << endl;

	// send yb
	sprintf(msg, "%d", yb);
	int ybsend;
	ybsend = send(csocket, msg, 255, 0);
	if(ybsend < 0)
		cout << "Error Sending yb" << endl;

	//calculating the secret key
	temp = pow(ya,xb);
	int kb = (int)temp % q;
	cout << "The secret key kb is: " << kb << endl;
	
	int msgrecv;
	msgrecv = recv(csocket, msg, 255,0);
	if(msgrecv < 0)
		cout << "Message Receiving Error" << endl;
	
	//decryption
	int size = strlen(msg);
	decrypt(kb, msg, size);
	cout << msg <<endl;
	//close connections
	int c1;
	c1 = close(newsoc);
	if(c1 < 0)
		cout << "Error Closing Connection" <<endl;

	int c2;
	c2 = close(csocket);
	if(c2 < 0)
		cout << "Error Closing Connection" <<endl;
	return 0;
}

