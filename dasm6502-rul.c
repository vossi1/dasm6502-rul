// dasm6502-rul.c
// removes unused labels in acme assembler source code
// written by Vossi 02/2019 in Hamburg/Germany
// version 1.0 copyright (c) 2019 Vossi. All rights reserved.

// removes all lxxxx: labels in first position of line, if no branch or jmp/jsr uses it.


#include <stdio.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned short word;

//static int option;

int main(int argc,char *argv[])
{
  FILE *f, *f2;
  int i, j, n, n2, lines, labels, ok;
  char source[40], target[40];
  char sourceext[5]=".a";
  char targetext[8]="_rul.a";

  byte buf[10000][160];
  byte label[10000][5];
  
  for(n=1;(n<argc)&&(*argv[n]=='-');n++)  // loop for argc>1 & starting with -
    switch(argv[n][1])                    // check first character
    {
//      case 'o': option=1; break;
      default:
        fprintf(stderr,"%s: Unknown option -%c\n",argv[0],argv[n][1]);
    }
  
  if(n==argc)  
  {
    fprintf(stderr,"dasm6502-rul v1.0 by Vossi 02/2019\n");
    fprintf(stderr,"removes unused labels in acme assembler source code\n");
    fprintf(stderr,"usage: %s [-p] .a-file[w/o extension]\n",argv[0]);
//    fprintf(stderr,"  -o - option -> unused;)\n");
    return(1);
  }
  
  strcpy(source,argv[n]);
  strcpy(target,argv[n]);
  strcat(source,sourceext);
  strcat(target,targetext);
  if(!(f=fopen(source,"rb")))
    {printf("\n%s: Can't open file %s\n",argv[0],argv[n]); return(1);}
  
  n = 0; i = 0;
  while (fread(buf[i],1,1,f))                 // read all lines of file in buf array
  {
    if(buf[i][0] != 0x0a)
    {
      do
      {
        n++;
        fread(buf[i]+n,1,1,f);
      } while(buf[i][n] != 0x0a);
      n = 0;
      i++;
    }
    else
      i++;
  }
  lines = i;                                 // lines total
  printf("Source lines read: %d\n",lines);

  labels = 0;
  for(i=0;i<lines;i++)                      // search for branch labels "lxxxx"
  {
    n = 1;
    n2 = 0;
    if(buf[i][0] != ';')
    {
      do
      {
        if(buf[i][n] == 'l')
        {
          label[labels][0] = 'l';
          n2 = 1;
        }
        else
        {
          if(n2 > 0)
          {
            if((buf[i][n] >= '0' && buf[i][n] <= '9') || (buf[i][n] >= 'a' && buf[i][n] <= 'f'))
            {
              label[labels][n2] = buf[i][n];
              n2++;
              if(n2 == 5)
              { 
                printf("%c%c%c%c%c,",label[labels][0],label[labels][1],label[labels][2],label[labels][3],label[labels][4]);
                labels++;
              }  
            }
            else
              n2 = 0;
          }
        }  
        n++;
      } while(buf[i][n-1] != 0x0a && buf[i][n-1] != ';');
    }
  }


  for(i=0;i<lines;i++)                      // remove unused labels "lxxxx"
  {
    n = 1;
    if(buf[i][0] == 'l')
    {
      ok = 0;
      j = 0;
      do
      {
        n2 = 1;
        while(buf[i][n2] == label[j][n2]) n2++;
        if(n2 == 5) ok = 1;
        j++;
      } while(ok == 0 && j < labels);
      if(ok == 0)
      {
        buf[i][0] = 0x09;                   // replace first char with tab
        j = 1;
        do
        {
          buf[i][j] = buf[i][j+5];
          j++;
        } while(buf[i][j+4] != 0x0a);
      }
    }
  }


  if(!(f2=fopen(target,"wb")))
    {printf("\n%s: Can't open target file %s\n",argv[0],target); fclose(f); return(1);}
  for(i=0; i<lines; i++)
  {  
    n = 0;
    do
    {
      fwrite(buf[i]+n,1,1,f2);
      n++;
    } while(buf[i][n-1] != 0x0a);
  }
  fclose(f);fclose(f2); return(0);
}
