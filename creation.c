#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "funcs.h"


void createDirectory(const char *name, Inode inodeList[], uint32_t *current, size_t *total) {
	int valid = 1;

	if (*total >= 1024)
		valid = 2;
	if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
		valid = 0;
	}
	if (valid == 1) {
		for (int i = 0; i < *total; i++) {
			if (inodeList[i].parentInode == *current && inodeList[i].inode != *current && strcmp(inodeList[i].name, name) == 0) {
				valid = 0;
				break;
			}
		}
	}

	if (valid == 0) {
		printf("error: invalid directory name or name already exists\n");
	} else if (valid == 2) {
		printf("error: inodes maxed out at 1024\n");
	}
	else {
		char fname[20];
		FILE *fp;
		snprintf(fname, sizeof(fname), "%d", (int)*total);

		Inode inode;
		inode.inode = *total;
		inode.parentInode = *current;
		inode.type = 'd';
		strcpy(inode.name, name);
		inodeList[*total] = inode;

		fp = fopen(fname, "w");
		if (fp == NULL) {
			printf("directory creation failed, rut ro\n");
			return;
		}

		//write into new directory file
		int inode_num = *total;
		int par = *current;
		char dot = '.';
		
		fwrite(&inode_num, sizeof(int), 1, fp); //basically adds the default . and ..
		fputc('\0', fp);
		fwrite(&dot, sizeof(char), 1, fp);
		for (int i = 0; i < 20; i++) {
			fputc('\0', fp);
		}
		fwrite(&par, sizeof(int), 1, fp);
		fputc('\0', fp);
		fwrite(&dot, sizeof(char), 1, fp);
		fwrite(&dot, sizeof(char), 1, fp);
		for (int i = 0; i < 20; i++) {
			fputc('\0', fp);
		}
		
		fclose(fp);

		//updating current directory
		char parent[20];
		snprintf(parent, sizeof(parent), "%d", (int)*current);
		
		fp = fopen(parent, "ab");
		if (fp == NULL) {
			printf("directory update failed, rut ro\n");
			return;
		}

		fwrite(&inode_num, sizeof(int), 1, fp);
		fputc('\0', fp);
		fwrite(name, sizeof(char), strlen(name), fp);
		for (int i = 0; i < 20; i++) {
			fputc('\0', fp);
		}

		fclose(fp);

		(*total)++;
	}
}

void createFile(const char *name, Inode inodeList[], uint32_t *current, size_t *total) {
	int valid = 1;

	if (*total >= 1024)
		valid = 2;
	if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
		valid = 0;
	}
	if (valid == 1) {
		for (int i = 0; i < *total; i++) {
			if (inodeList[i].parentInode == *current && inodeList[i].inode != *current && strcmp(inodeList[i].name, name) == 0) {
				valid = 0;
				break;
			}
		}
	}

	if (valid == 0) {
		printf("error: invalid file name or name already exists\n");
	} else if (valid == 2) {
		printf("error: inodes maxed out at 1024\n");
	}
	else {
		char fname[20];
		FILE *fp;
		snprintf(fname, sizeof(fname), "%d", (int)*total);

		Inode inode;
		inode.inode = *total;
		inode.parentInode = *current;
		inode.type = 'f';
		strcpy(inode.name, name);
		inodeList[*total] = inode;

		fp = fopen(fname, "w");
		if (fp == NULL) {
			printf("file creation failed, rut ro\n");
			return;
		}

		fprintf(fp, "%s", name); //writing file name inside inode file

		fclose(fp);

		char parent[20];
		snprintf(parent, sizeof(parent), "%d", (int)*current);
		int inode_num = *total;
		
		fp = fopen(parent, "ab");
		if (fp == NULL) {
			printf("directory update failed, rut ro\n");
			return;
		}

		fwrite(&inode_num, sizeof(int), 1, fp);
		fputc('\0', fp);
		fwrite(name, sizeof(char), strlen(name), fp);
		for (int i = 0; i < 20; i++) {
			fputc('\0', fp);
		}

		fclose(fp);

		(*total)++;
	}
}