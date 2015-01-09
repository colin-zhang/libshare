#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "ini.h"

void TrimL(unsigned char *str)
{
	unsigned char flag;
	int len,i,j;
	
	len = strlen(str);
	for(i = 0,j = 0,flag = FALSE; i < len; i++)
	{
		if(str[i] != ' ') flag = TRUE;
		if(flag) str[j++] = str[i];
	}
	str[j] = 0;
}


void TrimR(unsigned char *str)
{
	int len,i;

	len = strlen(str);
	for(i = len; i > 0; i--)
	{
		if(str[i-1] != ' ') 
		{
			break;
		}
	}
	str[i] = 0;
}

void Trim(unsigned char *str)
{
	TrimL(str);
	TrimR(str);
}


void IniTrimBracketL(unsigned char *str)
{
	unsigned char flag;
	int len,i,j;
	
	len = strlen(str);
	for(i = 0,j = 0,flag = FALSE; i < len; i++)
	{
		if((str[i] != ' ')&&(str[i] != '[')) flag = TRUE;
		if(flag) str[j++] = str[i];
	}
	str[j] = 0;
}

void IniTrimBracketR(unsigned char *str)
{
	int len,i;

	len = strlen(str);
	for(i = len; i > 0; i--)
	{
		if((str[i-1] != ' ') && (str[i-1] != ']'))
		{
			break;
		}
	}
	str[i] = 0;
}

void IniTrimBracket(unsigned char *str)
{
	unsigned char buf[1024];
    int len;
	
	memset(buf,0,sizeof(buf));
	len = strlen(str);
	memcpy(buf,str,len);
	
	IniTrimBracketL(buf);
	IniTrimBracketR(buf);
	memset(str,0,len);
	str[0] = '[';
	strcpy(str+1,buf);
	strcat(str,"]");
}

int IniWriteLine(FILE* stream,unsigned char *line)
{	
    unsigned char buf[1024];
    int len,i,j,flag;
	
	memset(buf,0,sizeof(buf));
	len = strlen(line);
	for(i = 0,j = 0,flag = 0; i < len; i++)
	{
		if((line[i] != 0x0A)&&(line[i] != 0x0D)) flag = 1;
		if(flag) buf[j++] = line[i];
	}
	if((line[0]!=0x0d)&&(line[0]!=0x0a)&&(len != 0))
	{
		fwrite(buf,strlen(buf),1,stream);
		fwrite("\x0D\x0A",2,1,stream);                    //写结束符
	}
    return 0;
}


int IniGetLine(FILE* stream, unsigned char *line, int maxlength)
{
    int size = 0,rslt = 0;
    unsigned char ch;
    memset(line, 0, maxlength);
	rslt = feof(stream);
    while( rslt == 0)
    {
        if( size < maxlength )
        {
            ch = fgetc(stream);
			rslt = feof(stream);
            if( ch == 0x0d || ch == 0x0a )
            {
                break;
            }
            else
            {
                line[size] = ch;
                size++;
            }
        }
    }
	if(rslt != 0) 
	{
		return -1;
	}
	else
	{
		return size;
	}
	
} 


unsigned char IniReadKey(const unsigned char *file, const unsigned char *section ,const unsigned char *key, unsigned char *value)
{
	FILE *fp;
	unsigned char *value_buf,rslt = 0;
    unsigned char line[1024];
    unsigned char file_buf[128];
    unsigned char section_buf[128],value_tmp[128];
    unsigned char key_buf[128],i,j;
    int flag = 0;

    
    memset(line,0,sizeof(line));
    memset(file_buf,0,sizeof(file_buf));
    memset(section_buf,0,sizeof(section_buf));
    memset(key_buf,0,sizeof(key_buf));
    
    memcpy(file_buf,file,strlen(file));
	Trim(file_buf);
    memcpy(section_buf,section,strlen(section));
	IniTrimBracket(section_buf);
    memcpy(key_buf,key,strlen(key));
	Trim(key_buf);
    fp = fopen(file_buf,"r");
    if(fp == NULL)
    {
		//fclose(fp);
		return 0;
    }
	else
	{
		while(IniGetLine(fp,line,1024)!= -1)
	    {
	    	 if(strstr(line,section_buf) !=  NULL)
	    	 {
	    	 	 flag = 1;	                                  //找到字段
	    	 	 continue; 
	    	 }
	    	 if((strchr(line,'[') !=  NULL)&&(flag == 1))
	    	 {
	    	 	break;
	    	 }
	    	 if((strstr(line,key_buf) !=  NULL)&&(flag == 1))
	    	 {
	    	 	 value_buf = strchr(line,'=');                //找到key
	    	 	 if(value_buf !=  NULL)
	    	 	 {	
	    	 	 	memset(value_tmp,0,sizeof(value_tmp));
	    	 	 	strcpy(value_tmp,value_buf+1);             
	    	 	 	rslt = 1;
	    	 	 	break;
	    	 	 }
	    	 }
	    }
		if(rslt == 1)
		{
			for(i = 0,j =0;i<strlen(value_tmp);i++)
			{
				if(!(isspace(value_tmp[i])))
				{
					if(value_tmp[i] ==';'){
						break;
					}
					value[j] =  value_tmp[i];
					j++;
				}
			}
			value[j] = 0;
		}
	    fclose(fp);
	}
   	return rslt;
}


