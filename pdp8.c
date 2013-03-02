#include <stdio.h>
#include <stdlib.h>

enum {AND,TAD,ISZ,DCA,JMS,JMP,IOT,OPR};

struct CPU{
unsigned short IF:3;   // instruction field
unsigned short IB:3;   // instruction buffer
unsigned short PC:12;  // program count
unsigned short DF:3;   // data field
unsigned short indirect:12;
unsigned short link:1;
unsigned short AC:12;
unsigned short MQ:12;
};

unsigned short mem[65535];

unsigned short FPswitches;

typedef void device_f(unsigned short func,unsigned short dev_num);
device_f *devs[32];

struct CPU CPU;




unsigned short M(unsigned short inst) {
int I = inst & (1<<8);
int Z = inst & (1<<7);
unsigned short disp = inst & 0x7f;
unsigned short page = Z ? (CPU.PC & ~0x7f) : 0;
unsigned short addr = (page + disp) & 0x0fff;
//printf("I:%d Z:%d DISP:%o PAGE:%o ADDR:%o \n",I,Z,disp,page,addr);
if ( !I ){ return addr; }
return mem[addr] & 0x0fff;
}






char * num_to_binary(unsigned short num)
{
	static char bin[13]; 	
	int i;
	for (i=0; i<12; i++)
		bin[11-i] = (num & (1<<i)) ? '1' : '0';
	bin[12] = 0;
	return bin;
}







void blinken_lights(void){
printf("IF: %s\n", num_to_binary(CPU.IF));
printf("IB: %s\n", num_to_binary(CPU.IB));
printf("PC: %s\n", num_to_binary(CPU.PC));
printf("DF: %s\n", num_to_binary(CPU.DF));
printf("indirect: %s\n", num_to_binary(CPU.indirect));
printf("link: %s\n", num_to_binary(CPU.link));
printf("AC: %s\n", num_to_binary(CPU.AC));
printf("MQ: %s\n", num_to_binary(CPU.MQ));
}






void do_iot(unsigned short inst){
	unsigned short dev_num = (inst >> 3) & 0x1f;
	device_f * dev = devs[dev_num];
	if (!dev) {
		printf("no such device %04o\n", dev_num);
		exit(1);
	}
	else {
		dev(inst & 0x7,dev_num);
	}
}




void do_mmu(unsigned short func,unsigned short dev_num){
   if (func==1){CPU.DF=dev_num&07;}  //CDF
   if (func==2){CPU.IB=dev_num&07;}  //CIF
   if (func==3){CPU.IB=dev_num&07;  CPU.DF=dev_num&07;}  //CDI
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
mem[0000]=00000;
mem[0001]=02165;
mem[0002]=05053;
mem[0003]=07604;
mem[0004]=07041;
mem[0005]=01172;
mem[0006]=07440;
mem[0007]=05012;

mem[0010]=02166;
mem[0011]=05051;
mem[0012]=04057;
mem[0013]=03000;
mem[0014]=04057;
mem[0015]=06206;
mem[0016]=00012;
mem[0017]=00001;

mem[0020]=04057;
mem[0021]=00156;
mem[0022]=07450;
mem[0023]=01151;
mem[0024]=01154;
mem[0025]=07410;
mem[0026]=05124;
mem[0027]=03170;

mem[0030]=07404;
mem[0031]=03172;
mem[0032]=01172;
mem[0033]=00152;
mem[0034]=07001;
mem[0035]=03046;
mem[0036]=01172;
mem[0037]=07002;

mem[0040]=00152;
mem[0041]=03163;
mem[0042]=04057;
mem[0043]=00163;
mem[0044]=07001;
mem[0045]=04067;
mem[0046]=00000;
mem[0047]=07040;

mem[0050]=03166;
mem[0051]=01162;
mem[0052]=03165;
mem[0053]=06046;
mem[0054]=01170;
mem[0055]=06005;
mem[0056]=05400;
mem[0057]=00000;

mem[0060]=01171;
mem[0061]=04067;
mem[0062]=05545;
mem[0063]=01157;
mem[0064]=03171;
mem[0065]=01166;
mem[0066]=05457;
mem[0067]=00000;

mem[0070]=03167;
mem[0071]=01155;
mem[0072]=03164;
mem[0073]=03165;
mem[0074]=03166;
mem[0075]=01167;
mem[0076]=07104;
mem[0077]=03167;

mem[0100]=07420;
mem[0101]=05107;
mem[0102]=01467;
mem[0103]=01165;
mem[0104]=03165;
mem[0105]=07420;
mem[0106]=02166;
mem[0107]=02164;

mem[0110]=05115;
mem[0111]=07300;
mem[0112]=01165;
mem[0113]=02067;
mem[0114]=05467;
mem[0115]=01165;
mem[0116]=07104;
mem[0117]=03165;

mem[0120]=01166;
mem[0121]=07004;
mem[0122]=03166;
mem[0123]=05075;
mem[0124]=07200;
mem[0125]=03171;
mem[0126]=01161;
mem[0127]=03130;

mem[0130]=06211;
mem[0131]=07200;
mem[0132]=01171;
mem[0133]=00153;
mem[0134]=01160;
mem[0135]=03571;
mem[0136]=02171;
mem[0137]=05132;
 
mem[0140]=01130;
mem[0141]=01151;
mem[0142]=03130;
mem[0143]=01130;
mem[0144]=00156;
mem[0145]=07440;
mem[0146]=05130;
mem[0147]=06201;

mem[0150]=05012;
mem[0151]=00010;
mem[0152]=00077;
mem[0153]=00177;
mem[0154]=00200;
mem[0155]=07764;
mem[0156]=00070;
mem[0157]=00541;

mem[0160]=05200;
mem[0161]=06211;
mem[0162]=07770;

}





int main (void){

  devs[020]=do_mmu;
  devs[021]=do_mmu;
  devs[022]=do_mmu;
  devs[023]=do_mmu;
  devs[024]=do_mmu;
  devs[025]=do_mmu;
  devs[026]=do_mmu;
  devs[027]=do_mmu;

  setup_mem();

CPU.PC=026;

  for (;;){
   unsigned short inst = mem[CPU.IF<<12|CPU.PC];
   
   blinken_lights();

   switch (inst>>9){
	
   case AND:{
        CPU.AC&=mem[ CPU.DF<<12|M(inst) ];
        CPU.PC++;
       break;}

   case TAD:{
        unsigned short oldAC=CPU.AC;
	CPU.AC+=mem[ CPU.DF<<12|M(inst) ];
        if (oldAC > CPU.AC) {CPU.link^=1;}
        CPU.PC++;   
       break;}    

   case ISZ:{
        unsigned short m = CPU.DF<<12|M(inst);
        unsigned short oldMEM=mem[m];
        unsigned short newMEM=oldMEM+1; 
        newMEM&=0xfff;
        mem[m]=newMEM;
        if (newMEM==0){CPU.PC++;}
        CPU.PC++;   
       break;}

   case DCA:{
        mem[CPU.DF<<12|M(inst)]=CPU.AC;
        CPU.AC=0;
        CPU.PC++;   
       break;}

   case JMS:{
       CPU.IF=CPU.IB;
       unsigned short m = M(inst);
       mem[m]=CPU.PC+1;
       CPU.PC=m+1;
      break;}

   case JMP:{
       CPU.IF=CPU.IB;
       unsigned short ADDR=M(inst);
       CPU.PC=00777&ADDR;
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

