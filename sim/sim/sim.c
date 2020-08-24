// sim.c : This file contains the 'main' function. Program execution begins and ends there.



#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define TRUE 1 
#define FALSE 0


#define MAX_LINES 4096
#define SIZE 9
#define REG_NUM 16
#define SECTORS_NUM 128
#define SECTOR_SIZE 128


int irq2_cycle = -1;
char disk_matrix[SECTORS_NUM*SECTOR_SIZE][SIZE];
char memory_in[MAX_LINES][SIZE];
int max_line_disk = 0;
int irq = 0;
int irq_flg = FALSE;
int disk_counter_cycles = 0;
int total_cycles = 0;
int pc = 0;
int max_line_mem_counter = 0;
int reg[REG_NUM] = { 0 };
char line[SIZE];
char name_ioreg[15];

//IO Registers
unsigned int irq0enable = 0;
unsigned int irq1enable = 0;
unsigned int irq2enable = 0;
unsigned int irq0status = 0;
unsigned int irq1status = 0;
unsigned int irq2status = 0;
unsigned int irqhandler = 0;
unsigned int irqreturn = 0;
unsigned long clks = 0;
unsigned int leds = 0;
unsigned int display = 0;
unsigned int timerenable = 0;
unsigned long timercurrent = 0;
unsigned int timermax = 0;
unsigned int diskcmd = 0;
unsigned int disksector = 0;
unsigned int diskbuffer = 0;
unsigned int diskstatus = 0;


int opcode;
int rd;
int rs;
int rt;
int imm;

FILE* memin, *diskin, *irq2in, *memout, *regout, *trace, *hwregtrace, *cycles, *ledsF, *displayF, *diskout;

/*Utility func*/
int getHex3(char* source);
int getHex8(char* source);
int hex2int(char ch);
int getAddress(int address);

/*Create func*/
void createTrace();
void createLastFiles();


/*Line Func*/
void add();
void sub();
void andf();
void orf();
void sll();
void sra();
void srl();
void beq();
void bne();
void blt();
void bgt();
void ble();
void bge();
void jal();
void lw();
void sw();
void reti();
void in();
void out();
void halt();

void clk_loop();
void check_disk();
void check_timer();
void decipher_line();


int main(int argc, const char* argv[])
{
	if (argc < 4)//open and check files
	{
		printf("Arg Amount Error");
		return 0;
	}

	memin = fopen(argv[1], "r");
	diskin = fopen(argv[2], "r");
	irq2in = fopen(argv[3], "r");
	memout = fopen("memout.txt", "w");
	regout = fopen("regout.txt", "w");
	trace = fopen("trace.txt", "w");
	hwregtrace = fopen("hwregtrace.txt", "w");
	cycles = fopen("cycles.txt", "w");
	ledsF = fopen("leds.txt", "w");
	displayF = fopen("display.txt", "w");
	diskout = fopen("diskout.txt", "w");
	if (memin == NULL || diskin == NULL || irq2in == NULL || memout == NULL
		|| regout == NULL || trace == NULL || hwregtrace == NULL || cycles == NULL
		|| ledsF == NULL || displayF == NULL || diskout == NULL)
	{
		printf("FILE Error");
		return 0;
	}

	while (!feof(memin))//memory_in  is filled
	{
		int check;
		if (check = fscanf(memin, "%s", memory_in[max_line_mem_counter]) != 0)
		{
			max_line_mem_counter++;
		}
	}

	fclose(memin);

	while (!feof(diskin))//disk is filled
	{
		int check;
		if (check = fscanf(diskin, "%s", disk_matrix[max_line_disk]) != 0)
			max_line_disk++;
	}
	fclose(diskin);

	if (!feof(irq2in))//update the first irq2
		fscanf(irq2in, "%d", &irq2_cycle);



	while (pc > -1)//Starting the program
	{
		irq = ((irq0enable & irq0status) | (irq1enable & irq1status) | (irq2enable & irq2status));
		if (irq == 1 && irq_flg == FALSE)// check irq
		{
			irq_flg = TRUE;
			irqreturn = pc;
			pc = irqhandler;
		}

		if (irq2_cycle == total_cycles) //check if we need to change irq2status
		{
			irq2status = 1;
			if (!feof(irq2in))//update the next irq2
				fscanf(irq2in, "%d", &irq2_cycle);
		}


		strcpy(line, memory_in[pc]);

		char str[4] = { line[5], line[6], line[7],'\0' };
		imm = getHex3(str);
		reg[1] = imm;

		createTrace();
		decipher_line();
		check_disk();
		check_timer();
		clk_loop();
		total_cycles++;


	}

	createLastFiles();

	fclose(memout);
	fclose(regout);
	fclose(trace);
	fclose(hwregtrace);
	fclose(cycles);
	fclose(ledsF);
	fclose(displayF);
	fclose(diskout);

	fclose(irq2in);
}

