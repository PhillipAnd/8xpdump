#include <stdio.h>

#include "tokens.h"

#define PROGRAM_NAME_OFFSET 0x3C
#define PROGRAM_SIZE_OFFSET 0x39
#define PROGRAM_DATA_OFFSET 0x4A
#define FILE_SIGNATURE_LENGTH 2

int littleEndianToInt(char *bytes);

char *interpret_token(unsigned char curToken);
long fsize(char *file);

int main(int argc, char **argv) {
	FILE *file;
	char progName[9];
	char progSizeBytes[2];
	long progSize;
	unsigned char curToken;
	if (argc < 2) {
		printf("Usage: %s [filename.8xp]\n",argv[0]);
		return -1;
	}
	
	progSize = fsize(argv[1]);
	progSize = progSize - PROGRAM_DATA_OFFSET - FILE_SIGNATURE_LENGTH; /* The size of the program itself, and not the file */
	
	file = fopen(argv[1],"r");
	if (!file) {
		printf("File cannot be read. Sorry.\n");
		return -1;
	}
	
	fseek(file,PROGRAM_NAME_OFFSET,SEEK_SET);
	fread(progName,1,8,file);
	progName[8] = 0;
	printf("Program Name: %s\n",progName);
	
	printf("Size: %ld bytes\n",progSize);
	
	fseek(file,PROGRAM_DATA_OFFSET,SEEK_SET);
	while (progSize > 0) {
		fread(&curToken,1,1,file);
		printf("%s",interpret_token(curToken));
		progSize--;
	}
	
	fclose(file);
	return 0;
}

int littleEndianToInt(char *bytes) {
		return bytes[1]*256 + bytes[0];
}

char *interpret_token(unsigned char curByte) {
	unsigned static char previousByte = 0;
	char *result;
	
	switch (previousByte) {
	case 0x7E:
		if (curByte < 0x20) {
			result = twobyte_tokens_7E[curByte];
		} else {
			result = "??";
		}
		previousByte = 0;
		break;
	case 0xBB:
		if (curByte <= 0xCE) {
			result = twobyte_tokens_BB[curByte];
		} else {
			result = "??";
		}
		previousByte = 0;
		break;
	default:
		result = single_byte_tokens[curByte];
		previousByte = curByte;
	}
	
	return result;
}

long fsize(char* file) /* Quick and very dirty way to find out the size of a file < 2GB */
{
	long len;
    FILE * f = fopen(file, "r");
	if (!f)
		return -1;
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fclose(f);
    return len;
}