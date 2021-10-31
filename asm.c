#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

#define MAX_COMMAND_LENGTH 500

/*DATA STRUCTURE*/
/*(1)Linked-List used to store Labels addresses in first iteration and memory addresses in
 *second iteration
*/

typedef struct Label
{
	char name[50];
	int address;
	struct Label* next;
} label;

/*(2) argument Holder data structure to store command line's arguments - op, rd, rs, rt, imm/label */
typedef struct argumentHolder
{
	char opcode[50];
	char a[50];
	char b[50];
	char c[50];
	char d[50];
} argumentHolder;

/*DATA STRUCTURE - LINKED-LIST - METHODS*/

/*(1) creating a label\ memory node for linked-list*/
label* create_node(char name[50], int address) {
	label* new_label = (label*)malloc(sizeof(label));
	if (new_label != NULL) {
		strcpy(new_label->name, name);
		new_label->address = address;
		new_label->next = NULL;
	}
	return new_label;
}
/* (2) insert node to linked-list sorted by address
 * the ascending order will help us print data to memory addresses by order*/
label* insert(label* head, char name[50], int address) {
	label* iter, *prev = NULL;
	label* new_label = create_node(name, address); /* create new node */
	if (new_label == NULL) return NULL; /* check for failure */
	if (head == NULL) return new_label;

	if (new_label->address < head->address)
	{ /* check if label's address is minimal in linked-list*/
		new_label->next = head;
		return new_label;
	}
	iter = head;
	while ((iter != NULL) && (new_label->address >= iter->address)) {
		/*iterate the list until node-> address is smaller than node->next->address*/
		prev = iter;
		iter = iter->next;
	}
	if ((iter == NULL) || (iter->address > new_label->address)) {
		prev->next = new_label;
		new_label->next = iter;
	}
	else {
		/*if address already exits - override it*/
		strcpy(iter->name, new_label->name);
		free(new_label);

	}

	return head;
}
/* (3) search for label by name in a linked-list*/
/* used for second iteration to find label's address*/
int search(label* head, char name[50]) {
	char current[50];
	label* ptr = head;
	strcpy(current, head->name);
	while (strcmp(current, name) != 0) {
		ptr = ptr->next;
		if (ptr == NULL) {
			return -1;
		}
		strcpy(current, ptr->name);
	}
	return ptr->address;
}
/*free memory of linked-list*/
void free_Array(label* head)
{
	label* curr_node;
	while (head != NULL)
	{
		curr_node = head;
		head = head->next;
		free(curr_node);
	}
}

/*HELPER FUNCTIONS*/

/*(1) given op string, encode it to its hexadecimal value and print to imemin*/
void encode_op(char op[50], FILE* output) {
	if (strcmp(op, "add") == 0) fprintf(output, "00");
	if (strcmp(op, "sub") == 0) fprintf(output, "01");
	if (strcmp(op, "and") == 0) fprintf(output, "02");
	if (strcmp(op, "or") == 0) fprintf(output, "03");
	if (strcmp(op, "xor") == 0) fprintf(output, "04");
	if (strcmp(op, "mul") == 0) fprintf(output, "05");
	if (strcmp(op, "sll") == 0) fprintf(output, "06");
	if (strcmp(op, "sra") == 0) fprintf(output, "07");
	if (strcmp(op, "srl") == 0) fprintf(output, "08");
	if (strcmp(op, "beq") == 0) fprintf(output, "09");
	if (strcmp(op, "bne") == 0) fprintf(output, "0A");
	if (strcmp(op, "blt") == 0) fprintf(output, "0B");
	if (strcmp(op, "bgt") == 0) fprintf(output, "0C");
	if (strcmp(op, "ble") == 0) fprintf(output, "0D");
	if (strcmp(op, "bge") == 0) fprintf(output, "0E");
	if (strcmp(op, "jal") == 0) fprintf(output, "0F");
	if (strcmp(op, "lw") == 0) fprintf(output, "10");
	if (strcmp(op, "sw") == 0) fprintf(output, "11");
	if (strcmp(op, "reti") == 0) fprintf(output, "12");
	if (strcmp(op, "in") == 0) fprintf(output, "13");
	if (strcmp(op, "out") == 0) fprintf(output, "14");
	if (strcmp(op, "halt") == 0) fprintf(output, "15");


}
/*(2) given register string, encode it to its hexadecimal value and print it to imemin*/
void encode_reg(char reg[50], FILE* output) {
	if (strcmp(reg, "$zero") == 0) fprintf(output, "0");
	if (strcmp(reg, "$imm") == 0) fprintf(output, "1");
	if (strcmp(reg, "$v0") == 0) fprintf(output, "2");
	if (strcmp(reg, "$a0") == 0) fprintf(output, "3");
	if (strcmp(reg, "$a1") == 0) fprintf(output, "4");
	if (strcmp(reg, "$t0") == 0) fprintf(output, "5");
	if (strcmp(reg, "$t1") == 0) fprintf(output, "6");
	if (strcmp(reg, "$t2") == 0) fprintf(output, "7");
	if (strcmp(reg, "$t3") == 0) fprintf(output, "8");
	if (strcmp(reg, "$s0") == 0) fprintf(output, "9");
	if (strcmp(reg, "$s1") == 0) fprintf(output, "A");
	if (strcmp(reg, "$s2") == 0) fprintf(output, "B");
	if (strcmp(reg, "$gp") == 0) fprintf(output, "C");
	if (strcmp(reg, "$sp") == 0) fprintf(output, "D");
	if (strcmp(reg, "$fp") == 0) fprintf(output, "E");
	if (strcmp(reg, "$ra") == 0) fprintf(output, "F");
}
/*(3)if file contains label and instructions instruction -  extract the instruction and return it*/

