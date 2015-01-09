#ifndef _C_INI__H
#define _C_INI__H

#define FALSE 0
#define TRUE  1


unsigned char IniReadKey(const unsigned char *file, const unsigned char *section ,const unsigned char *key, unsigned char *value);
unsigned char IniWriteKey(const unsigned char *file, const unsigned char *section ,const unsigned char *key, const unsigned char *value);


#endif