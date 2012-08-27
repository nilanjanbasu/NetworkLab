#include <string.h>
#include"netlib.h"

int wait_for_ACK(int,int);
void sender(FILE *,int,int,float);
int main(int argc,char *argv[])
{
	int data_processed;
	int pipe1[2],pipe2[2];
	FILE *fp;
	char buffer[BUFSIZ + 1];
	pid_t child_process;
	int probability_given = 0;
	
	if(argc < 3)
	{
		fprintf(stderr,"Too few arguments!\n");
		exit(EXIT_FAILURE);
	}
	if(argc == 4)
		probability_given = 1;
	fp = fopen(argv[1],"rb");
	if(fp == NULL)
	{
		fprintf(stderr,"Input file does not exists");
		exit(EXIT_FAILURE);
	}
	memset(buffer, '\0', sizeof(buffer));
	if (pipe(pipe1) == 0 && pipe(pipe2) == 0)
	{
		child_process = fork();
		if(child_process == (pid_t)-1)
		{
			fprintf(stderr,"Server/client connection could not be established\n");
			exit(EXIT_FAILURE);
		}
		if(child_process == 0) /*for child*/
		{
			sprintf(buffer,"%d %d %s",pipe1[0],pipe2[1],argv[2]);
			execl("receiver","receiver",buffer,(char *)0);
			exit(EXIT_FAILURE);
		}
		if(child_process > 0) /*for parent*/
		{
			if(probability_given)
				sender(fp,pipe1[1],pipe2[0],atof(argv[3]));
			else
				sender(fp,pipe1[1],pipe2[0],-1);
			exit(EXIT_SUCCESS);
		}		
	}
	exit(EXIT_FAILURE);
}
void sender(FILE *fp,int p_write,int p_read,float probability)
{
	char buffer[BUFFER_SIZE],temp_buffer[BUFFER_SIZE];
	char status_buff[200];
	int status,i,success=0,padded = 0,count=1;
	memset(buffer,'\0',sizeof(buffer));
	
	status = wait_for_ACK(p_read,2); /*2 means reciever ready*/
	if(status == 0)
	{
		fprintf(stderr,"SENDER: The reciever was not able to initialize\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("SENDER:Connection Established!\n");
	}
	init_random_generator();
	while(!feof(fp))
	{
		success = fread(buffer,sizeof(char),DATA_SIZE,fp);
		if(success < DATA_SIZE)
		{
			sprintf(status_buff,"%d bytes fetched from file, %d bytes padded",success,8-success);
			memset(buffer+success,(char)0xff,DATA_SIZE-success);
			padded = 1;
		}
		else
			sprintf(status_buff,"%d bytes fetched from file, 0 bytes padded",success);
		buffer[DATA_SIZE] = (char)count;
		
		CRC(buffer);
		memcpy(temp_buffer,buffer,BUFFER_SIZE);
		if(probability >=0)
		{
			//printf("SENDER: Probability %f\n",probability);
			damage(temp_buffer,probability);			
		}
		
		printf("SENDER:%s, Sent %d Bytes: Frame Number: %d\n",status_buff,BUFFER_SIZE,count);
		write(p_write,temp_buffer,BUFFER_SIZE);		
		while(!wait_for_ACK(p_read,1))
		{
			printf("SENDER: NAK received... Sending frame #%d again\n",count);
			memcpy(temp_buffer,buffer,BUFFER_SIZE);
			if(probability >=0)
			{
				//printf("SENDER: Probability %f\n",probability);
				damage(temp_buffer,probability);			
			}
			write(p_write,temp_buffer,BUFFER_SIZE);
		}
		count++;
	}
	if(!padded)
	{
		memset(buffer,0xff,BUFFER_SIZE);
		write(p_write,buffer,BUFFER_SIZE);
		printf("SENDER: Sending padding frame(file-bytes exact multiple of 8) frame #%d\n",count);
	}
	fclose(fp);
	close(p_write);
	close(p_read);
	printf("SENDER: Exiting successfully\n");
	exit(EXIT_SUCCESS);
}
int wait_for_ACK(int descriptor,int value_expected)
{
	char x;
	read(descriptor,&x,1);
	if(x == value_expected)
		return 1;
	else
		return 0;
}