void changeCurrentLine(char current_line[MAX_COMMAND_LENGTH], int x) {
	int i = 0;
	char letter = ' ';

	while ((letter != '\n') && (letter != '#')) {
		letter = current_line[x];
		current_line[i] = letter;
		x++;
		i++;
	}
	current_line[i] = '\0';
	return;

}
/*(4)given a string determines its type and/or  if substring occurred in string outside of a note
 * option = 1 : to check if line is blank
 * option = 2: to check if line is a Label
 * option = 3: to check if a command line uses the $imm register
 * option = 4: to check if command is of type .word
 * option =5: check if instruciton is in the same row as label (called only when the line has label in it)
*/

int lineinfo(char line[MAX_COMMAND_LENGTH], int option)
{

	if (option == 1) /*check for blank line*/
		/* iterates a line and check if it is comprised only of the following chars: '\n', '\t' and ' ' */
	{
		int i = 0;
		char current_char = line[i];
		while ((current_char == '\t') || (current_char == ' ')) {
			i++;
			current_char = line[i];
		}
		if ((current_char == '\n') || (current_char == '#'))  return 1;
		return 0;
	}
	/*the following explanation applies to all 3 remaining options:
	 * check if substring x occur in string s. if so, checks if x occur before the first
	 * occurrence of the character '#'.
	 */

	if (option == 2) /*check for labels*/
	{
		if (strstr(line, ":") != NULL)
		{
			if (strstr(line, "#") != NULL) {
				if ((strstr(line, ":")) > (strstr(line, "#"))) {
					return 0;
				}
			}
			return 1;
		}
		return 0;
	}
	if (option == 3) /* check for $IMM*/
	{
		if (strstr(line, "$imm") != NULL)
		{
			if (strstr(line, "#") != NULL) {
				if ((strstr(line, "$imm")) > (strstr(line, "#"))) {
					return 0;
				}
			}
			return 1;
		}
		return 0;
	}
	if (option == 4) /*check for .word operation*/
	{
		if (strstr(line, ".word") != NULL)
		{
			if (strstr(line, "#") != NULL) {
				if ((strstr(line, ".word")) > (strstr(line, "#"))) {
					return 0;
				}
			}
			return 1;
		}
		return 0;
	}
	if (option == 5) {/*checks for instruction in same line as label*/

		int i = 0;
		char current_char = line[i];
		while (current_char != ':') {
			i++;
			current_char = line[i];
		}
		i++;
		current_char = line[i];
		while ((current_char == '\t') || (current_char == ' ')) {
			i++;
			current_char = line[i];
		}
		if ((current_char == '\n') || (current_char == '#')) return 0;
		return i;
	}

	return 0;
}

/*(5) given the immediate argument of a command-line - determines whether its is a constant number
 * or a label*/
int isLabel(char label[50]) {
	if ((isalpha(label[0]) == 1) || (isalpha(label[0]) == 2)) {
		return 1;
	}
	return 0;
}

/*(6) given an argumentHolder consisting of all the arguments of a command-line and boolean variable
 * to determine whether one of the registers is $imm -
 *  prints encoded arguments to imemin as instructed
 */
