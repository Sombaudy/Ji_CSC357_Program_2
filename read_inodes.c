#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "funcs.h"

#define MAX_LENGTH 100

// typedef struct
// {
// 	uint32_t inode;
// 	uint32_t parentInode;
// 	char type;
// 	char name[32];
// } Inode;

void *checked_malloc(int len)
{
	void *p = malloc(len);
	if (p == NULL)
	{
		printf("Ran out of memory!\n");
		exit(1);
	}
	return p;
}

char *uint32_to_str(uint32_t i)
{
	// pretend to print to a string to get length
	int length = snprintf(NULL, 0, "%lu", (unsigned long)i);

	// allocate space for the actual string
	char* str = (char *)checked_malloc(length + 1);

	// print to string
	snprintf(str, length + 1, "%lu", (unsigned long)i);

	return str;
}

void loadInode(uint32_t inode, char type, int index, Inode inodeList[])
{
	char *filename = uint32_to_str(inode);

	FILE *file;
	char character; // Variable to store the read character

	// Open the binary file in read mode
	file = fopen(filename, "rb"); // "rb" stands for read binary
	free(filename);

	// Check if the file was opened successfully
	if (file == NULL)
	{
		perror("Error opening file");
		exit(1);
	}

	// Read characters from the file and print them as characters
	int char_count = 1;

	int name_count = 0; //keeping track of how many add chars there are. NOTE: above 3, then give the go to add name
	int is_dir = 0; //add name if in directory
	int inode_naming = 0; //keeping track of which inode you are naming
	int name_index = 0; //index of name
	//uint32_t inode = 0;
	//char type = '\0';
	char prev = '\0';
	char curr = '\0';
	while (fread(&character, sizeof(char), 1, file) == 1)
	{
		if(char_count == 1) {
			prev = character;
			if(type == 'd')
			{
				printf("This is a directory! %c\n", character);
				is_dir = 1;
			}
			else
			{
				printf("Add char: %c\n", character);
				if(name_count >= 3 && is_dir == 1 && inode_naming != 0 && name_index < 32) {
					inodeList[inode_naming].name[name_index] = character;
					name_index++;
				}
				name_count = name_count + 1;
			}
		} else {
			curr = character;
			if(prev == 0 && curr != 0)
			{
				if(!isprint(curr) && curr != 46)
				{
					printf("INODE num: %d\n", character);
					name_index = 0;
					if (name_count >= 3) {
						//printf("change parent node of inode %d to %d\n", (int)character, index);
						inodeList[(int)character].parentInode = inodeList[index].inode;
						inode_naming = (int)character;
					}
				}
				else
				{
					printf("Add char: %c\n", character);
					if(name_count >= 3 && is_dir == 1 && inode_naming != 0 && name_index < 32) {
					inodeList[inode_naming].name[name_index] = character;
					name_index++;
					}
					name_count = name_count + 1;
				}
			}
			else if(prev != 0 && curr != 0)
			{
				if(isprint(curr))
				{
					printf("Add char: %c\n", character);
					if(name_count >= 3 && is_dir == 1 && inode_naming != 0 && name_index < 32) {
					inodeList[inode_naming].name[name_index] = character;
					name_index++;
					}
					name_count = name_count + 1;
				}
			}
			else if(prev != 0 && curr == 0)
			{
				printf("Add null terminator. String is done!\n");
			}
		}
		char_count = char_count + 1;
		prev = character;
	}

	// Check for errors or end of file
	if (ferror(file))
	{
		perror("Error reading file");
		exit(1);
	}

	// Close the file
	fclose(file);
	
}

