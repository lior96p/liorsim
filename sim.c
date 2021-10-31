#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#define MAX_COMMAND_LENGTH 6 // Maximum Instruction Encryption
/*DATA STRUCTURES*/
/* Linked List data structure*/

typedef struct memory
{
	int address;   // address is the number of line in mem file
	int data;  // data= what is stored in the line
	struct memory* next;
} memory;

//address - number of line in *mem file
// data - information contained in line
// result - creates memory node for memory linked-list

//Constructor Function for memory node in linked-list
memory* create_node(int data, int address) {
	memory* new_mem = (memory*)malloc(sizeof(memory));
	if (new_mem != NULL) {
		new_mem->address = address;
		new_mem->data = data;
		new_mem->next = NULL;
	}
	return new_mem;
}

//input arguments:
//memory* head - pointer to head of linked-list
// linenum - address argument
// data - data argument
// the function receives pointer to head of linked list and insert a new node to linked list containing the
// linenum and data arguments. the linked list is always sorted by address
memory* insert(memory* head, int linenum, int data) {
	if (linenum < 0)
	{
		return head;
	}
	memory* iter, *prev = NULL;
	memory* new_memory = create_node(data, linenum); /* create new node */
	if (new_memory == NULL) {
		return NULL; /* check for failure */
	}
	if (head == NULL) return new_memory;

	if (new_memory->address < head->address)
	{ /* check if mems's address is minimal in linked-list*/
		new_memory->next = head;
		return new_memory;
	}
	iter = head;
	while ((iter != NULL) && (new_memory->address > iter->address)) {
		/*iterate the list until node-> address is smaller than node->next->address
		*/
		prev = iter;
		iter = iter->next;
	}
	if ((iter == NULL) || (iter->address > new_memory->address)) {
		prev->next = new_memory; //if address doesnt exist in list - insert it in ascending order
		new_memory->next = iter;
	}
	else { // if address already exits - override it
		iter->data = new_memory->data;
		free(new_memory);

	}
	return head;
}
// receives pointer to head of linked-list and searches a node by address.
// if node exists - return a pointer  to the node. else, return NULL
memory* search(memory* head, int address) {
	memory* ptr = head;
	while (ptr->address != address) {
		ptr = ptr->next;
		if (ptr == NULL) {
			return NULL;
		}
	}
	return ptr;
}

// free allocated memory for linked list
void free_Array(memory* head)
{
	memory* curr_node;
	while (head != NULL)
	{
		curr_node = head;
		head = head->next;
		free(curr_node);
	}
}

//input arguments: pointer to diskin/ dmemin files
// the function iterates over input's lines, creating a memory node whereas,
// node->address contains the current row number and node->data contains the data in that line
// the function has a tailor-made solution for dealing with data containing negative numbers by using several conditions
// the result output is a memory linked-list containing memory nodes as specified.

memory* loadmemory(FILE* input)
{
	memory* head = NULL;
	int data;
	int i;
	long negative = 0x80000000, test = 0;
	char temp[9];
	char nstring[9];
	char *tempptr;
	char tempchar[2];
	tempchar[1] = '\0';
	int address = 0;
	//our program faces a problem when a number in input has its last bit turned on,
	//meaning last char is bigger than 8 (in hex base).
	//The following conditions and loop are called to solve problem and generate linked-list
	while (fgets(temp, 9, input) != 0)
	{
		if (temp[0] == '\n') continue; // if line is blank continue
		tempchar[0] = temp[0]; // sampling last character (hex) of current line
		test = (int)strtol(tempchar, NULL, 16); //converting hex char to decimal
		// going over all possibilities for last character, given that last bit is 1
		if (test >= 8)
		{
			test = test & 0x7; // test will now hold all the bits of last character except for the signed-bit
			for (i = 1; i < 8; i++)
			{
				nstring[i] = temp[i];
			}
			nstring[8] = '\0';
			//nstring is now a string that contains the entire data
			// we change nstring's last char according to test
			if (test == 0) nstring[0] = '0';
			if (test == 1) nstring[0] = '1';
			if (test == 2) nstring[0] = '2';
			if (test == 3) nstring[0] = '3';
			if (test == 4) nstring[0] = '4';
			if (test == 5) nstring[0] = '5';
			if (test == 6) nstring[0] = '6';
			if (test == 7) nstring[0] = '7';
			tempptr = nstring; // Temporary pointer to point at nstring
			//tempptr holds the data except for the signed-bit and negative is the minimal negative number.
			// we add the two variables together identical to 2's complement.
			data = (double)strtol(tempptr, NULL, 16) + negative;
			head = insert(head, address, data); // insert new memory node with address and modified data after correction
			address++;
			continue;
		}
		data = (int)strtol(temp, NULL, 16);
		head = insert(head, address, data); // insert memory node with data and address
		address++;
	}
	return head;
}

//input arguments:
//FILE* output - pointer to diskout/ dmemout files
// memory* head - pointer to head of memory linked list containing the addresses and the data in them that
//are meant to be printed  to diskout/dmemout.
// the function is called upon completing running our assembly program.

