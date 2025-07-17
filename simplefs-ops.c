#include "simplefs-disk.h"

/* Array for storing opened files */
extern struct filehandle_t file_handle_array[MAX_OPEN_FILES];

/*
int main()
{
 // char str[] = "!-----------------------64 Bytes of Data-----------------------!";
    simplefs_formatDisk();

    simplefs_create("f1.txt");
    simplefs_create("f2.txt");
    simplefs_create("f3.txt");
    simplefs_create("f4.txt");
    simplefs_create("f5.txt");
    simplefs_create("f6.txt");
    simplefs_create("f7.txt");
    simplefs_create("f8.txt");
    simplefs_create("f9.txt");

    
    simplefs_create("f1.txt");
    int fd1 = simplefs_open("f1.txt");
    simplefs_create("f2.txt");
    int fd2 = simplefs_open("f2.txt");

    printf("Write Data: %d\n", simplefs_write(fd1, str, BLOCKSIZE));
    printf("Write Data: %d\n", simplefs_write(fd2, str, BLOCKSIZE));
    printf("Seek: %d\n", simplefs_seek(fd1, BLOCKSIZE));
    printf("Seek: %d\n", simplefs_seek(fd2, BLOCKSIZE));
    printf("Write Data: %d\n", simplefs_write(fd1, str, BLOCKSIZE));
    printf("Write Data: %d\n", simplefs_write(fd2, str, BLOCKSIZE));
    

    simplefs_dump();
    //simplefs_delete("f2.txt");
    //simplefs_dump();
    //simplefs_delete("f1.txt");
    //simplefs_dump();
}
*/

/*
int main()
{
	printf("----------------------------------------------------\n");

	simplefs_formatDisk();

	simplefs_dump();

	simplefs_create("hii");
	
	//simplefs_create("hello");
	
	//simplefs_create("byy");

	//simplefs_dump();

	int fd1 = simplefs_open("hii");
	
	//int fd2 = simplefs_open("hello");
	
	//simplefs_open("hello");
	
	//simplefs_open("byy");
	
	//simplefs_open("hii");

	//simplefs_delete("hii");
 
	//simplefs_dump();

	//simplefs_delete("hii");

	//simplefs_dump();

	//printf("number is %d\n",file_handle_array[1].inode_number);

	//int fd = simplefs_open("hii");
	
	//simplefs_open("hello");
	
	//char *buf1 = "qweruiopasdfdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuiopasayushiopasdfdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuishah";
	
	char *buf2 = "ushiopasdfdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnmqwertyuiopassdqqqqqq";

	char *buf3 = "Hello";
	char *buf4 = "hii";
	
	simplefs_write(fd1 , buf2 , 70);

	//simplefs_write(fd1 , buf4 , 3);
	
	simplefs_seek(fd1, 70);

	simplefs_write(fd1, buf4, 3);
	
	simplefs_dump();

	//simplefs_read(fd1 , buf3 , 64);
	//simplefs_delete("hii");

	//simplefs_dump();

	//printf("%s\n",buf3);

	for(int i = 0 ; i < MAX_OPEN_FILES ; i++)
	{
		printf("%d ",file_handle_array[i].inode_number);
	}



	//printf("number is %d\n",file_handle_array[0].inode_number);

	//simplefs_close(0);

        //printf("number is %d\n",file_handle_array[1].inode_number);

	//simplefs_dump();
}
*/

/*
iAuthor:166860_Ayush
Function Name: simplefs_create
Function Description: Takes filename as parameter and  creates file with name `filename` from disk
Created: 22-04-2025
Modified: 22-04-2025
*/
int simplefs_create(char *filename)
{
	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));

	char tempBuf[32];

	/* Loop to check if file already exists or not */
	for(int inodenum = 0 ; inodenum < NUM_INODES ; inodenum++)
        {
		simplefs_readInode( inodenum , inode );

		if(  ( strcmp(inode->name , filename) ) == 0 )
		{
			free(inode);

			return -1;
		}
	}

	/* Allocating the new inode if available */
	int allocated_inode = simplefs_allocInode();

	if( allocated_inode == -1 )
	{
		//printf("Inode is not available\n");

		free(inode);

		return -1;
	}
	
	simplefs_readInode( allocated_inode , inode );

	/* Updating inode structur */
	memcpy(inode->name, filename, strlen(filename) + 1);

	inode->status = INODE_IN_USE;

	simplefs_writeInode(allocated_inode , inode);

	free(inode);

	return allocated_inode;
}

