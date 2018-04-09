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

//cipher key for substitution
char key[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' ', '.', ',', '?'};

void encrypt(int k, char* msg, int size){
	
	int i = k%5;
	int index = 0; // when reading from the message
	bool finished = false;

	// Substitution Schemes
	if(i == 0 || i == 1){
		for(int h = 0; h < size; h++){ // +2 Encrypt
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
	else if(i == 2 || i == 3){
		for(int h = 0; h < size; h++){ //-2 Encrypt
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
	else{
		for(int h = 0; h < size; h++){//+4 Encrypt
			index = 0;
			finished = false;
			while(finished == false || index < 30){
				if(msg[h] == key[index]){
					index = (index+4)%30;
					msg[h] = key[index];
					finished = true;
				}
				index++;	
			}
		}
	}
	
	//Transposition Schemes
	char temp[4][4]; //where chars are stored to be transposed
	int place = 0; // when reading to the message
	index = 0;

	if(i == 0 || i == 2 || i == 4){
		while(index < size){
			for(int row = 0; row < 4; row++){ //read to matrix
				for(int col = 0; col < 4; col++){
					temp[row][col] = msg[index];
					index++;
				}
			}
			while(place < index){// read to message
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
		while(index < size){ // read to matrix
			for(int row = 0; row < 4; row++){
				for(int col = 0; col < 4; col++){
					temp[row][col] = msg[index];
					index++;
				}
			}
			while(place < index){ //read to message
				for(int row = 3; row > -1; row--){
					for(int col = 3; col > -1; col--){
						msg[place] = temp[row][col];
						place++;
					}
				}
			}
		
		}
	
	}
}

int main(int argc, char *argv[]) {
	
	// n and q
	int n = 3;
	int q = 7;

	//creates a socket
	int newsoc;
	newsoc = socket(PF_INET,SOCK_STREAM,0);
	if(newsoc < 0) // prints error if creation failed
		cout << "Socket Creation Error" << endl; 

	//structs that hold the server address
	struct sockaddr_in servaddr;
	struct hostent *serv;
	char *hostname = "november";
	
	//grab host ip address
	serv = gethostbyname(hostname);
	servaddr.sin_family = AF_INET;
	bcopy((char *)serv->h_addr, (char*)&servaddr.sin_addr.s_addr, serv->h_length);

	servaddr.sin_port = 7000;
	
	//create new connection
	int newcon;
	newcon = connect(newsoc, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(newcon < 0)
		cout << "Connection Error" << endl;
	
	//message
	char msg[256];
	
	//generate xa
	srand(time(NULL));
	int xa = rand() % 20 + 1;
	cout << "The private number xa is: " << xa << endl;
	
	//calculating ya
	double temp = pow(n,xa);
	int ya = (int)temp % q;
	cout << "The public number ya is: " << ya << endl;

	// sending ya
	sprintf(msg, "%d", ya);
	int yasend;
	yasend = send(newsoc, msg, 255, 0);
	if(yasend < 0)
		cout << "Error Sending ya" << endl;
	
	// receiving yb
	int ybrecv;
	ybrecv = recv(newsoc, msg, 255, 0);
	if(ybrecv < 0)
		cout << "Error Recieving yb" << endl;
	int yb = atoi(msg);
	cout << "The public number yb is: " << yb << endl;

	//caluting the secret key
	temp = pow(yb,xa);
	int ka = (int)temp % q;
	cout << "The secret key ka is:  " << ka << endl;

	cout << endl << "Enter message to be encrypted: ";
	cin.getline(msg, 255);
	
	//encryption
	int size = strlen(msg);

	encrypt(ka, msg, size);
	cout << msg <<endl;
	
	//sending the message
	int sendmsg;
	sendmsg = send(newsoc,msg,255,0);
	if (sendmsg < 0)
		cout << "Error Sending Message" <<endl;

	//close connection
	int c;
	c = close(newsoc);
	if(c < 0)
		cout << "Error Closing Connection" <<endl;


	return 0;
}