void printMemory(FILE* output, memory* head)
{
	memory* ptr;
	int address = 0;
	ptr = head;

	while ((ptr != NULL)) {
		if (address == ptr->address) {
			fprintf(output, "%08X\n", ptr->data & 0xffffffff);
			address++;
			ptr = ptr->next;
		}
		else {
			fprintf(output, "00000000\n");
			address++;
		}
	}

}

//input arguments:
//FILE* input - imemin
//int linenum - the line number which we want our file to set its position on
// the function sets the file position row with the number of linenum. The function is called
// upon branch and jal instructions and interruptions
// result argument:
// the function returns 1 if upon reaching the end of our file alerting that linenum is not reachable
// else, it returns 0

int lineplacement(FILE* input, int linenum)
{
	char current_line[MAX_COMMAND_LENGTH];
	fseek(input, 0, SEEK_SET); //revert to first line
	// our cursor starts from first line and moves fowarwad until linenum
	while (linenum > 0)
	{
		fgets(current_line, MAX_COMMAND_LENGTH, input);
		if (feof(input)) return 1;
		if (current_line[0] == '\n') continue;
		linenum = linenum - 1;
	}
	return 0;
}

/*ARITHMATIC OPCODES*/
// the following function behaves exactly as instructed, where we change the value of $rd
// according to the values in $rs and $rt and the action specified
// input arguments:
//int* regptr - pointer to int array at size of 16 where the value stored in the i'th index
// is the value stored in the register encoded to i (in decimal)
//rd, rs, and rt represents encoded registers

void add(int *regptr, int rd, int rs, int rt)
{
	if (rd == 0) return;
	regptr[rd] = regptr[rs] + regptr[rt];
	return;
}
void sub(int *regptr, int rd, int rs, int rt)
{
	if (rd == 0) return;
	regptr[rd] = regptr[rs] - regptr[rt];
	return;
}
void and(int *regptr, int rd, int rs, int rt)
{
	if (rd == 0) return;
	regptr[rd] = regptr[rs] & regptr[rt];
	return;
}
void or (int *regptr, int rd, int rs, int rt)
{
	if (rd == 0) return;
	regptr[rd] = regptr[rs] | regptr[rt];
	return;
}
void xor (int *regptr, int rd, int rs, int rt)
{

	if (rd == 0) return;
	regptr[rd] = regptr[rs] ^ regptr[rt];

	return;
}
void mul(int *regptr, int rd, int rs, int rt)
{
	if (rd == 0) return;
	regptr[rd] = regptr[rs] * regptr[rt];
	return;
}
void sll(int *regptr, int rd, int rs, int rt)
{
	if (rd == 0) return;
	regptr[rd] = regptr[rs] << regptr[rt];
	return;
}
void sra(int *regptr, int rd, int rs, int rt) 
{
	if (rd == 0) return;
	regptr[rd] = regptr[rs] >> regptr[rt];
	return;
}
void srl(int *regptr, int rd, int rs, int rt) 
{
	int n = sizeof(int);
	if (rd == 0) return;
	if (regptr[rt] == 0) regptr[rd] = regptr[rs];
	else {
		regptr[rd] = ((regptr[rs] >> regptr[rt]) & ~(((regptr[rs] >> (n << 3) - 1) << (n << 3) - 1)) >> (regptr[rt] - 1));
	}

	return;
}

/*BRANCH and JAL OPERATIONS*/

// branch file covers beq, bne, blt, bgt, ble and bge instructions where the option argument determines
// the relation we need to check, if existed, between R[$rd] and R[%rs].
//input arguements:
//int regptr - register array
//int rd, rs, and rt represents encoded registers. if condition is met for rt and rs we move our instruction
// to position of R[%rd] [9:0 bits]
// FILE* input - pointer to imemin file
// option- what relation to check between rs and rt
void branch(int *regptr, int rd, int rs, int rt, int *pc, FILE* input, int option)
{
	int x = 0;
	switch (option)
	{
	case 1: //beq
	{
		if (regptr[rs] == regptr[rt])
		{
			break;
		}
		else return;
	}
	case 2: //bne
	{
		if (regptr[rs] != regptr[rt])
		{
			break;
		}
		else return;
	}
	case 3: //blt
	{
		if (regptr[rs] < regptr[rt])
		{
			break;
		}
		else return;
	}
	case 4: //bgt
	{

		if (regptr[rs] > regptr[rt])
		{

			break;
		}
		else return;
	}
	case 5: //ble
	{
		if (regptr[rs] <= regptr[rt])
		{
			break;
		}
		else return;
	}
	case 6: //bge
	{
		if (regptr[rs] >= regptr[rt])
		{
			break;
		}
		else return;
	}

	default:
		return;
	}
	//if condition was unmet we return, meaning condition was met and we use lineplacement to move to different instruction
	// according to the value of the 9:0 bits of R[$rd]
	x = *pc; //we temporarily save current pc address
	*pc = regptr[rd] & (0x3ff);
	if (lineplacement(input, *pc) == 1) {
		//if the returned value is 1 we got unreachable address -> we move back to where the branching occurred
		*pc = x;
		lineplacement(input, x);
	}
	return;   // return with the right pc and the right line
}

