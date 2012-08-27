#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 11
#define DATA_SIZE 8
#define N 16 //number of bits in generator polynomial

#define c_size 8*sizeof(char)

void init_random_generator()
{
	srand((unsigned)time(0));
}
void damage(char *fr,float p)
{
    float p1,p2;
    int b1,b2,b3;    
	
    rand();
    p1=(float)rand()/(float)RAND_MAX;
    if(p<p1)
        return ;
    else
    {
         p2=(float)rand()/(float)RAND_MAX;
         if(p2>.8 & p2<=1)
         {
             rand();
             b1=((float)rand()/(float)RAND_MAX)*88;
            // srand(time(NULL));
             rand();
             b2=((float)rand()/(float)RAND_MAX)*88;
            // srand(time(NULL));
             rand();
             b3=((float)rand()/(float)RAND_MAX)*88;
             fr[b1/c_size]^=1<<b1%c_size;
             fr[b2/c_size]^=1<<b2%c_size;
             fr[b3/c_size]^=1<<b3%c_size;
         }
         else if(p2>.5 &&p2<=.8)
         {
             // srand(time(NULL));
              rand();
             b1=((float)rand()/(float)RAND_MAX)*88;
          //   srand(time(NULL));
             rand();
             b2=((float)rand()/(float)RAND_MAX)*88;
             fr[b1/c_size]^=1<<b1%c_size;
             fr[b2/c_size]^=1<<b2%c_size;
         }
         else if(p2<=.5)
         {
             //srand(time(NULL));
             rand();
             b1=((float)rand()/(float)RAND_MAX)*88;
             fr[b1/c_size]^=1<<b1%c_size;
         }
    }
}
void CRC(unsigned char  *t)
{
    int g=0x8EDB,i,e,x;     //generator polynomial1000111011011011
    char fr[11];
    for(i=0;i<2;++i)
    {
        t[i+9]=0;
        fr[i+9]=0;
    }
     for(i=0;i<9;++i)
     fr[i]=t[i];
    for(e=0;e<N;++e)
        t[e/c_size+9]|=fr[e/c_size] & 1<<e%c_size;
    do
    {
        if(t[9]>>(c_size-1))
        {
            t[9]=t[9]^((g & 0xFF00)>>8);
            t[10]=t[10]^(g & 0x00FF);

        }
        for(i=c_size-1;i>=1;--i)
        {
            x=(t[9]&(1<<(i-1)));
            if(x)
            t[9]|=(t[9]&(1<<(i-1)))<<1;
            else
            t[9]&=~(1<<i);
        }
        x=(t[10] & 1<<(c_size-1))>>(c_size-1);
        if(x)
        t[9]|=(t[10] & 1<<(c_size-1))>>(c_size-1);
        else
        t[9]&=~1;
         for(i=c_size-1;i>=1;--i)
        {
            x=(t[10]&(1<<(i-1)));
            if(x)
            t[10]|=(t[10]&(1<<(i-1)))<<1;
            else
            t[10]&=~(1<<i);

        }
         x=(fr[e/c_size]& 1<<(c_size-e%c_size-1))>>(c_size-e%c_size-1);
        if(x)
        t[10]|=(fr[e/c_size]& 1<<(c_size-e%c_size-1))>>(c_size-e%c_size-1);
        else
        t[10]&=~1;
        ++e;

    }while(e<=(BUFFER_SIZE*c_size-1));
}
int validate(char *fr)   //returns 1 if error free else returns 0
{
    char cs[2];
    int i;
    for(i=9;i<=10;++i)
		cs[i-9]=fr[i];
    CRC(fr);
    if(cs[0]==fr[9] && cs[1]==fr[10])
		return 1;
    else
		return 0;
}
