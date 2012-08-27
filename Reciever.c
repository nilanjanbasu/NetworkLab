#include"netlib.h"

void send_ACK(int,char);

int main(int argc,char *argv[])
{
	char filename[100];
	char frame[BUFFER_SIZE];
	int count,i,still_coming=1,rcv=0;
	FILE *fp;
	int reader,writer;
	sscanf(argv[1],"%d %d %s",&reader,&writer,filename);
	fp = fopen(filename,"wb");
	if(fp == NULL)
	{
		send_ACK(writer,0);
		exit(EXIT_FAILURE);
	}
	else
		send_ACK(writer,2);
	
	while(still_coming)
	{
		rcv = read(reader,frame,BUFFER_SIZE);
		printf("RECEIVER: Read %d bytes---",rcv);
		if(rcv < BUFFER_SIZE || !validate(frame))
		{
			send_ACK(writer,0); //Send NACK
			printf("RECEIVER: Sending NACK, damaged frame\n");
			continue;
		}
		else
		{
			printf("Frame Number: %d\n",frame[DATA_SIZE]);
		}
		send_ACK(writer,1);
		for(i=0;i<DATA_SIZE;++i)
		{
			if(frame[i] == (char)0xff)
			{
				still_coming = 0;
				break;
			}
		}
		if(i>0)
			fwrite(frame,sizeof(char),i,fp);		
	}
	fclose(fp);
	close(reader);
	close(writer);
	printf("RECEIVER: Exiting successfully\n");
	exit(EXIT_SUCCESS);
}
void send_ACK(int writer,char value)
{
	write(writer,&value,1);
}