//int regptr - register array
//int rd, rs, and rt represents encoded registers. we move our instruction
//  to position to the value in R[rd] [9:0] bits
// FILE* input - pointer to imemin file
void jal(int *regptr, int rd, int rs, int rt, int *pc, FILE* input)
{
	int x = 0;
	x = regptr[15];
	regptr[15] = *pc;  // keep return address
	*pc = regptr[rd] & (0x3ff);
	if (lineplacement(input, *pc) == 1) {
		add(regptr, 0, 0, 0);
		*pc = regptr[15];
		regptr[15] = x;
		lineplacement(input, *pc);
	}
	return;
}

/*LOAD AND STORE INSTRUCTIONS*/

//input arguments:
//int regptr - register array
//int rd, rs, and rt represents encoded registers.
// memory* head - pointer to linked list constructed with the data and address (line number) from diskin/ dmemin files
// the function loads data from address regptr[rs] + regptr[rt] in linked-list to regptr[rd]

void lw(int *regptr, int rd, int rs, int rt, memory* head)
{
	if (head == NULL) {
		// if linked list is NULL it means that the input file was empty -  the data can only be 0
		regptr[rd] = 0;
		return;
	}
	if (rd == 0) return; // loading to register $zero does nothing
	int count = 0, data = 0;
	int linenum = regptr[rs] + regptr[rt]; // address to load from
	if ((linenum < 0) || (linenum > 4096)) {
		//if address is not in our given boundries we will load from the address that shares 10 lower bits of the given address
		// where the rest are 0's so the address is solid
		linenum = linenum & (0xfff);

	}
	memory* ptr = head;
	// we iterate over list - if we reached end of list or the number of nodes we iterated over is bigger than the address,
	// it means the address was initialized with data (and there is no node to represent it) so we load zero.

	while ((ptr->address) != linenum)
	{
		ptr = ptr->next;
		if ((ptr == NULL) || (count > linenum))
		{
			regptr[rd] = 0;
			return;
		}
		count++;
	}
	//if we did reached here, linked-list has a node with address argument of the address we want to load from
	// we put the data argument of the node in register $rd
	data = ptr->data;
	ptr = NULL;
	regptr[rd] = data;
	return;
}

//input arguments:
//int regptr - register array
//int rd, rs, and rt represents encoded registers.
// memory* head - pointer to linked list constructed with the data and address (line number) from diskin/ dmemin files

// the function creates a memory linked-list node where node->address = regptr[rs] + regptr[rt],
// and data= regptr[rd] and insert it to the head argument


void sw(int *regptr, int rd, int rs, int rt, memory* head)
{
	int linenum = regptr[rs] + regptr[rt];

	if (head == NULL) {
		head = insert(head, linenum, regptr[rd]);
		return;
	}

	if ((linenum < 0) || (linenum > 4096)) {
		//if address is not in our given boundries we will load from the address that shares 10 lower bits of the given address
		// where the rest are 0's so the address is solid
		linenum = linenum & (0xfff);
	}
	int count = 0;
	memory* ptr = head;


	//the following loop is used only for assurance as to whether there is a node containing the same address
	// we want store data to
	while ((ptr->address) != linenum)
	{
		ptr = ptr->next;
		if ((ptr == NULL) || (count > linenum))
		{
			head = insert(head, linenum, regptr[rd]);
			return;
		}
		count++;
	}
	ptr->data = regptr[rd];
	return;
}

void hwregout_p(int line_num, int op1, int op2, int* clcptr, int* ioptr, FILE* hwregtrace);// function decleration

/*IO INSTRUCTIONS*/

//input arguments:
//int regptr - register array
//int iogptr -  io register array
//int rd, rs, and rt represents encoded registers.
//FILE* hwregtrace - file pointer
//int* clcptr - pointer to current clock cycle value

void in(int *regptr, int *ioptr, int rd, int rs, int rt, int* clcptr, FILE* hwregtrace) {
	int op2 = 0;
	if (regptr[1] != 0) op2 = 1;
	int line_num = regptr[rs] + regptr[rt];
	if (line_num == 18) {
		// if line_num = 18 we are reading from monitorcmd register so we return the value 0 to $rd as instructed
		regptr[rd] = 0;
		hwregout_p(line_num, 0, op2, clcptr, ioptr, hwregtrace);
		return;
	}
	regptr[rd] = ioptr[line_num]; //update the value in register $rd with the value in hardware register
	hwregout_p(line_num, 0, op2, clcptr, ioptr, hwregtrace);

}


//input arguments:
//int regptr - register array
//int iogptr -  io register array
//int rd, rs, and rt represents encoded registers.
//FILE* leds, hwregtrace - pointer to these files
//int* clcptr - pointer to current clock cycle value
//int screen - 2-dimentional array where screen[i][j] is the data in pixel in that position on the monitor

// the function has multiple effects dependent on regptr[rs] + regptr[rt].
// the function call for hwreout_p to print information followed by in instruction