void loadInodesList(char *filename, Inode inodeList[], size_t *inodeCount)
{
	FILE *file;
	char character; // Variable to store the read character

	// Open the binary file in read mode
	file = fopen(filename, "rb"); // "rb" stands for read binary

	// Check if the file was opened successfully
	if (file == NULL)
	{
		perror("Error opening file");
		exit(1);
	}

	// Read characters from the file and print them as characters
	int char_count = 1;
	int index = 0;
	uint32_t inode = 0;
	char type = '\0';
	uint32_t inodeNumList[1024] = {0};
	char typeList[1024] = {'\0'};
	while (fread(&character, sizeof(char), 1, file) == 1)
	{
		if(char_count == 1 || char_count % 5 == 1)
		{
			//printf("INODE: %d\n", character);
			inode = character;
		}
		else if(character != 0)
		{
			//printf("TYPE: %c\n", character);
			type = character;
			printf("NEW FILE/DIR: %d, %c\n", inode, type);
			inodeNumList[index] = inode;
			typeList[index] = type;

			Inode node; //adding to inode list
			node.inode = inode;
			node.parentInode = 0;
			node.type = type;
			for (int i = 0; i < 32; i++) {
				node.name[i] = '\0';
			}
			inodeList[index] = node;

			*inodeCount = *inodeCount + 1;
			index = index + 1;
		}
		char_count = char_count + 1;
	}

	// Check for errors or end of file
	if (ferror(file))
	{
		perror("Error reading file");
		exit(1);
	}

	// Close the file
	fclose(file);
	
	printf("%c", '\n');
	
	for(size_t i = 0; i < index; i++)
	{
		printf("NEW INODE: %d\n", inodeNumList[i]);
		loadInode(inodeNumList[i], typeList[i], i, inodeList);
		printf("%c", '\n');
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
        printf("No directory provided\n");
        exit(-1);
    }

	char *dirname = argv[1];
    printf("directory name: %s\n", dirname);

	if (chdir(dirname) != 0) {
		perror(dirname);
		exit(-1);
	}

	Inode inodeList[1024]; //init some inode variables
	size_t inodeCount = 0;
	uint32_t currentInode = 0;

	// Read inodes_list file and load fs data
	loadInodesList("inodes_list", inodeList, &inodeCount);

	printf("total inodes: %d\n", (int)inodeCount);

	

	for (int i = 0; i < inodeCount; i++) {
		Inode in = inodeList[i];
		printf("inode: %d\n", in.inode);
		printf("parent: %d\n", in.parentInode);
		printf("type: %c\n", in.type);
		printf("name: %s\n", in.name);
		printf("\n");
	}
	
	char input[MAX_LENGTH + 2];
	int val_command = 0;

	while (1) {
		val_command = 0;
		printf("> ");
		fgets(input, sizeof(input), stdin);

		input[strcspn(input, "\n")] = '\0';

		if((strlen(input)) > MAX_LENGTH) {
			printf("input too long\n");
			while (getchar() != '\n');
			continue;
		}

		if (strcmp(input, "exit") == 0) {
            printf("Exiting...\n");

			char type;
			//char null = '\0';

			FILE *listp;
			listp = fopen("inodes_list", "wb");
			if (listp == NULL) {
				printf("opening inodelist failed\n");
				return 0;
			}

			for (int i = 0; i < inodeCount; i++) {
				fwrite(&i, sizeof(int), 1, listp);
				// for (int i = 0; i < 3; i++) {
				// 	fwrite(&null, 1, 1, listp);
				// }
				type = inodeList[i].type;
				fwrite(&type, sizeof(char), 1, listp);
			}
			printf("inodelist updated;\n");
			fclose(listp);
			
            break; // Exit the loop if input is "exit"
        }
		//printf("command entered: %s\n", input);

		if (strcmp(input, "ls") == 0) {
			val_command = 1;
			listContents(inodeList, currentInode, inodeCount);
		} else {
			char *token = strtok(input, " \t\n");

			if (token != NULL && strcmp(token, "cd") == 0) {
				int count = 0;
				val_command = 1;
				char directory[100];

				while ((token = strtok(NULL, " \t\n")) != NULL) {
					count++;
					if (count > 1) {
						printf("too many arguments provided for cd\n");
						break;
					}
					strncpy(directory, token, sizeof(directory));
					directory[sizeof(directory)-1] = '\0';
					//printf("directory entered: %s\n", directory);
				}

				if (count == 0) {
					printf("directory not given after cd\n");
					continue;
				} else if (count == 1) {
					changeDirectory(directory, inodeList, &currentInode, inodeCount);
				}
			}

			if (token != NULL && strcmp(token, "mkdir") == 0) {
				int count = 0;
				val_command = 1;
				char directory[100];

				while ((token = strtok(NULL, " \t\n")) != NULL) {
					count++;
					if (count > 1) {
						printf("too many arguments provided for mkdir\n");
						break;
					}
					strncpy(directory, token, sizeof(directory));
					if (strlen(directory) > 32) {
						printf("name exceeds max of 32 characters\n");
						count = 2;
						break;
					}
					directory[sizeof(directory)-1] = '\0';
					//printf("directory entered: %s\n", directory);
				}

				if (count == 0) {
					printf("name not given after mkdir\n");
					continue;
				} else if (count == 1) {
					printf("make directory %s\n", directory);
					createDirectory(directory, inodeList, &currentInode, &inodeCount);
					printf("total inodes now: %d\n", (int)inodeCount);
				}
			}

			if (token != NULL && strcmp(token, "touch") == 0) {
				int count = 0;
				val_command = 1;
				char file[100];

				while ((token = strtok(NULL, " \t\n")) != NULL) {
					count++;
					if (count > 1) {
						printf("too many arguments provided for touch\n");
						break;
					}
					strncpy(file, token, sizeof(file));
					if (strlen(file) > 32) {
						printf("name exceeds max of 32 characters\n");
						count = 2;
						break;
					}
					file[sizeof(file)-1] = '\0';
					//printf("file entered: %s\n", file);
				}

				if (count == 0) {
					printf("name not given after touch\n");
					continue;
				} else if (count == 1) {
					//printf("make file %s\n", file);
					createFile(file, inodeList, &currentInode, &inodeCount);
					//printf("total inodes now: %d\n", (int)inodeCount);
				}
			}

			if (val_command == 0) {
				printf("invalid command\n");
			}
		}

	}

    return 0;
}