#ifndef FUNCS_H
#define FUNCS_H

typedef struct
{
	uint32_t inode;
	uint32_t parentInode;
	char type;
	char name[32];
} Inode;

void listContents(Inode inodeList[], int current, int total);

void changeDirectory(const char *name, Inode inodeList[], uint32_t *current, int total);

void createDirectory(const char *name, Inode inodeList[], uint32_t *current, size_t *total);

void createFile(const char *name, Inode inodeList[], uint32_t *current, size_t *total);

#endif