void printline(argumentHolder* arguments, FILE *output, int immFlag, label* labels)
{
	int x;
	/*print opcode, rd, rs, rt */
	encode_op(arguments->opcode, output);
	encode_reg(arguments->a, output);
	encode_reg(arguments->b, output);
	encode_reg(arguments->c, output);

	if (immFlag == 1) {
		/*if immFlag equals 1, one of the registers is $imm meaning we need to print
		 * the imm argument (constant \ Label) in a new line
		 */
		fprintf(output, "\n");
		if (isLabel(arguments->d) == 1)
			/*check to see if imm argument is Label or constant and print the result in hexadecimal
			 * if it is a Label it will search for labels' name in our Label linked-list and
			 * upon match with a node, prints node-> address to imemin*/
		{
			fprintf(output, "%05X", search(labels, arguments->d));
		}
		else {
			/* if imm is constant simply print constant in hex to imemin */
			x = (int)strtol(arguments->d, NULL, 0);
			fprintf(output, "%05X", x & 0xfffff);
		}
	}
	fprintf(output, "\n");
}

/*(7) given memory linked-list (sorted by address) which was constructed during second iteration,
 * we loop from address = 0 to address = 4096. if address equals to an address of node in our linked-list,
 * we print the node's data in hexadecimal. else, we print zeros
 *
 */
void printMemory(FILE* dmemin, label* memory)
{
	long data;
	long negative = 0x80000000, max = 0x7fffffff, test = 0;
	char nstring[9];
	char *tempptr;
	char tempchar[2];
	label* ptr;
	int x, i, j;
	int address;
	tempchar[1] = '\0';
	address = 0;
	ptr = memory;
	/*our program faces a problem when a number has its last bit turned on,
	 * meaning last char is bigger than 8 (in hex base).
	 * The following conditions and loop are called to solve problem and print memory correctly*/

	while ((ptr != NULL) && (address != 4096)) {
		if (address == ptr->address) {
			j = (int)strtol(ptr->name, NULL, 16);
			tempchar[0] = (ptr->name)[2]; /*sampling last character (hex) of data*/
			test = (int)strtol(tempchar, NULL, 16); /*converting hex char to decimal*/
			if ((test >= 8) && (max == j))
				/*going over all possibilities for last character, given that last bit is 1*/
			{
				test = test & 0x7; /*test will now hold all the bits of last character except for the signed-bit*/
				for (i = 1; i < 8; i++)
				{
					nstring[i] = (ptr->name[i + 1]);

				}
				nstring[8] = '\0';
				/*nstring is now a string that contains the entire data*/
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
				data = (int)strtol(tempptr, NULL, 16) + negative;
				fprintf(dmemin, "%08X\n", data & 0xffffffff);
				address++;
				ptr = ptr->next;
				continue;
			} // last bit is not on
			x = (int)strtol(ptr->name, NULL, 0);
			fprintf(dmemin, "%08X\n", x & 0xffffffff);
			address++;
			ptr = ptr->next;
		}
		else { //node with data is not in linked-list so we print 0
			fprintf(dmemin, "00000000\n");
			address++;
		}
	}

}
/* (8) given .word command line and pointer to a memory linked-list:
 * a. parse .word command to create address and data in this line
 * b. create new node consisting of the address and data
 * c. insert the new node to linked-list
 *
 * The following method parses the command by looping over the command-line and using an integer
 * indication flag variable which at any point gives us information on which argument are we reading/
 * about to read
 */
label* fetchMemmory(char current_line[MAX_COMMAND_LENGTH], label* head)
{
	int word_flag = 0, i = 0, j = 0, temp = 0;
	char address[6];
	char data[11];
	int address2 = 0;
	label* new_head = NULL;

	while ((current_line[temp] != '\n') && (current_line[temp] != '#') && (current_line[temp] != '\0')) /*read only the useable text*/
	{
		if ((current_line[temp] == ' ') || (current_line[temp] == '\t')) { /*skip spaces and tabs and mark the end of arg data*/

			if (word_flag == 2) {
				/*word_flag equals 2 only when we started reading the address */
				word_flag = 3; /*the loop is designed in a way that word_flag equals 3 only if we strated reading data */
				continue;
			}
			temp++;
			continue;
		}
		if ((current_line[temp] == 'd') && (word_flag == 0)) /*mark the location of the address start*/
		{

			word_flag = 1; /*indicates we finished reading .word*/
			temp++;
			continue;
		}
		if ((word_flag == 1) || (word_flag == 2)) /*indicates we started reading address*/
		{
			address[i] = current_line[temp];

			i++;
			word_flag = 2; /*started to read usable characters from address*/
			temp++;
			continue;
		}
		if (word_flag == 3) /* we started read data */
		{
			data[j] = current_line[temp];
			j++;
			temp++;
			continue;
		}
		temp++;
	}
	data[j] = '\0';
	address[i] = '\0';
	address2 = (int)strtol(address, NULL, 0);
	/*address2 is converted to decimal in order to allow us to enter data in the correct line in dmemin*/
	new_head = insert(head, data, address2); /*updates memory linked-list by one node */

	return new_head;

}

