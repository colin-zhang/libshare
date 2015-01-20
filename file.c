/***********************************************************/
/*                   _                                     */
/*        ___       | |                                    */
/*      //     ___  | | || |\ ||                           */
/*     ||     / _ \ | | || ||\||                           */
/*      \\___ \___/ |_| || || \|                           */
/*                                                 @2014   */
/***********************************************************/

#include <sys/stat.h>
#include <sys/statfs.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include "file.h"
#include "crc16.h"

char df(char *lpDisk,unsigned int *lpAvail,unsigned int *lpUsed,unsigned int *lpUsedRate)
{
	struct statfs	fstate;
	if (NULL == lpDisk)
	{	
		printf("%d\n",__LINE__ );
		return -1;
	}
	if(!statfs(lpDisk, &fstate))
	{
		*lpAvail = (fstate.f_bsize * fstate.f_bavail) / 1024;			
		if(fstate.f_blocks > fstate.f_bavail)
		{
			*lpUsed = (fstate.f_bsize * (fstate.f_blocks - fstate.f_bavail)) / 1024;
			*lpUsedRate = (((*lpUsed) * 100) / (*lpAvail + *lpUsed + 1)) + 1;
		}
		else 
		{
			*lpUsed = 0;
			*lpUsedRate = 0;
		}
		return 0;
	}
	return -1;
}


int get_file_size(const char *path)
{  
    int    file_size = -1;      
    struct stat statbuff;  

    if(stat(path, &statbuff) < 0)
    {  
        return file_size;  
    } 
    else
    {  
        file_size = statbuff.st_size;  
        return file_size;  
    }  
}


int file_exist(char *file_path)
{
    struct stat buf;
    int ret = stat(file_path, &buf);
    return (0==ret);
}



bool is_dir(const char *path)
{
    struct stat statbuf;
    if(lstat(path, &statbuf) == 0)
    {
        return S_ISDIR(statbuf.st_mode) != 0;
    }
    return false;
}

bool is_fifo(const char *file)
{
	struct stat statbuf;
	if(lstat(file,&statbuf) == 0)
	{
		return S_ISFIFO(statbuf.st_mode) != 0;
	}
	return false;
}

bool is_file(const char *path)
{
    struct stat statbuf;
    if(lstat(path, &statbuf) == 0)
    {
        return S_ISREG(statbuf.st_mode) != 0;
    }
    return false;
}

bool is_special_dir(const char *path)
{
    return strcmp(path, ".") == 0 || strcmp(path, "..") == 0;
}

void get_file_path(const char *path, const char *file_name,  char *file_path)
{
    strcpy(file_path, path);
    if(file_path[strlen(path) - 1] != '/')
        strcat(file_path, "/");
    strcat(file_path, file_name);
}

void delete_file(const char *path)
{
    DIR* dir;
    char file_path[PATH_MAX];
    struct dirent *dir_info;

    if(is_file(path))
    {
        remove(path);
        return;
    }
    if(is_dir(path))
    {
        if((dir = opendir(path)) == NULL)
        {
            return;
        }
        while((dir_info = readdir(dir)) != NULL)
        {
            get_file_path(path, dir_info->d_name, file_path);
            if(is_special_dir(dir_info->d_name))
            {
                continue;
            }
            delete_file(file_path);
            rmdir(file_path);
        }
        closedir(dir);
    }
    rmdir(path);
}

int make_dir(const char* dir, mode_t mode)
{
    const char* p = dir;
    int   len = 0;
    char  tmp[strlen(dir) + 1];

    while((p = strchr(p, '/')) != NULL)
     {
     	
        len = p - dir;
        if(len > 0)
        {
            memcpy(tmp, dir, len);
            tmp[len] = '\0';

            if((mkdir(tmp, mode) < 0) && (errno != EEXIST))
            {
                return -3;
            }
        }
        p += 1;
    }
   

    if((mkdir(dir, mode) < 0) && (errno != EEXIST))
    {
        return -2;
    }
   
    return 0;
}

