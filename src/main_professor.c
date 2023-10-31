#include <stdio.h>
#include <stdlib.h>

//lex.yy.h
int yylex();
extern char *yytext;
extern FILE *yyin;


int isRunning(void);
void initMe(void);

int main(int argc, char** argv)
  {
  FILE *gold = 0;
  int token = 0;
  int answar = 0;
  int nota = 0;
  int i=1;
      fprintf(stderr,"Rodando main do prof. \n");

  if (argc < 3)
    {
    printf("call: ./etapa1 input.txt output.txt \n");
    exit(1);
    }
  if (0==(yyin = fopen(argv[1],"r")))
    {
    printf("Cannot open file %s... \n",argv[1]);
    exit(1);
    }
  if (0==(gold = fopen(argv[2],"r")))
    {
    printf("Cannot open file %s... \n",argv[2]);
    exit(1);
    }
  initMe();
  while (isRunning())
    {
    token = yylex();
    
    if (!isRunning())
      break;
    fscanf(gold,"%d",&answar);
    if (token == answar)
      {
      fprintf(stderr,"%d=ok(%s)  ",i,yytext  );
      ++nota;
      }
    else
      fprintf(stderr,"\n%d=ERROR(%s,%d,%d) ",i,yytext,token,answar );
    ++i;
    }
  printf("NOTA %d\n\n",nota);  
  fprintf(stderr,"NOTA %d\n\n",nota);  
  }