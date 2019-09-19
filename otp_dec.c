//Name: Aditya Kothari
//Class: CS344
//Program: otp_dec
//Description: sends crypted text and key to the server, prints out the plain text

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<time.h>

int main(int argc, char *argv[])
{
	int i = 0;
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];

	if (argc < 4) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	//Read contents of the two files charcter by character and store them in a buffer
	//Char array to store plaintext and key
	char plainText[100000];
	char key[100000];
	//filling them up with null chars
	memset(plainText, '\0', sizeof(plainText));
	memset(key, '\0', sizeof(plainText));
	
	//file pointers to read the two files 
	FILE *fp1 = NULL; //read plain text
	FILE *fp2 = NULL; //read key
	fp1 = fopen(argv[1], "r");
	fp2 = fopen(argv[2], "r");
	
	//check if the files can be opened 
	if(fp1 == NULL)
	{
		fprintf(stderr, "Plaintext File cannot be opened \n");
	}

	if(fp2 == NULL)
	{
		fprintf(stderr, "Key File cannot be opened \n");
	}
	
	//read both files character by charcter
	//read plain Text file
	char ch;
	int count = 0;
	while((ch = fgetc(fp1)) != EOF)
	{
		if(ch > 32 && ch < 65)
		{
			fprintf(stderr, "Error: Plain Text has invalid characters \n");
			exit(1);
		}

		//printf("%c",ch);
		plainText[count] = ch;
		//strcpy(plainText[count],ch);
		count++;
	}
	
	//get rid of new line char and replace it with Null char
	plainText[count - 1] = '\0';

	fclose(fp1);
/*
	//test print	
	for(i = 0; i < count; i++)
	{
		printf("%c", plainText[i]);
	}
//	printf("\n");
*/

	//read Key file
	count = 0;
	while((ch = fgetc(fp2)) != EOF)
	{
		//printf("%c",ch);
		key[count] = ch;
		//strcpy(plainText[count],ch);
		count++;
	}
	
	//get rid of new line char and replace it with Null char
	key[count - 1] = '\0';

	fclose(fp2);

	//test print
/*	
	for(i = 0; i < count; i++)
	{
		printf("%c", key[i]);
	}
*/
	//throw error if key genrated is smaller than plain text
	//printf("key = %d \n", strlen(key));
	//printf("plainText = %d \n", strlen(plainText));

	if(strlen(key) < strlen(plainText))
	{
		fprintf(stderr, "Error: Key is shorter than plain text \n");
		exit(1);
	}
	

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct.
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string	
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

 	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");


	//printf("test1 \n"); fflush(stdout);
	//Connect to the server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to addy
	{	
		error("CLIENT: ERROR connecting");
	}

	//Error check make sure connection is from right client
	//send client name to the server
	charsWritten = send(socketFD, argv[0], strlen(argv[0]), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");




	//SEND PLAIN TEXT

	//send file size of plain text
	int32_t strSize = strlen(plainText);
	char *length = (char*)&strSize;
	int num = sizeof(int32_t);
	int iter = 0;
	int sendSize;
	char *ptr_size = length;

	//write off data to server - length of file being sent
	while(iter < num)
	{
		sendSize = num - iter;
		charsWritten = send(socketFD, ptr_size, sendSize, 0); // Write to the server
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		iter = iter + charsWritten;
		ptr_size = ptr_size + charsWritten;
	}

	//send plain text
	num = strlen(plainText);
	iter = 0;
	char *ptr = &plainText[0]; //pointer point to the first char of plain text
	int sendLen;//ammount of data to be sent

	while(iter < num)
	{
		sendLen = num - iter;
		charsWritten = send(socketFD, ptr, sendLen, 0); // Write to the server
		
	//	printf("client sending: %d \n",charsWritten); //test print
		
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		iter = iter + charsWritten; //update ammount of chars sent
		ptr = ptr + charsWritten;//pointer maff: update pointer location
	}

	//SEND KEY

	//send file size of plain text
	strSize = strlen(key); //get size of key
	length = (char*)&strSize; //type cast 
	num = sizeof(int32_t); //num = 4
	iter = 0; //reset 
	sendSize = 0; //reset
	ptr_size = length; 

	//write off data to server - length of key sent
	while(iter < num)
	{
		sendSize = num - iter;
		charsWritten = send(socketFD, ptr_size, sendSize, 0); // Write to the server
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		iter = iter + charsWritten;
		ptr_size = ptr_size + charsWritten;
	}

	//send key
	num = strlen(key);
	iter = 0;
	ptr = &key[0]; //pointer point to the first char of key
	sendLen = 0;//ammount of data to be sent

	while(iter < num)
	{
		sendLen = num - iter;
		charsWritten = send(socketFD, ptr, sendLen, 0); // Write to the server
		
	//	printf("client sending: %d \n",charsWritten); //test print
		
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		iter = iter + charsWritten; //update ammount of chars sent
		ptr = ptr + charsWritten;//pointer maff: update pointer location
	}
	
	//Recive crypt buff

	//Recieve crypt buff length
	int32_t clen;
	char *cryptLength = (char*)&clen;
	iter = 0;
	num = sizeof(int32_t);
	int rcvlen = 0;
	char *ptr_rcv = cryptLength;

	while(iter < num)
	{
		rcvlen = num - iter;
		charsRead = recv(socketFD, ptr_rcv, rcvlen, 0);
		if(charsRead < 0) error("ERROR reading from socket");
		iter = iter + charsRead;
		ptr_rcv = ptr_rcv + charsRead;
	}

	//test print length
	//printf("%d \n", clen);


	//Recieve cypt buff
	char CryptBuff[clen]; //create buffer of leng
	memset(CryptBuff, '\0', sizeof(CryptBuff));
	iter = 0;
	char *crypt_ptr = &CryptBuff[0];
	rcvlen = 0;

	while(iter < clen)
	{
		rcvlen = clen - iter;
	//	printf("client length expected: %d",rcvlen);

		charsRead = recv(socketFD, crypt_ptr, rcvlen, 0);
	//	printf("client chars Read: %d",charsRead);

		if(charsRead < 0) error("ERROR reading from socket");
		
		iter = iter + charsRead;
	//	printf("client iter: ",rcvlen);

		crypt_ptr = crypt_ptr + charsRead;
	}
		
	//Print crypt buff to stodut
	//int i = 0;
	for(i = 0; i < charsRead; i++)
	{
		fprintf(stdout,"%c", CryptBuff[i]);
	}
	fprintf(stdout,"\n");

	close(socketFD);
	return 0;
}
