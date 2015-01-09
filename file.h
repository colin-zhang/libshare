#ifndef _DCC_PROXY_FILE_H
#define _DCC_PROXY_FILE_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#define	FILE_OK 			0
#define FILE_EOF			1
#define FILE_SEEK_ERROR  	2
#define FILE_ERROR 			3
#define FILE_CRCERR			4

#define SDMODE_FILE			0
#define SDMODE_DIR			1
#define SDMODE_EXE			2
#define SDMODE_ALL			3


char df(char *lpDisk,unsigned int *lpAvail,unsigned int *lpUsed,unsigned int *lpUsedRate);
int get_file_size(const char *path);
bool is_fifo(const char *file);
bool is_dir(const char *path);
int file_exist(char *file_path);
void delete_file(const char *path);
int make_dir(const char* dir, mode_t mode);

char *file_to_alloc_mem(const char *path,int *file_len_out);
int copy_file(const char *src_file,const char *des_file);


uint8_t std_write_file(uint8_t *lpFile,uint8_t *data,uint32_t len);
uint8_t std_read_file(uint8_t *lpFile, uint8_t *lpOut, uint32_t nStart, uint32_t *lpLen);
uint8_t std_append_file(uint8_t *lpFile, uint8_t *lpIn, uint32_t nLen);
uint8_t std_insert_file(uint8_t *lpFile, uint8_t *lpIn, uint32_t nStart, uint32_t nLen);
uint8_t std_save_overlap(uint8_t *lpFile,uint8_t *lpIn,uint32_t nLen);
uint8_t std_read_overlap(uint8_t *lpFile,uint8_t *lpOut,uint32_t nLen);
uint32_t std_get_file_size(uint8_t * lpFile);
uint32_t std_scan_dir(uint8_t *lpDir,uint8_t lpEntryItem[][128],uint8_t OpeMode);
void std_del_file(uint8_t *lpFile);

#endif

