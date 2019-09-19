//Name: Aditya Kothari
//Class: CS344
//Program: otp_enc_d
//Descriptions: recieves key and crypted text from the client, decrypts it and sends it back to the client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

//function conversts n to text
char tton(char val)
{
	//if empty space encountered return 26 i.e SUB
	//else return value between 0 and 25 i.e NULL and EM
	return (val == ' ')?26:val - 'A';
}

//function conversts int to text
char ntot(char val)
{
	//if encounter SUB i.e 26 replace with empty space
	//else retrun value between 65 and 90 i.e A and Z
	return (val == 26)?' ':val + 'A';
}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256];
//	char bufferPlainText[100000];
//	char buffKey[100000];
	struct sockaddr_in serverAddress, clientAddress;
	int i = 0;
	pid_t spawnPid;
	int listenFlag;	

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");

	while(1)
	{
		listenFlag = listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

		if(listenFlag == 0)
		{
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");

		//Error check client	
		char client_name[8]; 
		memset(client_name, '\0', sizeof(client_name));
		charsRead = recv(establishedConnectionFD, client_name, 7, 0); // Read the client's message from the socket
		if (charsRead < 0) error("ERROR reading from socket");
	
/* 		Test print
		for(i = 0; i < charsRead; i++)
		{
			printf("%c", client_name[i]); fflush(stdout);
		}
		printf("\n");
*/
		//if not the right client throw error
		if((strcmp(client_name, "otp_dec") != 0))
		{
			fprintf(stderr, "ERROR: NOT THE RIGHT CLIENT \n");
			exit(1);
		}


		//FORK
		spawnPid = fork();
	
		if(spawnPid == -1) //if failed throw error
		{
			fprintf(stdout,"ERROR: DID NOT FORK\n");
			exit(1);
		}
		else if(spawnPid == 0) //inside child: recieve key and plain text, encrypt and send it to client
		{
			//GET PLAIN TEXT

			//Get length of incoming file from the client 
			int32_t len; 
			char *length = (char*)&len;
			int iter = 0;			
			int num = sizeof(int32_t);
			int rcvLen = 0;
			char *ptr_rcv = length;

			while(iter < num)
			{
				rcvLen = num - iter;
				charsRead = recv(establishedConnectionFD, ptr_rcv, rcvLen, 0); // Read the client's message from the socket
		//		printf("length rcv: %d \n",charsRead);
			
				if (charsRead < 0) error("ERROR reading from socket");
				iter = iter + charsRead;
				ptr_rcv = ptr_rcv + charsRead;
			}
			//test print 
		//	printf("length recived server: %d \n", len);  fflush(stdout);
			int lenPlainText = len;
	
	/*		//send sucess message
			charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0);
			if (charsRead < 0) error("ERROR writing to socket");
	*/

			//Get Plain text from the client
			char PlainTextBuff[len]; // allocate buffer size of len
			memset(PlainTextBuff, '\0', sizeof(PlainTextBuff)); // fill buff with null chars
			
			iter = 0; //keep track of chars written
			char *ptr = &PlainTextBuff[0];	
			rcvLen = 0;	

		//	printf("test 1 \n"); fflush(stdout);
			while(iter < len)
			{
//				printf("%d \n", len);

				rcvLen = len - iter;
//				printf("%d \n", rcvLen);

				charsRead = recv(establishedConnectionFD, ptr, rcvLen, 0); // Read the client's message from the socket
//				printf("text rcv: %d \n",charsRead);
	
				if (charsRead < 0) error("ERROR reading from socket");
				iter = iter + charsRead; //update the number of chars read
				ptr = ptr + charsRead;
			}
		//	printf("test 2\n");
			
			/*
			//TEST PRINT PLAIN TEXT
			for(i = 0; i < charsRead; i++)
			{
				printf("%c", PlainTextBuff[i]);
			}
			printf("\n");
			*/

			//GET KEY

			//get key length

			len = 0; //reset len 
			length = (char*)&len; //type cast
			iter = 0; //reset iter			
			num = sizeof(int32_t); //num = 4
			rcvLen = 0;
			ptr_rcv = length;

			while(iter < num)
			{
				rcvLen = num - iter;
				charsRead = recv(establishedConnectionFD, ptr_rcv, rcvLen, 0); // Read the client's message from the socket
		//		printf("length rcv: %d \n",charsRead);
			
				if (charsRead < 0) error("ERROR reading from socket");
				iter = iter + charsRead;
				ptr_rcv = ptr_rcv + charsRead;
			}
			//test print 
		//	printf("length recived server: %d \n", len);  fflush(stdout);
	
	/*		//send sucess message
			charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0);
			if (charsRead < 0) error("ERROR writing to socket");
	*/

			//Get key from client

			char KeyBuff[len]; // allocate buffer size of len
			memset(KeyBuff, '\0', sizeof(KeyBuff)); // fill buff with null chars
			
			iter = 0; //keep track of chars written
			ptr = &KeyBuff[0];	
			rcvLen = 0;	

		//	printf("test 1 \n"); fflush(stdout);
			while(iter < len)
			{
//				printf("%d \n", len);

				rcvLen = len - iter;
//				printf("%d \n", rcvLen);

				charsRead = recv(establishedConnectionFD, ptr, rcvLen, 0); // Read the client's message from the socket
//				printf("text rcv: %d \n",charsRead);
	
				if (charsRead < 0) error("ERROR reading from socket");
				iter = iter + charsRead; //update the number of chars read
				ptr = ptr + charsRead;
			}
		//	printf("test 2\n");
		
			/*
			//TEST PRINT KEY
			for(i = 0; i < charsRead; i++)
			{
				printf("%c", KeyBuff[i]);
			}
			printf("\n");
			*/

			//Encrypt data
			char CryptBuff[lenPlainText + 1]; //create character buffer size of plain Text
			memset(CryptBuff, '\0', sizeof(CryptBuff)); // +1 for trailing '\0'

			//loop for size of buffer
			for(i = 0; i < lenPlainText; i++)
			{
				//encrypt data and store it in crypt buffer
				CryptBuff[i] = ntot((tton(PlainTextBuff[i]) - tton(KeyBuff[i]) + 27)%27);
			}
	
			/*
			//TEST PRINT CRYPT BUFF
			for(i = 0; i < lenPlainText; i++)
			{
				printf("%c", CryptBuff[i]);
			}
			printf("\n");
			*/

			//Sent crypted file over to the client	

			//send file size of the crypted file over to the client
			int32_t strSize = strlen(CryptBuff);
			char *lent = (char*)&strSize;
			num = sizeof(int32_t);
			iter = 0;
			int sendSize = 0;
			char *ptr_len = lent;
			int charsWritten = 0;

			//send length over
			while(iter < num)
			{
				sendSize = num - iter;
				charsWritten = send(establishedConnectionFD, ptr_len, sendSize, 0);
				iter = iter + num;
				ptr_len = ptr_len + charsWritten;
			}

			//send crypted file over
			num = strlen(CryptBuff);
			iter = 0; //reset iterator
			char *crypt_ptr = &CryptBuff[0];
			sendSize = 0;

			while(iter < num)
			{
				sendSize = num - iter;
				charsWritten = send(establishedConnectionFD, crypt_ptr, sendSize, 0);
				iter = iter + num;
				crypt_ptr = crypt_ptr + charsWritten;
			}
			}
		}
		else //parrent
		{
			close(establishedConnectionFD); // Close the existing socket which is connected to the client
		}
	}

	close(listenSocketFD); // Close the listening socket
	return 0; 
}