char *file_to_alloc_mem(const char *path,int *file_len_out)
{
	FILE *file;

	if (!(file = fopen(path, "rb")))
	{
		fprintf(stderr, "Unable to open file %s\n", path);
		return NULL;
	}

	if (-1 == fseek(file, 0, SEEK_END))
	{
		fprintf(stderr, "Unable to seek file %s\n", path);
		return NULL;
	}

	int file_len;
	if (-1 == (file_len = ftell(file)))
	{
		fprintf(stderr, "Unable to ftell() file %s\n", path);
		return NULL;
	}

	if (-1 == fseek(file, 0, SEEK_SET))
	{
		fprintf(stderr, "Unable to seek file %s\n", path);
		return NULL;
	}

	char *contents;
	if (!(contents = malloc(file_len + 1)))
	{
		fprintf(stderr, "Memory error!\n");
		fclose(file);
		return NULL;
	}

	fread(contents, 1, file_len, file);

	fclose(file);

	contents[file_len] = '\0';

	if (file_len_out)
	*file_len_out = file_len;

	return contents;
}



void write_2_file(int fd, const char* buf, int size)
{
    int result = -1;

    do
    {
        result = write(fd, buf, size);
    } while((result < 0) && (errno == EINTR));

    return;
}

int read_file(char* buffer, int size, const char* path)
{
    int ret = -1; 
    int fd = open(path, O_RDONLY);
    if (fd == -1) 
    {   
        snprintf(buffer, size, "can not open file : %s, %d - %s",
                path, errno, strerror(errno));
    }   
    else
    {   
        ret = read(fd, buffer, size);
        close(fd);

        if (ret < 0)
        {   
            snprintf(buffer, size, "read file : \"%s\" error, %d - %s",
                    path, errno, strerror(errno));
        }   
    }   
    return ret;
}



int copy_file(const char *src_file,const char *des_file)
{

#define BUFSIZE 4096
	int flag = 0,ret = 0 ,written = 0;
	int input = 0,output = 0;
	char *buffer = NULL;
	char *p = NULL; 
	
	if(NULL == src_file || NULL == des_file){
		return -1;
	}

	buffer = (char*)calloc(BUFSIZE,sizeof(char));
	if(NULL == buffer){
		return -1;
	}
	
	input = open(src_file,O_RDONLY);
	if(input < 0){
		printf("src_file failed\n");
		free(buffer);
		return -1;
	}

	output = open(des_file,O_RDWR|O_CREAT);
	if(output < 0){
		printf("des_file failed\n");
		free(buffer);
		close(input);
		return -10;		
	}
	
	for (;;) { 
        ret = read(input, buffer, BUFSIZE);         
        if (!ret){
        	flag = 0;
        	break; 
        }
                
        if (ret < 0) { 
                if (errno == EINTR) 
                        continue; 
                //.. exit with an inpot error .. 
                flag = -1;
                break;
        } 
        p = buffer; 
        do { 
                written = write(output, p, ret); 
                if (!written){
					//.. exit with filesystem full .. 
					flag = -2;
					goto r1;
                } 

                if (written < 0) { 
                        if (errno == EINTR) 
                                continue; 
                    //.. exit with an output error .. 
                    flag = -3;
                    goto r1;
                } 
                p += written; 
                ret -= written; 
        } while (ret); 
 	} 

r1: 	
    
    close(output);
    close(input);
    free(buffer);
 	return flag;

}


FILE *std_open_file(uint8_t *lpFile,uint8_t *lpMode)
{
	uint8_t temp[256];
	uint32_t i;
	FILE *fp;
	
	if(access(lpFile, F_OK) != 0)								
	{
		for(i = strlen(lpFile); i > 0; i--)
		{
			if(lpFile[i-1] == '/')
			{
				memset(temp,0,sizeof(temp));				
				memcpy(temp,lpFile,i-1);
				if(make_dir(temp,0777)){						
					return NULL;
				}
				fp = fopen(lpFile,"wb+");						
				fclose(fp);
				break;
			}	
		}					
	}		
	fp = fopen(lpFile,lpMode); 									
	return fp;
}




uint8_t std_write_file(uint8_t *lpFile,uint8_t *data,uint32_t len)
{
	FILE *fp;
	
	fp = std_open_file(lpFile,"wb+"); 						
	if(fp == NULL)
	{
		return FILE_ERROR;  								
	}
	if(fwrite(data, sizeof(uint8_t), len, fp) != len) 	
	{
		fclose(fp);									
		return FILE_ERROR;								
	}
	else
	{
		fflush(fp); 										
		fclose(fp);									
	}	
	return FILE_OK;									
}