/*
Author:166860_Ayush
Function Name: simplefs_delete
Function Description: delete file with name `filename` from disk
Created: 22-04-2025
Modified: 22-04-2025
*/
void simplefs_delete(char *filename)
{
	struct inode_t *inode = (struct inode_t *)malloc( sizeof(struct inode_t) );

        char tempBuf[32];

        /* Loop for checking the inode number of the file to be deleted */
	for( int inodenum = 0 ; inodenum < NUM_INODES ; inodenum++ )
        {
		simplefs_readInode(inodenum , inode);

                if(  ( strcmp(inode->name , filename) ) == 0 )
                {
			//printf("File exists\n");
 
			/* Removing the blocks allocated */
			for (int iterate_blocks = 0 ; iterate_blocks < MAX_FILE_SIZE ; iterate_blocks++)
			{
				if (inode->direct_blocks[iterate_blocks] != -1) 
                		{
                    			simplefs_freeDataBlock(inode->direct_blocks[iterate_blocks]);
				}		
		       	}

			/* Removing the file handles if allocated */
			for( int iterate_file_handle = 0 ; iterate_file_handle < MAX_OPEN_FILES ; iterate_file_handle++ )
                        {
				if( file_handle_array[iterate_file_handle].inode_number == inodenum )
                                {
                                        file_handle_array[iterate_file_handle].inode_number = -1;

                                        file_handle_array[iterate_file_handle].offset = 0;
                                }
			}


			/* Updating the inode structure after deleting the file */
			simplefs_freeInode(inodenum);

			free(inode);

			return;                        
                
		}
	}

	free(inode);
	
	printf("Not found\n");
}

/*
Author:166860_Ayush
Function Name:simplefs_open
Function Description: open file with name `filename`
Created: 22-04-2025
Modified: 22-04-2025
*/
int simplefs_open(char *filename)
{
	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));

        char tempBuf[32];

        /* Loop for checking the inode number of the file to be open */
        for(int inodenum = 0 ; inodenum < NUM_INODES ; inodenum++)
        {
                simplefs_readInode(inodenum , inode);

                if( ( inode->status  == INODE_IN_USE ) && ( strcmp(inode->name , filename) ) == 0 )
                {
                        //printf("File exists\n");

			/* Loop for assigning the file decsriptor from the list if available */
                        for( int i = 0 ; i < MAX_OPEN_FILES ; i++ )
        		{
                		if( file_handle_array[i].inode_number == -1 )
				{
					file_handle_array[i].inode_number = inodenum;

					file_handle_array[i].offset = 0;

					free(inode);

					return i;
				}

        		}
                }
        }

	//printf("File do not exist\n");
	
	free(inode);

	return -1;
}

/*
Author:166860_Ayush
Function Name: simplefs_close
Function Description: close file pointed by `file_handle`
Created: 22-04-2025
Modified: 22-04-2025
*/
void simplefs_close(int file_handle)
{
	/* Validating the file_handel */
	if (file_handle < 0 || file_handle >= MAX_OPEN_FILES) 
	{
		printf("Invalid file handle\n");
        
		return;
	}

       	if (file_handle_array[file_handle].inode_number == -1) 
	{
		printf("File already closed\n");
        
		return;
	}
	
	/* Removing the file_handle */
	file_handle_array[file_handle].inode_number = -1;
	
	file_handle_array[file_handle].offset = 0;
}


/*
Author:166860_Ayush
Function Name: simplefs_read
Function Description: read `nbytes` of data into `buf` from file pointed by `file_handle` starting at current offset
Created: 22-04-2025
Modified: 22-04-2025
*/
int simplefs_read(int file_handle, char *buf, int nbytes)
{
	if (file_handle < 0 || file_handle >= MAX_OPEN_FILES) 
	{
		printf("Invalid file handle\n");
        
		return -1;
	}

	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));

	int inodenum = file_handle_array[file_handle].inode_number;

	int file_offset = file_handle_array[file_handle].offset;

	simplefs_readInode(inodenum, inode);
	
	if (inodenum == -1) 
	{
		printf("File not open\n");
        
		free(inode);

		return -1;
	}

	if (file_offset + nbytes > inode->file_size)
	{
		printf("Trying to read beyond file size\n");
        
		free(inode);
        
		return -1;
	}

	int start_block = file_offset / BLOCKSIZE;
    
	int end_block = (file_offset + nbytes - 1) / BLOCKSIZE;

    	int bytes_remaining = nbytes;
    
	int buffer_offset = 0;
    
	int curr_file_offset = file_offset;

    	/* Read blocks */
    	for (int i = start_block; i <= end_block; i++)
	{
		int block_offset = curr_file_offset % BLOCKSIZE;
        
		int to_read = BLOCKSIZE - block_offset;
        
		if (to_read > bytes_remaining)
		{
			to_read = bytes_remaining;
		}

        	/* Seek to correct position */
        	lseek(DISK_FD, (5 * BLOCKSIZE) + inode->direct_blocks[i] * BLOCKSIZE + block_offset, SEEK_SET);

        	int r = read(DISK_FD, buf + buffer_offset, to_read);
        
		if (r != to_read)
		{
			printf("Disk read error\n");
            
			free(inode);
            
			return -1;
		}

        	buffer_offset += to_read;
        
		bytes_remaining -= to_read;
        
		curr_file_offset += to_read;
	}

    	free(inode);
    
	return 0;  
}