void createLastFiles()//Creates the output files
{
	for (int i = 2; i < REG_NUM; i++)//print regout file
	{
		fprintf(regout, "%08X\n", reg[i]);
	}
	for (int i = 0; i < max_line_mem_counter; i++)//print memout
	{
		int instruction = getHex8(memory_in[i]);
		if (instruction < 0)
			instruction -= 0xFFFFFFFF00000000;
		fprintf(memout, "%08X\n", instruction);
	}
	fprintf(cycles, "%ld\n", total_cycles);


	// updates into diskout file the matrix 'disk-out-matrix'
	for (int i = 0; i < SECTORS_NUM * SECTOR_SIZE; i++)
	{
		fprintf(diskout, "%s\n", disk_matrix[i]);
	}

}

void clk_loop() //Updates the clock as needed
{
	if (clks == 0xFFFFFFFF)
		clks = 0;
	else clks++;
}

void createTrace()//Creates the file trace
{
	fprintf(trace, "%08X %s ", pc, line);
	for (int i = 0; i < REG_NUM - 1; i++)
		fprintf(trace, "%08x ", reg[i]);

	fprintf(trace, "%08x\n", reg[REG_NUM - 1]);///to print the last one without tab
}


void decipher_line() //decipher the line
{

	opcode = hex2int(line[0]) * 16 + hex2int(line[1]);
	rd = hex2int(line[2]);
	rs = hex2int(line[3]);
	rt = hex2int(line[4]);


	switch (opcode)
	{
	case 0:	add(); break;	//The opcode is add
	case 1: sub(); break;    //The opcode is sub
	case 2: andf(); break;    //The opcode is and
	case 3: orf(); break;    //The opcode is or
	case 4: sll(); break;    //The opcode is sll
	case 5: sra(); break;    //The opcode is sra
	case 6: srl(); break;    //The opcode is srl
	case 7: beq(); break;    //The opcode is beq
	case 8: bne(); break;    //The opcode is ben
	case 9: blt(); break;    //The opcode is blt
	case 10: bgt(); break;    //The opcode is bgt
	case 11: ble(); break;    //The opcode is ble
	case 12: bge(); break;    //The opcode is bge
	case 13: jal(); break;    //The opcode is jal
	case 14: lw(); break;    //The opcode is lw
	case 15: sw(); break;    //The opcode is sw
	case 16: reti(); break;  //The opcode is reti
	case 17: in(); break;    //The opcode is in
	case 18: out(); break;   //The opcode is out
	case 19: halt(); break;  //The opcode is halt

	default:;

	}
	pc += 1;
	reg[0] = 0;    //Ensures that Register 0 has not changed
}
//Functions of operations
void add()
{
	reg[rd] = reg[rs] + reg[rt];
}
void sub()
{
	reg[rd] = reg[rs] - reg[rt];
}
void andf()
{
	reg[rd] = reg[rs] & reg[rt];
}
void orf()
{
	reg[rd] = reg[rs] | reg[rt];
}
void sll()
{
	reg[rd] = reg[rs] << reg[rt];
}
void sra()
{
	reg[rd] = reg[rs] >> reg[rt];

}

void srl() {
	reg[rd] = (int)((unsigned)reg[rs] >> reg[rt]);

}
void beq() {
	if (reg[rs] == reg[rt])
		pc = (reg[rd] & 0x00000FFF) - 1;
}
void bne() {
	if (reg[rs] != reg[rt])
		pc = (reg[rd] & 0x00000FFF) - 1;

}
void blt() {
	if (reg[rs] < reg[rt])
		pc = (reg[rd] & 0x00000FFF) - 1;

}
void bgt() {
	if (reg[rs] > reg[rt])
		pc = (reg[rd] & 0x00000FFF) - 1;

}
void ble() {
	if (reg[rs] <= reg[rt])
		pc = (reg[rd] & 0x00000FFF) - 1;

}
void bge() {
	if (reg[rs] >= reg[rt])
		pc = (reg[rd] & 0x00000FFF) - 1;

}