uint8_t std_read_file(uint8_t *lpFile, uint8_t *lpOut, uint32_t nStart, uint32_t *lpLen)
{
	FILE *fp;
	uint32_t i; 
	
	fp = std_open_file(lpFile, "rb"); 						
	if(fp == NULL)
	{
		*lpLen = 0;
		return FILE_ERROR;								
	}
	fseek(fp,nStart,SEEK_SET); 			
	i = *lpLen;
	*lpLen = fread(lpOut,sizeof(uint8_t),i,fp);
	if(*lpLen != i)
	{
		if(feof(fp)) 										
		{
			fclose(fp);
			return FILE_EOF; 								
		}
		else
		{
			fclose(fp);
			return FILE_ERROR;																	
		}
	}	
	fclose(fp); 					
	return FILE_OK;	
}

uint8_t std_append_file(uint8_t *lpFile, uint8_t *lpIn, uint32_t nLen)
{	
	FILE *fp;
	
	fp = std_open_file(lpFile,"ab+"); 						
	if(fp == NULL)
	{
		return FILE_ERROR;  								
	}
	fseek(fp, 0, SEEK_END); 								
	if(fwrite(lpIn, sizeof(uint8_t), nLen, fp) != nLen) 	
	{
		fclose(fp);										
		return FILE_ERROR;							
	}
	else
	{
		fflush(fp); 									
		fclose(fp);										
	}	
	return FILE_OK;										
}


uint8_t std_insert_file(uint8_t *lpFile, uint8_t *lpIn, uint32_t nStart, uint32_t nLen)
{
	FILE *fp;
	
	fp = std_open_file(lpFile, "rb+"); 						
	if(fp == NULL)
	{
		return FILE_ERROR;								
	}
	fseek(fp,0,SEEK_END);
	if(nStart > ftell(fp))
	{
		fclose(fp);										
		return FILE_SEEK_ERROR;  
	}
	fseek(fp, nStart,SEEK_SET);
	if(fwrite(lpIn, sizeof(uint8_t), nLen, fp) != nLen) 	
	{
		fclose(fp);									
		return FILE_ERROR;							
	}
	else
	{
		fflush(fp); 										
		fclose(fp);										
	}	
	return FILE_OK;											
}


uint8_t std_save_overlap(uint8_t *lpFile,uint8_t *lpIn,uint32_t nLen)
{
	uint8_t *pData,*pBak,ret;
	uint16_t crc;
	
	pData= (uint8_t *)malloc(nLen+2);
	pBak = (uint8_t *)malloc(strlen(lpFile)+5);
	memcpy(pData,lpIn,nLen);
	crc = crc16_calc(lpIn,nLen);	
	pData[nLen] = (crc & 0xFF00) >> 8;	
	pData[nLen+1] = crc & 0x00FF;	
	
	memset(pBak,0,strlen(lpFile)+5);
	strcpy(pBak,lpFile);
	strcat(pBak,".bak");
	ret = std_write_file(pBak,pData,nLen+2);		
	if(ret == FILE_OK)
	{
		ret = std_write_file(lpFile,pData,nLen+2);		
	}	
	free(pData);
	free(pBak);
   	return ret;
}


