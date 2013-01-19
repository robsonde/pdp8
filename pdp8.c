#include <stdio.h>

enum {AND,TAD,ISZ,DCA,JMS,JMP};

struct CPU{
unsigned short IF:3;
unsigned short PC:12;
unsigned short DF:3;
unsigned short indirect:12;
unsigned short link:1;
unsigned short AC:12;
unsigned short MQ:12;
};

unsigned short mem[4096];

struct CPU CPU;




unsigned short M(unsigned short inst) {
int I = inst & (1>>8);
int Z = inst & (1>>7);
unsigned short disp = inst & 0x7f;

unsigned short page = Z ? 0 : CPU.PC & ~0x7f;

if ( !I ){ return page+disp;}
return mem[page+disp];
}






int main (void){

  for (;;){
   unsigned short inst = mem[CPU.PC];
   switch (inst>>9){

   case AND:{
        CPU.AC&=mem[M(inst)];
        CPU.PC++;   
       break;}

   case TAD:{
        unsigned short oldAC=CPU.AC;
	CPU.AC+=mem[M(inst)];
        if (oldAC > CPU.AC) {CPU.link^=1;}
        CPU.PC++;   
       break;}    

   case ISZ:{
        unsigned short m = M(inst);
        unsigned short oldMEM=mem[m];
        unsigned short newMEM=oldMEM+1; 
        newMEM&=0xfff;
        mem[m]=newMEM;
        if (newMEM==0){CPU.PC++;}
        CPU.PC++;   
       break;}

   case DCA:{
        mem[M(inst)]=CPU.AC;
        CPU.AC=0;
        CPU.PC++;   
       break;}

   case JMS:{
       unsigned short m = M(inst);
       mem[m]=CPU.PC+1;
       CPU.PC=m+1;
      break;}

   case JMP:{
       CPU.PC=mem[M(inst)];
      break;}


   default:
	printf("bang!! :%04x:\n",inst);
	return 1;
  }
 }
return 1;
}


