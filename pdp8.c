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

unsigned short FPswitches;

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







void dump(void){
   unsigned short inst = mem[CPU.PC];
	printf("PC:%04o:\n",CPU.PC);
	printf("AC:%04o:\n",CPU.AC);
	printf("INST:%04o:\n",inst);
        getchar();
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




void do_opr(unsigned short inst){

 if (~inst&(1<<8)){    //group 1 
    if ((inst&07200)==07200){CPU.AC=0;}  //CLA
    if ((inst&07100)==07100){CPU.link=0;}  //CLL
    if ((inst&07040)==07040){CPU.AC^=07777;}  //CMA
    if ((inst&07020)==07020){CPU.link^=1;}  //CML
    if ((inst&07001)==07001){unsigned short oldAC=CPU.AC; //IAC
                             CPU.AC++;
                             if (oldAC > CPU.AC) {CPU.link^=1;} }
    if ((inst&07010)==07010){CPU.AC=CPU.AC>>1;}  //RAR
    if ((inst&07004)==07004){CPU.AC=CPU.AC<<1;}  //RAL
    if ((inst&07012)==07012){CPU.AC=CPU.AC>>2;}  //RTR
    if ((inst&07006)==07006){CPU.AC=CPU.AC<<2;}  //RTL
    if ((inst&07002)==07002){CPU.AC=(CPU.AC<<6)|(CPU.AC>>6);}  //BSW
}

 if ((inst&0x109)==0x100){  //group 2 OR
    if ((inst&07500)==07500){if (CPU.AC&(1<<11)){CPU.PC++;}}  //SMA
    if ((inst&07440)==07440){if (CPU.AC==0){CPU.PC++;}}  //SZA
    if ((inst&07420)==07420){if (CPU.link==1){CPU.PC++;}}  //SNL
    if ((inst&07600)==07600){CPU.AC=0;}  //CLA
 }

 if ((inst&0x109)==0x108){  //group 2 AND
   if ((inst&07510)==07510){if ((CPU.AC&(1<<11))!=0400){CPU.PC++;}}  //SPA 
   if ((inst&07450)==07450){if (CPU.AC!=0){CPU.PC++;}}  //SNA
   if ((inst&07430)==07430){if (CPU.link==0){CPU.PC++;}}  //SZL
   if ((inst&07610)==07610){CPU.AC=0;}  //CLA
 }

 if ((inst&0x109)==0x108){  //priv group 2
   if ((inst&07404)==07404){CPU.AC^=FPswitches;}  //OSR
   if ((inst&07402)==07402){}  //HLT  FIXME
 }

 if ((inst&0x101)==0x101){  //group 3
   if ((inst&07601)==07601){CPU.AC=0;}  //CLA
   if ((inst&07501)==07501){CPU.AC^=CPU.MQ;}  //MQA
   if ((inst&07421)==07421){CPU.MQ=CPU.AC; CPU.AC=0;}  //MQL
 }
}







void setup_mem(void){
mem[0]=05200;         //JMP 0200
mem[0200]=07300;
mem[0201]=01300;
mem[0202]=01301;
mem[0203]=03302;
mem[0204]=07402;
mem[0205]=05200;
mem[0300]=00002;
mem[0301]=00003;
}





int main (void){

  setup_mem();

  for (;;){
   unsigned short inst = mem[CPU.PC];
   
   dump(); //debug

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
         do_opr(inst);
        CPU.PC++;   
      break;}

   default:
	printf("bang!! :%04x:\n",inst);
	return 1;
  }
 }
return 1;
}