void out(int *regptr, int *ioptr, int rd, int rs, int rt, int* clcptr, FILE* hwregtrace, FILE* leds, int screen[288][352]) {
	int op2 = 0;
	if (regptr[1] != 0) op2 = 1;
	
	int line_num = regptr[rs] + regptr[rt];
	ioptr[line_num] = regptr[rd];
	//write to hardware register number 9 -> changed our leds -> print new information to leds file
	if (line_num == 9) {
		fprintf(leds, "%d ", *clcptr - 1);
		fprintf(leds, "%08X\n", ioptr[9]);
	}
	//write to hardware register number 18 -> changed our monitor -> update monitor array
	if ((ioptr[18] == 1)) {
		screen[ioptr[20]][ioptr[19]] = ioptr[21];
		//ioptr[18] = 0;

	}
	hwregout_p(line_num, 1, op2, clcptr, ioptr, hwregtrace); // send arguments to print to hwregout_p function
	if (ioptr[18] == 1)
	{
		ioptr[18] = 0;
	}
}


/*INTERRUPTIONS*/


//input arguments:
//int* iogptr -  io register array
//int* irq_ptr - pointer to interrupt request pointer
//int immdetected - immmdeteced is 1 if last instruction used $imm. else it is 0
//int op- op=0 tells us to check if interruption should occur. op=1 will increment timer
// by the time cycles it took the last instruction to be executed

//the function updates timer hardware and checks if interruption is bound to occur and update irq_ptr

void check_irq0(int *ioptr, int* irq_ptr, int immdeteced, int op) {

	if (op == 1) //we only want to deal with the hardware
	{
		if (ioptr[11] == 1) // if timer is enabled
		{
			ioptr[12] = ioptr[12] + (immdeteced + 1); //increment timer with clock cycles ti took from last instruction
		}
	}

	if (op == 0) //we want to check for interruption and update hardware as instructed
	{
		if (ioptr[12] == ioptr[13]) //if timercurrent= timermax
		{
			ioptr[12] = 0; //reset timer
			if (ioptr[0] == 1) ioptr[3] = 1; //if irq0enable is enabled, turn on irq0status
		}
		if (ioptr[12] > ioptr[13]) {//if timercurrent> timermax (last instruction took 2 clockcycles so we overpassed timermax
			ioptr[12] = 1;//reset timer to 1
			if (ioptr[0] == 1) ioptr[3] = 1; //if irq0enable is enabled, turn on irq0status
		}

	}
}

//input arguments:
//int* iogptr -  io register array
//int* irq_ptr - pointer to interrupt request pointer
//int immdetected - immmdeteced is 1 if last instruction used $imm. else it is 0
//int op- op=0 tells us to check if interruption should occur. op=1 will increment timer
//int* current_irq1 - pointer to variable which determines how many clock cycles passed since last write/read command to disk
// memory* head_dmemin, head_disk - linked-lists containing all current information of disk and main memory
// by the time cycles it took the last instruction to be executed

//the function is responsible to move data between memory to disk and vice-versa. if 1024 cycles passed since last
// write/read command to disk we check if interruption is bound to occur and update irq_ptr


memory* check_irq1(int *ioptr, int* irq_ptr, memory* head_dmemin, memory* head_disk, int immdetected, int* current_irq1, int op) {
	memory* ptr = NULL;
	int i;
	int address = 0;
	if (op == 0) { //we want to check for interruption and update hardware as instructed
		if (*current_irq1 >= 1024) //if 1024 cycles passed since last write/read command
		{
			ioptr[17] = 0; // turn off diskstatus
			ioptr[14] = 0; // turn off diskcmd
			*current_irq1 = 0; //reset disk timer
			if (ioptr[1] == 1) //if irq1enable is on, turn on irq1status
			{
				ioptr[4] = 1;
			}
		}
	}

	if (op == 1) { //we only want to deal with the hardware
		if (ioptr[17] == 0) { //if disk is free
			*current_irq1 = (immdetected + 1); //initialize disk timer to time cycles passed since last ins
			address = ioptr[15] * 128; //update the sector we want to read/write to in disk
			if ((ioptr[14] == 1)) {//write to memory from disk
				ioptr[17] = 1; //diskstatus is on
				for (i = 0; i < 128; i++) {
					if (head_disk != NULL) ptr = search(head_disk, address + i); // search for data in disk in that address
					if (ptr == NULL) { //node with that address was not put in disk linked-list meaning the data in that address is 0
						//insert node with wanted address and data=0 to main memory list
						head_dmemin = insert(head_dmemin, ioptr[16] + i, 0);
					}
					else {
						//insert a node to main memory list containing the data extracted from disk linked list
						head_dmemin = insert(head_dmemin, ioptr[16] + i, ptr->data);
					}
				}
			}

			if (ioptr[14] == 2) {//write to disk from memory
				ioptr[17] = 1; //diskstatus is on
				for (i = 0; i < 128; i++) {
					if (head_dmemin != NULL) ptr = search(head_dmemin, ioptr[16] + i); // search for data in main-memory in that address
					if (ptr == NULL) {//node with that address was not put in main-memory linked -> data in that address is 0
						//insert node with wanted address and data=0 to disk list
						head_disk = insert(head_disk, ioptr[15] * 128 + i, 0);
					}
					else {
						//insert a node to disk memory list containing the data extracted from main-memory linked list
						head_disk = insert(head_disk, (ioptr[15] * 128) + i, ptr->data);
					}
				}

			}

		}
		else {
			//the disk is busy
			*current_irq1 += (immdetected + 1); // increment disk timer
		}
	}

	return head_dmemin; //we always return pointer to updated main-memory linked-list
}
//input arguments:
//int* iogptr -  io register array
//int* irq_ptr, clcptr- pointer to interrupt request pointer and current clock cycle
//FILE* irq2in - pointer to irq2in file
//char current_irq2[MAX_COMMAND_LENGTH] - String containing the time-cycle in the row that is currently pointed at by irq2in file


