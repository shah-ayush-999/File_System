/* DISK EMULATION */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

#define BLOCKSIZE 64
#define NUM_BLOCKS 35
#define NUM_DATA_BLOCKS 30
#define NUM_INODE_BLOCKS 4
#define NUM_INODES 8
#define NUM_INODES_PER_BLOCK 2
#define MAX_FILE_SIZE 4 // In Blocks
#define MAX_FILES 8
#define MAX_OPEN_FILES 20
#define MAX_NAME_STRLEN 8
#define INODE_FREE 'x'
#define INODE_IN_USE '1'
#define DATA_BLOCK_FREE 'x'
#define DATA_BLOCK_USED '1'

extern int DISK_FD;

struct superblock_t
{
	/* "simplefs" after formatting */
	char name[MAX_NAME_STRLEN]; 			
	
	/* INODE_FREE if free, INODE_IN_USE if used */
	char inode_freelist[NUM_INODES];			
	
	/* DATA_BLOCK_FREE if free, DATA_BLOCK_USED if used */
	char datablock_freelist[NUM_DATA_BLOCKS];  
};

struct inode_t
{
	/* INODE_FREE if free, INODE_IN_USE if used */
	int status;						
	
	/* name of the file */
	char name[MAX_NAME_STRLEN];							
	/* size of the file in bytes */
	int file_size;
	
	/* -1 if free, block number if used */
	int direct_blocks[MAX_FILE_SIZE];			
};

struct filehandle_t
{
	/* current offset in opened file */
	int offset;		  
	
	/* Inode number for the file */
	int inode_number; 
};

void simplefs_formatDisk();

int simplefs_allocInode();

void simplefs_freeInode(int inodenum);

void simplefs_readInode(int inodenum , struct inode_t *inodeptr);

void simplefs_writeInode(int inodenum , struct inode_t *inodeptr); 

int simplefs_allocDataBlock();

void simplefs_freeDataBlock(int blocknum);

void simplefs_readDataBlock(int blocknum , char *buf);

void simplefs_writeDataBlock(int blocknum , char *buf);

void simplefs_dump();

int simplefs_create(char *filename);

void simplefs_delete(char *filename);

int simplefs_open(char *filename);

void simplefs_close(int file_handle);

int simplefs_read(int file_handle, char *buf, int nbytes);

int simplefs_write(int file_handle, char *buf, int nbytes);

int simplefs_seek(int file_handle, int nseek);
