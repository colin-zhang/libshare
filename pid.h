#ifndef _LIB_SHARE_PID_H_
#define _LIB_SHARE_PID_H_

extern char *ps_name(int pid, char *buffer, int maxlen);
extern int pidof(const char *name);
extern int killall(const char *name, int sig);
extern int ppid(int pid);

#endif