#include <stdio.h>
#include <stdlib.h>

enum {AND,TAD,ISZ,DCA,JMS,JMP,IOT,OPR};

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

typedef void device_f(unsigned short func);
device_f *devs[32];

struct CPU CPU;




unsigned short M(unsigned short inst) {
int I = inst & (1>>8);
int Z = inst & (1>>7);
unsigned short disp = inst & 0x7f;

unsigned short page = Z ? 0 : (CPU.PC & ~0x7f);
unsigned short addr = (page + disp) & 0x0fff;
if ( !I ){ return addr; }
return mem[addr] & 0x0fff;
}








void do_iot(unsigned short inst){
	unsigned short dev_num = (inst >> 3) & 0x1f;
	device_f * dev = devs[dev_num];
	if (!dev) {
		printf("no such device %d\n", dev_num);
		exit(1);
	}
	else {
		dev(inst & 0x7);
	}
}



void do_opr(void){

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


   case IOT:{
         do_iot(inst);
        CPU.PC++;   
      break;}


   case OPR:{
         do_opr();
        CPU.PC++;   
      break;}


   default:
	printf("bang!! :%04x:\n",inst);
	return 1;
  }
 }
return 1;
}