/*(9) helper function which its only caller is "recoverData" function
 * the function extracts all arguments of a command-line (excluding .word command-line), except opcode
 * the extraction of an argument is based on the value of the parameter "depth":
 * depth = 2: "rd"
 * depth = 3: "rs"
 * depth = 4: "rt"
 * depth = 5: immediated
 */
char* fillArgument(char *current_line, int depth)
{
	/* if depth equals 3 or 4 the function operates in a recursive manner:
	 * we each time find first occurrence of '$', increment the pointer by 1, and repeat the proccess according
	 * to depth argument.
	 * the base case which serves for depth 2 by default (no condition was met for depth=2),
	 *iterates over given string and extract usable character.
	 * note: the size of the recursion stack is up to 3 at max
	 */
	char outchar[50];
	char* startPtr;
	int i = 0, temp = 0;
	startPtr = strstr(current_line, "$"); //first ocurrence of ('$')
	if (depth == 3) // extract $rs
	{
		startPtr++;
		return fillArgument(startPtr, 2);
	}
	if (depth == 4) //extract $rt
	{
		startPtr++;
		return fillArgument(startPtr, 3);
	}
	if (depth == 5) //extract immediate
	{
		startPtr = strstr(current_line, ",");
		startPtr++;
		startPtr = strstr(startPtr, ",");
		startPtr++;
		startPtr = strstr(startPtr, ",");
		startPtr++;

		i = 0; /*used as a flag*/
		while ((*startPtr != '\n') && (*startPtr != '#'))
		{
			if ((*startPtr != ' ') && (*startPtr != '\t')) //we reached usable char
			{
				i = 1;
				outchar[temp] = *startPtr;
				temp++;
				startPtr++;
				continue;
			}
			if (i == 1)
			{
				outchar[temp] = '\0';
				startPtr = outchar;
				return startPtr;
			}
			startPtr++;
		}
		outchar[temp] = '\0';
		startPtr = outchar;
		return startPtr;

	}
	while ((*startPtr != '\n') && (*startPtr != '#')) //base case which serves for depth =2
	{
		if ((*startPtr != ' ') && (*startPtr != '\t')) {
			if (*startPtr == ',')
			{
				outchar[temp] = '\0';
				startPtr = outchar;
				return startPtr;
			}
			outchar[temp] = *startPtr;
			temp++;
			startPtr++;
		}
	}
	return NULL;
}
/* (10)the following method allows us to correctly extract an argument from commandline (excluding .word command) according to the option number
 * which was given
 * option = 1: opcode
 * option = 2: rd
 * option = 3: rs
 * option = 4: rt
 * option = 5: imm
 */

char* recoverData(char current_line[MAX_COMMAND_LENGTH], int option)
{
	char argument[50];
	char *outptr;
	switch (option)
	{
	case 1: /*return first argument*/
		/* parses opcode while disregarding white spaces */
	{
		int tracker = 0, i = 0, temp = 0;
		char* cptr;
		while ((current_line[temp] != '\n') && (current_line[temp] != '#'))
		{
			if ((current_line[temp] != ' ') && (current_line[temp] != '\t'))
			{
				argument[i] = current_line[temp];
				i++;
				tracker = 1; /*start reading command*/
				temp++;
				continue; /*done with first argument*/
			}
			if (tracker == 1) /* this condition is answered only if we started reading opcode*/
			{
				argument[i] = '\0';
				cptr = argument;
				return cptr;
			}
			temp++;
		}
		argument[i] = '\0';

		cptr = argument;
		return cptr;
	}
	/*remaining arguments are extracted using the fillArgument command
	 * note: the first argument was not extracted using fillArgument due to the usage
	 * of occurrences of the character ',' which helped us parse the string more easily */
	case 2:
	{
		outptr = current_line;
		return fillArgument(outptr, 2);
	}
	case 3:
	{
		outptr = current_line;
		return fillArgument(outptr, 3);
	}
	case 4:
	{
		outptr = current_line;
		return fillArgument(outptr, 4);
	}
	case 5:
	{
		outptr = current_line;
		return fillArgument(outptr, 5);
	}
	default:
		break;
	}
	return NULL;
}