/*
Author:166860_Ayush
Function Name: simplefs_write
Function Description: write `nbytes` of data from `buf` to file pointed by `file_handle` starting at current offset
Created: 22-04-2025
Modified: 22-04-2025
*/

int simplefs_write(int file_handle, char *buf, int nbytes)
{
        /* condition for invalid file descriptor */
	if(file_handle < 0 || file_handle >= MAX_OPEN_FILES)
	{
		return -1;
	}

        int inodenum = file_handle_array[file_handle].inode_number;
        
	if(inodenum == -1)
	{
		return -1;
	}

        struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
        
	simplefs_readInode(inodenum, inode);

        /* Condition to check the overflow of the file contents */
	if(file_handle_array[file_handle].offset + nbytes > MAX_FILE_SIZE * BLOCKSIZE)
        {
                free(inode);
                
		return -1;
        }

        int offset = file_handle_array[file_handle].offset;
        
	int remaining = nbytes;
        
	int buf_index = 0;

        while(remaining > 0)
        {
		int block_idx = offset / BLOCKSIZE;
                
		int block_offset = offset % BLOCKSIZE;

                if(block_idx >= MAX_FILE_SIZE)
                {
                        free(inode);
                        
			return -1;
                }

                if(inode->direct_blocks[block_idx] == -1)
                {
                        int blocknum = simplefs_allocDataBlock();
                        
			if(blocknum == -1)
                        {
                                free(inode);
                                
				return -1;
                        }
                        
			inode->direct_blocks[block_idx] = blocknum;

                        /* Filing the contents with zero initially */
			char zero_block[BLOCKSIZE] = {0};
                        
			simplefs_writeDataBlock(blocknum, zero_block);
                }

                char block[BLOCKSIZE];
                
		simplefs_readDataBlock(inode->direct_blocks[block_idx], block);

                int to_write = BLOCKSIZE - block_offset;
                
		if(to_write > remaining)
		{
			to_write = remaining;
		}

                memcpy(block + block_offset, buf + buf_index, to_write);
                
		simplefs_writeDataBlock(inode->direct_blocks[block_idx], block);

                offset += to_write;
                
		buf_index += to_write;
                
		remaining -= to_write;
        }

        if(file_handle_array[file_handle].offset + nbytes > inode->file_size)
	{
		inode->file_size = file_handle_array[file_handle].offset + nbytes;
	}

        simplefs_writeInode(inodenum, inode);

        free(inode);
        
	return 0;
}

/*
Author:166860_Ayush
Function Name: simplefs_seek
Function Description: increase `file_handle` offset by `nseek`
Created: 22-04-2025
Modified: 22-04-2025
*/
int simplefs_seek(int file_handle, int nseek)
{
	if (file_handle < 0 || file_handle >= MAX_OPEN_FILES) 
	{
		//printf("Invalid file handle\n");
        
		return -1;
	}

    	int inode_num = file_handle_array[file_handle].inode_number;
    
	if (inode_num == -1) 
	{
		//printf("File not open\n");
        
		return -1;
	}

    	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
    
	simplefs_readInode(inode_num, inode);

    	int current_offset = file_handle_array[file_handle].offset;
   
       	int new_offset = current_offset + nseek;

    	if (new_offset < 0 || new_offset > inode->file_size) 
	{
		//printf("Seek out of bounds\n");
        
		free(inode);
        
		return -1;
	}

    	file_handle_array[file_handle].offset = new_offset;
    
	free(inode);
    
	return 0;
}

