/*
  Copyright (C) 2015 CS416/CS516

  This program can be distributed under the terms of the GNU GPLv3.
  See the file COPYING.
*/

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "block.h"
#include "knoten.h"

int diskfile = -1;

void disk_open(const char* diskfile_path)
{
    if(diskfile >= 0){
	return;
    }
    
    diskfile = open(diskfile_path, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if (diskfile < 0) {
	perror("disk_open failed");
	exit(EXIT_FAILURE);
    }
}

void disk_close()
{
    if(diskfile >= 0){
	close(diskfile);
    }
}

/** Read a block from an open file
 *
 * Read should return (1) exactly @BLOCK_SIZE when succeeded, or (2) 0 when the requested block has never been touched before, or (3) a negtive value when failed. 
 * In cases of error or return value equals to 0, the content of the @buf is set to 0.
 */
int block_read(const int block_num, void *buf)
{
    int retstat = 0;
    retstat = pread(diskfile, buf, BLOCK_SIZE, block_num*BLOCK_SIZE);
    if (retstat <= 0){
	memset(buf, 0, BLOCK_SIZE);
	if(retstat<0)
	perror("block_read failed");
    }

    return retstat;
}


int sBlock_read(const int block_num, void *buf, int size)
{
 	int retstat = 0;
    retstat = pread(diskfile, buf, size, 0);
    if (retstat <= 0){
	memset(buf, 0, size);
	if(retstat<0)
	perror("block_read failed");
    }

    return retstat;
}

/*
*  Same as block_read except this is used for inodes
*  The only difference is the offset number used to account for
*  the current offset in the block
*/
int inode_read(const int block_num, void *buf, int offset)
{
    int retstat = 0;
    retstat = pread(diskfile, buf, sizeof(fileControlBlock), (block_num * BLOCK_SIZE) + offset);
    if (retstat <= 0){
    	memset(buf, 0, BLOCK_SIZE);
    if(retstat<0)
    perror("block_read failed");
    }

    return retstat;
}

/** Write a block to an open file
 *
 * Write should return exactly @BLOCK_SIZE except on error. 
 */
int block_write(const int block_num, const void *buf)
{
    int retstat = 0;
    retstat = pwrite(diskfile, buf, BLOCK_SIZE, block_num*BLOCK_SIZE);
    if (retstat < 0)
	perror("block_write failed");
    
    return retstat;
}

int sBlock_write_padded(const int block_num, const void *buf, int size)
{
    int retstat = 0;
    char tmp_buffer[size];
    memset(tmp_buffer, '0', sizeof(tmp_buffer));

	
    retstat = pwrite(diskfile, tmp_buffer, sizeof(tmp_buffer), 0);
    if (retstat >= 0) {
        retstat = pwrite(diskfile, buf, size, 0);
    }

    if (retstat < 0)
    	perror("block_write failed");

    return retstat;
}


int block_write_padded(const int block_num, const void *buf, int size, int offset)
{
    int retstat = 0;
    char tmp_buffer[BLOCK_SIZE];
    memset(tmp_buffer, '0', sizeof(tmp_buffer));

    retstat = pwrite(diskfile, tmp_buffer, size, (block_num * BLOCK_SIZE) + offset);
    if (retstat >= 0) {
        retstat = pwrite(diskfile, buf, size, 
			(block_num * BLOCK_SIZE) + offset);
    }

    if (retstat < 0)
    	perror("block_write failed");

    return retstat;
}