void jal() {
	reg[15] = pc + 1;
	pc = (reg[rd] & 0x00000FFF) - 1;
}

void lw()
{
	char  ln1[SIZE];
	int address = getAddress(reg[rs] + reg[rt]);
	strcpy(ln1, memory_in[address]);
	reg[rd] = getHex8(ln1);
}

void sw()
{
	int address = getAddress(reg[rs] + reg[rt]);
	char ch[SIZE];
	sprintf(ch, "%08x", reg[rd]);
	strcpy(memory_in[address], ch);
	if (max_line_mem_counter < address)
		max_line_mem_counter = address;

}

void reti() {
	pc = irqreturn - 1;
	irq_flg = FALSE;
}

void in() {
	int rst = reg[rs] + reg[rt];
	switch (rst)
	{
	case 0: reg[rd] = irq0enable;    strcpy(name_ioreg, "irq0enable");   break;
	case 1: reg[rd] = irq1enable;    strcpy(name_ioreg, "irq1enable");   break;
	case 2: reg[rd] = irq2enable;    strcpy(name_ioreg, "irq2enable");   break;
	case 3: reg[rd] = irq0status;    strcpy(name_ioreg, "irq0status");   break;
	case 4: reg[rd] = irq1status;    strcpy(name_ioreg, "irq1status");   break;
	case 5: reg[rd] = irq2status;    strcpy(name_ioreg, "irq2status");   break;
	case 6: reg[rd] = irqhandler;    strcpy(name_ioreg, "irqhandler");   break;
	case 7: reg[rd] = irqreturn;     strcpy(name_ioreg, "irqreturn");    break;
	case 8: reg[rd] = clks;          strcpy(name_ioreg, "clks");         break;
	case 9: reg[rd] = leds;          strcpy(name_ioreg, "leds");         break;
	case 10: reg[rd] = display;      strcpy(name_ioreg, "display");      break;
	case 11: reg[rd] = timerenable;  strcpy(name_ioreg, "timerenable");  break;
	case 12: reg[rd] = timercurrent; strcpy(name_ioreg, "timercurrent"); break;
	case 13: reg[rd] = timermax;     strcpy(name_ioreg, "timermax");     break;
	case 14:reg[rd] = diskcmd;       strcpy(name_ioreg, "diskcmd");      break;
	case 15:reg[rd] = disksector;    strcpy(name_ioreg, "disksector");   break;
	case 16:reg[rd] = diskbuffer;    strcpy(name_ioreg, "diskbuffer");   break;
	case 17:reg[rd] = diskstatus;    strcpy(name_ioreg, "diskstatus");   break;
	default: strcpy(name_ioreg, "Error");

	}
	fprintf(hwregtrace, "%ld READ %s %08x\n", total_cycles, name_ioreg, reg[rd]);
	reg[0] = 0;    //Ensures that Register 0 has not changed
}

