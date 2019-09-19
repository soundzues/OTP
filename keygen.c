//Name: Aditya Kothari
//Class: CS344
//Program: keygen
//Description: produes a string of uppercase letters of specified length 

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>

int main(int argc, char *argv[])
{
	int i = -5;
	time_t t;
	int randomNum;
	char c;

	//seed rand()
	srand(time(&t)); 

	//throw error if number of arguments greater than 2
	if(argc > 2 || argc < 2)
	{
		fprintf(stderr,"Error: Invalid number of arguments \n");
		exit(1);
	}
	
	//convert argument from char to int
	int keylen = atoi(argv[1]); 

	//genrate a string of random number of keylen
	for(i = 0; i < keylen; i++)
	{	
		//genrate a random number bettween 
		randomNum = rand()%27;

		//genrate random char
			
		//26 test
		//randomNum = 26;			
		//printf("%d \n", randomNum);

		//if random num genrateed is 26 add white space & print to stdout
		if(randomNum == 26) 
		{
			c = ' ';
			fprintf(stdout,"%c", c);
		}
		//else genrate random char with offset from A & print to stdout
		else
		{
			c = 'A' + randomNum;
			fprintf(stdout,"%c", c);
		}
	}	

	//add newline char
	fprintf(stdout,"\n");

	return 0;	
}