//the function is responsible to make an interruption if the current time cycle of our simulator equals
//to the cycle in irq2in and if other conditions are met as specified in our instruction document
void check_irq2(int* clcptr, char current_irq2[MAX_COMMAND_LENGTH], FILE* irq2in, int *ioptr, int* irq_ptr, int op) {
	int x;
	if (op == 0) {
		x = (int)strtol(current_irq2, NULL, 0); //convert time cycle in irq2in file to decimal
		//if clcptr equals to x
		if ((*clcptr - 2 == x) || (*clcptr - 1 == x)) {
			fgets(current_irq2, MAX_COMMAND_LENGTH, irq2in); //move to next line in irq2in file
			current_irq2 = strtok(current_irq2, "\n"); //extract row information from irq2in and make sure it recieved properly
			if (current_irq2[0] == '\n') {
				fgets(current_irq2, MAX_COMMAND_LENGTH, irq2in);
			}
			if (ioptr[2] == 1) {//if irq2enabled is enabled
				ioptr[5] = 1; // turn on irq2status

			}
		}
	}
}
//input arguments:
//FILE* input -  pointer to imemin file
//int* irq_ptr, pcptr- pointer to interrupt request pointer and pc
//int* ioptr - io registers array


//the function checks whether an interruption should occur. it also checks whether we are currently dealing with
// an interruption. if no, than we call lineplacement function to move to another place in our program.
// the function returns 1 if we move to another place in our program. else, it returns 0

int itqhandler(FILE* input, int* pcptr, int *ioptr, int *irq_ptr) {
	//only if there is an interruption with both enable and status registers are on
	if (((ioptr[0] == 1) && (ioptr[3] == 1)) || ((ioptr[1] == 1) && (ioptr[4] == 1)) || ((ioptr[2] == 1) && (ioptr[5] == 1))) {
		if ((*irq_ptr == -1)) {
			*irq_ptr = 1;
			ioptr[7] = *pcptr; //put PC of interrupt return address
			*pcptr = ioptr[6]; //PC of interrupt handler
			lineplacement(input, ioptr[6]); //move to address in irqhandler
			return 1;
		}
		else {
			//we are currently dealing with an interruption so do nothing
			return 0;
		}
	}
	return 0;
}

/*PRINT FUNCTIONS*/
//the following functions print information to output files as specified

void printMonitor(int screen[288][352], FILE* monitor, FILE* yuv) {

	int i = 0;
	int j = 0;
	unsigned char temp;
	unsigned char *ptr;
	for (i = 0; i < 288; i++) {
		for (j = 0; j < 352; j++) {
			fprintf(monitor, "%02X\n", screen[i][j]);
			temp = screen[i][j];
			ptr = &temp;
			fwrite(ptr, 1, 1, yuv);
		}
	}
}

void printCycles(FILE* cycles, int* icountptr, int *clcptr) {
	int x;
	int y;
	x = *clcptr;
	y = *icountptr;
	fprintf(cycles, "%d\n", x);
	fprintf(cycles, "%d", y);
}

void hwregout_p(int line_num, int op1, int op2, int* clcptr, int* ioptr, FILE* hwregtrace) {
	fprintf(hwregtrace, "%d ", *clcptr - op2);
	if (op1 == 0) fprintf(hwregtrace, "READ ");
	if (op1 == 1) fprintf(hwregtrace, "WRITE ");

	if (line_num == 0) fprintf(hwregtrace, "irq0enable ");
	if (line_num == 1)fprintf(hwregtrace, "irq1enable ");
	if (line_num == 2)fprintf(hwregtrace, "irq2enable ");
	if (line_num == 3)fprintf(hwregtrace, "irq0status ");
	if (line_num == 4)fprintf(hwregtrace, "irq1status ");
	if (line_num == 5)fprintf(hwregtrace, "irq2status ");
	if (line_num == 6)fprintf(hwregtrace, "irqhandler ");
	if (line_num == 7)fprintf(hwregtrace, "irqreturn ");
	if (line_num == 8)fprintf(hwregtrace, "clks ");
	if (line_num == 9)fprintf(hwregtrace, "leds ");
	if (line_num == 10)fprintf(hwregtrace, "reserved ");
	if (line_num == 11)fprintf(hwregtrace, "timerenable ");
	if (line_num == 12)fprintf(hwregtrace, "timercurrent ");
	if (line_num == 13)fprintf(hwregtrace, "timermax");
	if (line_num == 14)fprintf(hwregtrace, "diskcmd ");
	if (line_num == 15)fprintf(hwregtrace, "disksector ");
	if (line_num == 16)fprintf(hwregtrace, "diskbuffer ");
	if (line_num == 17)fprintf(hwregtrace, "diskstatus ");
	if (line_num == 18)fprintf(hwregtrace, "monitorcmd ");
	if (line_num == 19)fprintf(hwregtrace, "monitorx ");
	if (line_num == 20)fprintf(hwregtrace, "monitory ");
	if (line_num == 21)fprintf(hwregtrace, "monitordata ");

	fprintf(hwregtrace, "%08X\n", ioptr[line_num]);

}