void out() {
	int rst = reg[rs] + reg[rt];
	switch (rst)
	{
	case 0:  irq0enable = (unsigned)reg[rd] & 0x00000001;   strcpy(name_ioreg, "irq0enable");   break;
	case 1:  irq1enable = (unsigned)reg[rd] & 0x00000001;   strcpy(name_ioreg, "irq1enable");   break;
	case 2:  irq2enable = (unsigned)reg[rd] & 0x00000001;   strcpy(name_ioreg, "irq2enable");   break;
	case 3:  irq0status = (unsigned)reg[rd] & 0x00000001;   strcpy(name_ioreg, "irq0status");   break;
	case 4:  irq1status = (unsigned)reg[rd] & 0x00000001;   strcpy(name_ioreg, "irq1status");   break;
	case 5:  irq2status = (unsigned)reg[rd] & 0x00000001;   strcpy(name_ioreg, "irq2status");   break;
	case 6:  irqhandler = (unsigned)reg[rd] & 0x00000fff;   strcpy(name_ioreg, "irqhandler");   break;
	case 7:  irqreturn = (unsigned)reg[rd] & 0x00000fff;    strcpy(name_ioreg, "irqreturn");    break;
	case 8:  printf("we cant change it");                                break;//we cant change it	
	case 9:  leds = (unsigned)reg[rd]; 	     strcpy(name_ioreg, "leds");
		fprintf(ledsF, "%ld %08x\n", total_cycles, reg[rd]);
		break;//update leds file                                                   
	case 10: display = (unsigned)reg[rd];  	 strcpy(name_ioreg, "display");
		fprintf(displayF, "%ld %08x\n", total_cycles, reg[rd]);
		break;//update display file
	case 11: timerenable = (unsigned)reg[rd] & 0x00000001;  strcpy(name_ioreg, "timerenable");  break;
	case 12: timercurrent = (unsigned)reg[rd]; strcpy(name_ioreg, "timercurrent"); break;
	case 13: timermax = (unsigned)reg[rd]; 	 strcpy(name_ioreg, "timermax");     break;
	case 14: if (diskstatus == 0)
	{
		diskcmd = (unsigned)reg[rd] & 0x00000003;
		diskstatus = 1;
		if (diskcmd == 3) diskcmd = 0;

		if (diskcmd == 2)//write
			for (int i = 0; i < SECTOR_SIZE; i++)
				strcpy(disk_matrix[disksector*SECTOR_SIZE + i], memory_in[diskbuffer + i]);

		if (diskcmd == 1)//read
			for (int i = 0; i < SECTOR_SIZE; i++)
				strcpy(memory_in[diskbuffer + i], disk_matrix[disksector*SECTOR_SIZE + i]);
	}
			 else return;//if diskstatus==1 -dont do anything and dont print in hwregtrace
		strcpy(name_ioreg, "diskcmd");      break;

	case 15: if (diskstatus == 0)
		disksector = (unsigned)reg[rd] & 0x0000007f;
			 else return;//if diskstatus==1 -dont do anything and dont print in hwregtrace
		strcpy(name_ioreg, "disksector");   break;

	case 16: if (diskstatus == 0)
		diskbuffer = (unsigned)reg[rd] & 0x00000fff;
			 else   return;//if diskstatus==1 -dont do anything and dont print in hwregtrace
		strcpy(name_ioreg, "diskbuffer");   break;

	case 17: diskstatus = (unsigned)reg[rd] & 0x00000001;
		strcpy(name_ioreg, "diskstatus"); 	break;

	default: strcpy(name_ioreg, "Error");
	}
	fprintf(hwregtrace, "%ld WRITE %s %08x\n", total_cycles, name_ioreg, reg[rd]);
}

void halt()
{
	pc = -10;
}

int getHex3(char* source) //Converts three hexadecimal characters to a number
{
	int n = (int)strtol(source, NULL, 16);
	if (n > 0x7ff)
		n -= 0x1000;
	return n;
}
int getHex8(char* source)//Converts eight hexadecimal characters to a number
{
	int n = (int)strtoul(source, NULL, 16);
	if (n > 0x7fffffff)
		n -= 0x100000000;
	return n;
}
int hex2int(char ch)//Hexadecimal character converter to number
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	printf("Char given, %c, is invalid - non hex character.\n", ch);
	return -1;
}

int getAddress(int address)//Checks if the address is valid
{
	if (address < 0)
	{
		printf("Address given, %X, is invalid - negative.\n", address);
		return -10;
	}

	if (address >= 4096)
	{
		printf("Address given, %X, is invalid - exceeds limited space in memory.\n", address);
		address = address & 0x0FFF; //if given address is too high, take only 12 LSBs.
		printf("Simulator refers only to 12 LSBs, %X in this case. \n", address);
	}
	return address;
}

void check_disk()//Checks and performs disk operations
{

	if (disk_counter_cycles == 1024)//finish disk read/write
	{
		irq1status = 1;
		disk_counter_cycles = 0;
		diskstatus = 0;
		diskcmd = 0;
	}

	if (diskstatus == 1)// disk_counter_cycles++ if need
		disk_counter_cycles++;
}
void check_timer()//Checks and performs timer operations
{
	if (timerenable == 1)
	{
		if (timermax == timercurrent)
		{
			irq0status = 1;
			timercurrent = 0;
		}
		else timercurrent++;
	}

}