/*MAIN FUNCTIONS*/

label* secondIteration(FILE *asmFile, label* labels, FILE *imemin) {
	int x = 0;
	char current_line[MAX_COMMAND_LENGTH];
	label* memory = NULL;
	argumentHolder* line = NULL;
	int immDetected = 0;
	while (fgets(current_line, MAX_COMMAND_LENGTH, asmFile) != 0) {
		if ((lineinfo(current_line, 1) == 0)) /*if the line is not blank :*/
		{
			if (lineinfo(current_line, 2) == 1) /*if the line is Label :*/
			{
				x = lineinfo(current_line, 5); //if line is label and instruction in the same row
				if (x == 0) {
					continue;
				}
				else {
					changeCurrentLine(current_line, x);
				}
			}
			if (lineinfo(current_line, 4) == 1) /*if the line is a .word order*/
			{
				memory = fetchMemmory(current_line, memory);
				continue;
			}
			line = (argumentHolder*)malloc(sizeof(argumentHolder));
			if (line != NULL)
			{
				immDetected = lineinfo(current_line, 3);
				strcpy(line->opcode, recoverData(current_line, 1));
				strcpy(line->a, recoverData(current_line, 2));
				strcpy(line->b, recoverData(current_line, 3));
				strcpy(line->c, recoverData(current_line, 4));
				strcpy(line->d, recoverData(current_line, 5));
				printline(line, imemin, immDetected, labels);
				free(line);
				line = NULL;
			}

		}
	}

	return memory;

}
/*INPUT: assembly file
*OPERATION:
*1. iterates over the file's lines disregarding notes and whitespaces
*2. for each line, asserts it is a label
*3. the function can determine label's address (line position) according to the iterations made thus far*/
label* firstIteraion(FILE *asmFile) {
	char current_line[MAX_COMMAND_LENGTH];

	int count = 0, i = 0, j = 0, x = 0;
	label* head = NULL;
	while (fgets(current_line, MAX_COMMAND_LENGTH, asmFile) != 0) {
		if (lineinfo(current_line, 2) == 1) /* if it is a label we take it and we dont raise the count level*/
		{
			char name[51];
			i = 0;
			j = 0;
			while (current_line[i] != ':')
			{
				if ((current_line[i] == ' ') || (current_line[i] == '\t'))
				{
					i++;
				}
				else
				{
					name[j] = current_line[i];
					i++;
					j++;
				}
			}
			name[j] = '\0';
			head = insert(head, name, count);
			x = lineinfo(current_line, 5);
			if (x == 0) {
				continue;
			}
			else {
				changeCurrentLine(current_line, x);
			}
		}
		if (((lineinfo(current_line, 1) == 0)) && (lineinfo(current_line, 4) == 0)) /*if the line was not blank we check if it had IMM and raise the count as needed*/
		{
			if (lineinfo(current_line, 3) == 1) /*had imm so we raise by 2*/
			{
				count = count + 2;
			}
			else /*no imm found so we raise by 1*/
			{
				count++;
			}
		}
	}
	return head;
}


int main(int argc, char* argv[]) {

	label* label_linked_list;
	FILE* imemin;
	FILE* dmemin;
	label* memory;
	FILE* asmFile;
	asmFile = fopen(argv[1], "r"); /*open assembly file*/
	if (asmFile == NULL) {
		exit(1);
	}
	label_linked_list = firstIteraion(asmFile); /*create label list in iteration 1*/
	fclose(asmFile); /*close assembly file*/
	asmFile = fopen(argv[1], "r"); /*open assenbly file*/
	if (asmFile == NULL) {
		exit(1);
	}
	imemin = fopen(argv[2], "w"); /*open imemin file*/
	if (imemin == NULL) {
		exit(1);
	}
	/*printing instructions to imemin in iteration 2 and returning memory addresses linked-list*/
	memory = secondIteration(asmFile, label_linked_list, imemin);
	fclose(imemin); /*close imemin*/
	fclose(asmFile); /*close assembly file*/
	dmemin = fopen(argv[3], "w");
	if (dmemin == NULL) {
		exit(1);
	}
	printMemory(dmemin, memory); /* print memory state to dmemin using memory linked-list from iteration 2*/
	fclose(dmemin); /*close dmemin*/
	free_Array(label_linked_list); /*free labels' linked list*/
	free_Array(memory); /* free memory's linked list*/
	assert(argc > 0);

	return 0;
}