void regoutprint(int *regptr, FILE* output)
{
	int i = 0, temp = 0;
	for (i = 2; i < 16; i++)
	{
		temp = regptr[i];

		fprintf(output, "%08X\n", temp & 0xfffff);
	}

}
//input arguments:
//1. int* pc - pointer to current pc
//2. int* regptr - pointer to int array at size of 16 where the value stored in the i'th index
// is the value stored in the register encoded to i (in decimal)
//char line[MAX_COMMAND_LENGTH] - encoded instruction as recieved from imemin
//FILE* output - pointer to trace file

//the function prints information to the trace file
void traceprint(int *pc, int *regptr, char line[MAX_COMMAND_LENGTH], FILE* output)
{
	int i = 0, temp = 0;
	char op[3];
	op[0] = line[0];
	op[1] = line[1];
	op[2] = '\0';
	//if the opcode in decimal is bigger than 21, that it is undefined and we treat it like
	// add operation to register $zero
	if ((int)strtol(op, NULL, 0) > 21) {
		line[0] = 0;
		line[1] = 0;
	}


	fprintf(output, "%03X", *pc & 0xfffff);
	fprintf(output, " ");
	fprintf(output, "%s", line);
	for (i = 0; i < 16; i++)
	{
		fprintf(output, " ");
		temp = regptr[i];
		fprintf(output, "%08X", temp & 0xffffffff);
	}
	fprintf(output, "\n");
	return;
}





/*MAIN FUMCTIONS*/

//input arguments:
//int* pc - pointer to pc
//int* regptr, ioptr, irq_ptr, clcptr - pointer to: registers, io registers, current clock cycle, interrupt request variable
//char line[MAX_COMMAND_LENGTH] -  encoded instruction as recieved from imemin
//memory* head - pointer to main-memory linked-list constructed with dmemin file
//FILE* output, hwregtrace, leds - pointer to imemin, hwregtrace and leds  file
//int screen[288][352] - 2-dimentional array contatining the information for each pixel in monitor

//the function extracts opcode and registers from line argument and calls for the proper function dependent on the encoded
// opcode. if the opcode is halt we return 1. else, we return 0
// the function also deals with several additional situations as described below

int execute(int *pc, int *regptr, int *ioptr, char line[MAX_COMMAND_LENGTH], FILE* input, FILE* hwregtrace, FILE* leds, memory* head, int *irq_ptr, int* clcptr, int screen[288][352])
{

	int arguments[3];

	int i = 0, j = 0;
	char op[3];
	char temp[2];
	char* ptr;
	temp[1] = '\0';
	//extract opcode
	op[0] = line[0];
	op[1] = line[1];
	op[2] = '\0';
	//extract registers
	for (i = 2; i < 5; i++)
	{
		temp[0] = line[i];
		ptr = temp;
		arguments[j] = (int)strtol(ptr, NULL, 16); //extract opcode
		j++;
	}
	//search for the encoded opcode
	if (strcmp(op, "00") == 0) {
		add(regptr, arguments[0], arguments[1], arguments[2]);
	}
	else if (strcmp(op, "01") == 0) {
		sub(regptr, arguments[0], arguments[1], arguments[2]);
	}
	else if (strcmp(op, "02") == 0) {
		and (regptr, arguments[0], arguments[1], arguments[2]);
	}
	else if (strcmp(op, "03") == 0) {
		or (regptr, arguments[0], arguments[1], arguments[2]);
	}
	else if (strcmp(op, "04") == 0) {
		xor (regptr, arguments[0], arguments[1], arguments[2]);
	}
	else if (strcmp(op, "05") == 0) {
		mul(regptr, arguments[0], arguments[1], arguments[2]);
	}
	else if (strcmp(op, "06") == 0) {
		sll(regptr, arguments[0], arguments[1], arguments[2]);
	}
	else if (strcmp(op, "07") == 0) { sra(regptr, arguments[0], arguments[1], arguments[2]); }
	else if (strcmp(op, "08") == 0) { srl(regptr, arguments[0], arguments[1], arguments[2]); }
	else if (strcmp(op, "09") == 0) { branch(regptr, arguments[0], arguments[1], arguments[2], pc, input, 1); }
	else if (strcmp(op, "0A") == 0 || strcmp(op, "0a") == 0) { branch(regptr, arguments[0], arguments[1], arguments[2], pc, input, 2); }
	else if (strcmp(op, "0B") == 0 || strcmp(op, "0b") == 0) { branch(regptr, arguments[0], arguments[1], arguments[2], pc, input, 3); }
	else if (strcmp(op, "0C") == 0 || strcmp(op, "0c") == 0) { branch(regptr, arguments[0], arguments[1], arguments[2], pc, input, 4); }
	else if (strcmp(op, "0D") == 0 || strcmp(op, "0d") == 0) { branch(regptr, arguments[0], arguments[1], arguments[2], pc, input, 5); }
	else if (strcmp(op, "0E") == 0 || strcmp(op, "0e") == 0) { branch(regptr, arguments[0], arguments[1], arguments[2], pc, input, 6); }
	else if (strcmp(op, "0F") == 0 || strcmp(op, "0f") == 0) { jal(regptr, arguments[0], arguments[1], arguments[2], pc, input); }
	else if (strcmp(op, "10") == 0) { lw(regptr, arguments[0], arguments[1], arguments[2], head); }
	else if (strcmp(op, "11") == 0) { sw(regptr, arguments[0], arguments[1], arguments[2], head); }
	else if (strcmp(op, "12") == 0) {
		//if opcode is reti
		lineplacement(input, ioptr[7]); //move to the address stored in irqreturn register
		*pc = ioptr[7]; //update pc
		*irq_ptr = -1; //*irq_ptr =-1 means are not currently not dealing with an interruption
	}
	else if (strcmp(op, "13") == 0) { in(regptr, ioptr, arguments[0], arguments[1], arguments[2], clcptr, hwregtrace); }
	else if (strcmp(op, "14") == 0) {
		out(regptr, ioptr, arguments[0], arguments[1], arguments[2], clcptr, hwregtrace, leds, screen);
	}
	else if (strcmp(op, "15") == 0) { return 1; } //if opcde is halt, return 1
	else { //if no opcode was found we add $rt and $rs and put the result in register $zero
		add(regptr, 0, arguments[1], arguments[2]);
	}
	return 0;
}


