#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "funcs.h"

void listContents(Inode inodeList[], int current, int total) {
	printf("%d .\n", current);
	printf("%d ..\n", inodeList[current].parentInode);
	for (int i = 0; i < total; i++) {
		if (inodeList[i].parentInode == current && inodeList[i].inode != current) {
			printf("%d %s\n", inodeList[i].inode, inodeList[i].name);
		}
	}
}

void changeDirectory(const char *name, Inode inodeList[], uint32_t *current, int total) {
	int found = 0;

	if (strcmp(name, ".") == 0) {
		*current = inodeList[*current].inode;
		found = 1;
	} else if (strcmp(name, "..") == 0) {
		*current = inodeList[*current].parentInode;
		found = 1;
	}
	if (found == 0) {
		for (int i = 0; i < total; i++) {
			if (inodeList[i].parentInode == *current && inodeList[i].inode != *current && inodeList[i].type == 'd' && strcmp(inodeList[i].name, name) == 0) {
				*current = inodeList[i].inode;
				found = 1;
				break;
			}
		}
	}

	if (found == 0)
		printf("error: directory does not exist\n");
}