uint8_t std_read_overlap(uint8_t *lpFile,uint8_t *lpOut,uint32_t nLen)
{
   	uint8_t *pData,*pBak,ret;
  	uint16_t crc,m;
  	uint32_t le;										

	pBak = (uint8_t *)malloc(strlen(lpFile)+5);							
	memset(pBak,0,strlen(lpFile)+5);
	strcpy(pBak,lpFile);
	strcat(pBak,".bak");
	pData= (uint8_t *)malloc(nLen+2);											
	memset(pData,0,nLen+2);					

	le = nLen+2;								
	ret = std_read_file(lpFile,pData,0,&le);							
	switch(ret)
	{
		case FILE_OK:								
			crc = crc16_calc(pData,nLen);	
			m = pData[nLen];
			m <<= 8;
			m |= pData[nLen+1];
			if( crc == m)	
			{
				memcpy(lpOut,pData,nLen);
				break;
	    	}	
		case FILE_EOF:						
			le = nLen+2;
			ret = std_read_file(pBak,pData,0,&le);					
			switch( ret )
			{
				case FILE_OK:									
					crc = crc16_calc(pData,nLen);	
					m = pData[nLen];
					m <<= 8;
					m |= pData[nLen+1];
					if( crc == m)	
					{
						std_save_overlap(lpFile,pData,nLen);	
						memcpy(lpOut,pData,nLen);
		    			break;
			    	}	
				case FILE_EOF:					
					ret = FILE_EOF;
					break;

				default:
					ret = FILE_ERROR;
					break;
			}
			break;	
			
		default:
			ret = FILE_ERROR;
			break;
	}
	free(pBak);
	free(pData);	
   	return ret;										
}


uint32_t std_get_file_size(uint8_t * lpFile)
{
	FILE *fp;
	uint32_t i;

	fp = std_open_file(lpFile, "rb"); 						
	if(fp == NULL)
	{
		return 0;											
	}
	fseek(fp, 0, SEEK_END);
	i = ftell(fp);
	fclose(fp);
	return i;
}



uint32_t std_scan_dir(uint8_t *lpDir,uint8_t lpEntryItem[][128],uint8_t OpeMode)
{
	uint32_t Count = 0;
 	uint8_t Path[256];								
 	DIR *pDir = NULL;								
 	struct dirent Entry;										
   	struct dirent *pEntry = NULL;					
	struct stat EntryInfo;    						
	
    pDir = opendir(lpDir);							
    if( pDir != NULL ) 
    {
		readdir_r(pDir, &Entry, &pEntry);		
	    while(pEntry != NULL && Count < 256)		 
	    {
	    	memset(Path,0,sizeof(Path));			
	    	strcpy(Path,lpDir);				
	    	strcat(Path,"/");						
	    	strcat(Path,Entry.d_name);				
	        if(	(strcmp(Entry.d_name,"..") != 0) && 
	        	(strcmp(Entry.d_name,".") != 0) &&
	        	(strcmp(Entry.d_name,"lost+found") != 0))
	        {
	        	lstat(Path, &EntryInfo );			
	        	switch(OpeMode)						
	        	{
	        		case SDMODE_FILE:
	        			if(S_ISREG(EntryInfo.st_mode))
						{
							if(strlen(Entry.d_name) < 32)
							{
								strcpy(lpEntryItem[Count],Entry.d_name);
       		    				Count++;
       		    			}	
						}
	        			break;
	        		case SDMODE_DIR:
	        			if(S_ISDIR(EntryInfo.st_mode))
						{
							if(strlen(Entry.d_name) < 32)
							{
								strcpy(lpEntryItem[Count],Entry.d_name);
       		    				Count++;
       		    			}
						}
	        			break;
	        		case SDMODE_EXE:
		       			if(S_ISREG(EntryInfo.st_mode))
						{
							if(access(Path, X_OK) == 0)		
							{
								if(strlen(Entry.d_name) < 32)
								{
									strcpy(lpEntryItem[Count],Entry.d_name);
       		    					Count++;
       		    				}
       		    			}		
						}
	        			break;		
	        		case SDMODE_ALL:
	        		default:	
	        			if(S_ISREG(EntryInfo.st_mode) || S_ISDIR(EntryInfo.st_mode))
						{
							if(strlen(Entry.d_name) < 32)
							{
								strcpy(lpEntryItem[Count],Entry.d_name);
       		    				Count++;
       		    			}	
						}
	        			break;
	        	}
	        }	
	    	readdir_r(pDir, &Entry, &pEntry);								
	    }
	    closedir(pDir);														
	}
    return Count;
}

void msleep(uint32_t ms)
{
	struct timeval tv;   
	
	memset((uint8_t*)&tv,0,sizeof(struct timeval));
	tv.tv_sec = ms / 1000;   
    tv.tv_usec = (ms % 1000) * 1000;  
    select(0, NULL, NULL, NULL, &tv);
}

void std_del_file(uint8_t *lpFile)
{
	delete_file(lpFile);	
	sync();					
}