//input arguments:
//int* pc - pointer to pc
//int* regptr, ioptr, irq_ptr, clcptr, icountptr - pointer to: registers, io registers, current clock cycle, interrupt request variable and command count
//char line[MAX_COMMAND_LENGTH] -  encoded instruction as recieved from imemin
//memory* head_dmemin, head_disk - pointer to main-memory and disk linked-lists constructed with dmemin and diskin files
//FILE* trace, output, hwregtrace, leds, irq2in - pointer to  trace,imemin, hwregtrace, irq2in, and leds files
//int screen[288][352] - 2-dimentional array contatining the information for each pixel in monitor

//the function iterates over imemin, exceuting the instructions, and simulating MIPS
// each iteration a call to traceprint is made where the function prints what is needed
// in each iteration the function checks whether an in interruption should occur and act accordingly, and also
// deals with the timer, disk and irq2in file and their associated global variables
//the function returns pointer to main-memory linked list

memory* datarun(int *pcptr, int *regptr, int *ioptr, FILE* input, FILE* trace, memory* head_dmemin, memory* head_disk, FILE* hwregtrace, FILE* leds, int *clcptr, int *icountptr, FILE* irq2in, int *irq_ptr, int screen[288][352])
{
	int handler_flag = 0;
	char current_line[MAX_COMMAND_LENGTH];
	char current_irq2[MAX_COMMAND_LENGTH];
	int immdetected = 0, i = 2, haltdetected;
	char imm[MAX_COMMAND_LENGTH];
	int offset = 0;
	int* current_irq1 = &offset;
	int negative = 0x80000, test = 0;
	//memory* temp;
	fgets(current_irq2, MAX_COMMAND_LENGTH, irq2in);


	while (fgets(current_line, MAX_COMMAND_LENGTH, input) != 0)
	{
		//check if interruption should occur before executing the instruction
		check_irq0(ioptr, irq_ptr, immdetected, 0);
		head_dmemin = check_irq1(ioptr, irq_ptr, head_dmemin, head_disk, immdetected, current_irq1, 0);
		check_irq2(clcptr, current_irq2, irq2in, ioptr, irq_ptr, 0);
		handler_flag = itqhandler(input, pcptr, ioptr, irq_ptr);
		if (handler_flag == 1) {
			//if interruption occurred- current_line will hold the string where the file is pointing at
			fgets(current_line, MAX_COMMAND_LENGTH, input);
		}


		if (current_line[0] == '\n') //check whether fgets resulted in blank line
		{
			if (fgets(current_line, MAX_COMMAND_LENGTH, input) == 0)
			{
				continue;
			}
		}

		immdetected = 0;
		for (i = 2; i < 5; i++) //if one of the registers is 1 meaning its $imm
		{
			if (current_line[i] == '1')
			{
				immdetected = 1; //update flag as 1
				fgets(imm, MAX_COMMAND_LENGTH, input);
				//we advance to the next line because it has the value of immediate
				if (imm[0] == '\n') //checks if we after fgets we got a blank linke
				{
					fgets(imm, MAX_COMMAND_LENGTH, input);
				}
				//the following lines are meant to deal with negative numbers in hex
				test = negative & (int)strtol(imm, NULL, 16);
				if (test != 0)
				{
					test = 0x7ffff & (int)strtol(imm, NULL, 16);
					//extract all bits from instruction except the last one
					regptr[1] = -(negative)+test; //add test with the minimal negative number identical to 2's complement
					break;
				}
				regptr[1] = (int)strtol(imm, NULL, 16); //update register $imm data
				break; //exit inner loop
			}
		}
		traceprint(pcptr, regptr, current_line, trace); //print pc, instruction and register's status to trace file

		if (immdetected == 1) //if instruction uses $imm increment clock-cycle by 2. else, increment by 1
		{
			*pcptr = *pcptr + 2;
			*clcptr = *clcptr + 2;
		}
		else
		{
			*pcptr = *pcptr + 1;
			*clcptr = *clcptr + 1;
		}
		//update timer and disk hardware and associated global variables
		check_irq0(ioptr, irq_ptr, immdetected, 1);
		head_dmemin = check_irq1(ioptr, irq_ptr, head_dmemin, head_disk, immdetected, current_irq1, 1);
		//execute current instruction and recieve 1 if opcode is halt. else we recieve 0.
		haltdetected = execute(pcptr, regptr, ioptr, current_line, input, hwregtrace, leds, head_dmemin, irq_ptr, clcptr, screen);
		*icountptr = *icountptr + 1; //in crement command counter by 1

		if (haltdetected == 1) { //if opcode is halt, exit
			return head_dmemin;
		}
	}
	return head_dmemin; //return main-memory linked-list
}
//the function will initialize several pointers to variables which will be passed onwards to other functions
// so that these variables will be used as global variables and could be changed throughout the entire run
int main(int argc, char* argv[])
{
	int i = 0, j = 0;
	//pointer to all Files
	FILE* imemin;
	FILE* dmemin;
	FILE* diskin;
	FILE* irq2in;
	FILE* dmemout;
	FILE* regout;
	FILE* trace;
	FILE* hwregtrace;
	FILE* cycles;
	FILE* leds;
	FILE* monitor;
	FILE* diskout;
	FILE* yuv;
	int pc = 0;
	int irq = -1;
	memory* memdmemin;
	memory* memdiskin;
	int screen[288][352];
	int total_clock = 0, total_i = 0;  //counting the number of clocks and instructions
	int reg_arr[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	//pointer to int array at size of 16 where the value stored in the i'th index
	// is the value stored in the register encoded to i (in decimal)
	int io_reg[22] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	//pointer to int array at size of 22 where the value stored in the i'th index
	// is the value stored in the "hardware register" (output/input register) encoded to i (in decimal)
	int *pcptr, *regptr, *ioptr, *clcptr, *icountptr, *irq_ptr;
	//pointer to variables so that they can be used as global in the program.
	// we have a pointer to our: registers, io register, pc, time cycle, command count, interruption
	irq_ptr = &irq;
	pcptr = &pc;
	icountptr = &total_i;
	clcptr = &total_clock;
	irq_ptr = &irq;
	regptr = reg_arr;
	ioptr = io_reg;

	//Initializing monitor data array containging data for each pixel in pur monitor
	for (i = 0; i < 288; i++) {
		for (j = 0; j < 352; j++) {
			screen[i][j] = 0;
		}
	}
	/*open all files*/
	imemin = fopen(argv[1], "r");
	if (imemin == NULL) exit(1);
	dmemin = fopen(argv[2], "r");
	if (dmemin == NULL) exit(1);
	diskin = fopen(argv[3], "r");
	if (diskin == NULL) exit(1);
	irq2in = fopen(argv[4], "r");
	if (irq2in == NULL) exit(1);
	dmemout = fopen(argv[5], "w");
	if (dmemout == NULL) exit(1);
	regout = fopen(argv[6], "w");
	if (regout == NULL) exit(1);
	trace = fopen(argv[7], "w");
	if (trace == NULL) exit(1);
	hwregtrace = fopen(argv[8], "w");
	if (hwregtrace == NULL) exit(1);
	cycles = fopen(argv[9], "w");
	if (cycles == NULL) exit(1);
	leds = fopen(argv[10], "w");
	if (leds == NULL) exit(1);
	monitor = fopen(argv[11], "w");
	if (monitor == NULL) exit(1);
	yuv = fopen(argv[12], "wb");
	if (yuv == NULL) exit(1);
	diskout = fopen(argv[13], "w");
	if (diskout == NULL) exit(1);

	memdmemin = loadmemory(dmemin);
	memdiskin = loadmemory(diskin);

	memdmemin = datarun(pcptr, regptr, ioptr, imemin, trace, memdmemin, memdiskin, hwregtrace, leds, clcptr, icountptr, irq2in, irq_ptr, screen);

	//print data to output files
	printMonitor(screen, monitor, yuv);
	printMemory(dmemout, memdmemin);
	printMemory(diskout, memdiskin);
	printCycles(cycles, icountptr, clcptr);
	regoutprint(regptr, regout);

	fclose(dmemin);
	fclose(imemin);
	fclose(dmemin);
	fclose(diskin);
	fclose(irq2in);
	fclose(dmemout);
	fclose(regout);
	fclose(trace);
	fclose(hwregtrace);
	fclose(cycles);
	fclose(leds);
	fclose(monitor);
	fclose(yuv);
	fclose(diskout);


	free_Array(memdmemin); /*free labels' linked list*/
	free_Array(memdiskin); /* free memory's linked list*/




	return 0;
}