unsigned char IniWriteKey(const unsigned char *file, const unsigned char *section ,const unsigned char *key, const unsigned char *value)
{
	FILE *fp,*fp_bak;
    unsigned char line[1024],rslt = 0;
    unsigned char file_buf[128],file_bak[128];
    unsigned char section_buf[128];
    unsigned char key_buf[128];
    unsigned char value_buf[128];
    int flag = 0,flag_bak = 0;

    
    memset(line,0,sizeof(line));
	memset(file_bak,0,sizeof(file_bak));
    memset(file_buf,0,sizeof(file_buf));
    memset(section_buf,0,sizeof(section_buf));
    memset(key_buf,0,sizeof(key_buf));
    memset(value_buf,0,sizeof(value_buf));
    
    memcpy(file_buf,file,strlen(file));
	Trim(file_buf);
    memcpy(section_buf,section,strlen(section));
	IniTrimBracket(section_buf);
    memcpy(key_buf,key,strlen(key));
	Trim(key_buf);
    memcpy(value_buf,value,strlen(value));
	Trim(value_buf);

    fp = fopen(file_buf,"r+");
    if(fp == NULL)
    {
		//fclose(fp);
		return 0;
    }
    else
    {
	    strcpy(file_bak,file_buf);
		strcat(file_bak,".bak");
	    fp_bak = fopen( file_bak, "w" ); 
	    while(IniGetLine(fp,line,1024) != -1)
	    {
	    	 if(strstr(line,section_buf) !=  NULL)
	    	 {
	    	 	flag = 1;	
				IniWriteLine(fp_bak,line);
	    	 	continue; 
	    	 }
	    	 if((strchr(line,'[') !=  NULL)&&(flag == 1)&&(flag_bak == 0))
	    	 {
				strcat(key_buf,"=");
				strcat(key_buf,value_buf);
				IniWriteLine(fp_bak,key_buf);
				flag_bak = 1;
				rslt = 1;
	    	 }
	    	 if((strstr(line,key_buf) !=  NULL)&&(flag == 1)&&(flag_bak == 0))
	    	 {
	    	 	 strcat(key_buf,"=");
				 strcat(key_buf,value_buf);
				 IniWriteLine(fp_bak,key_buf);
				 rslt = 1;
				 flag_bak = 1;
	    	 	 continue;
	    	 } 
			 IniWriteLine(fp_bak,line);
	    }
		if(rslt != 1)
		{
			if(flag == 0)                            //没有找到字段名
			{
				IniWriteLine(fp_bak,section_buf);
				strcat(key_buf,"=");
				strcat(key_buf,value_buf);
				IniWriteLine(fp_bak,key_buf);
				rslt = 1;
			}
			else
			{
				strcat(key_buf,"=");
				strcat(key_buf,value_buf);
				IniWriteLine(fp_bak,key_buf);
				rslt = 1;
			}
		}
		if(rslt == 1)
		{
			memset(line,0,sizeof(line));
			strcpy(line,"mv -f ");
			strcat(line,file_bak);
			strcat(line," ");
			strcat(line,file_buf);
			system(line);
		}
	    fclose(fp);
		fclose(fp_bak);
    }
   	return rslt;
}

