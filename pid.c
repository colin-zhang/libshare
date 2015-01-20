#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>

int f_read(const char *path, void *buffer, int max)
{
	int f;
	int n;

	if ((f = open(path, O_RDONLY)) < 0) return -1;
	n = read(f, buffer, max);
	close(f);
	return n;
}

int read_string(const char *path, char *buffer, int max)
{
	if (max <= 0) return -1;
	int n = f_read(path, buffer, max - 1);
	buffer[(n > 0) ? n : 0] = 0;
	return n;
}


char *ps_name(int pid, char *buffer, int maxlen)
{
	char buf[512];
	char path[64];
	char *p;

	if (maxlen <= 0) return NULL;
	*buffer = 0;
	sprintf(path, "/proc/%d/stat", pid);
	if ((read_string(path, buf, sizeof(buf)) > 4) && ((p = strrchr(buf, ')')) != NULL)) {
		*p = 0;
		if (((p = strchr(buf, '(')) != NULL) && (atoi(buf) == pid)) {
			strncpy(buffer, p + 1, maxlen);
		}
	}
	return buffer;
}

static int _pidof(const char *name, pid_t **pids)
{
	const char *p;
	char *e;
	DIR *dir;
	struct dirent *de;
	pid_t i;
	int count;
	char buf[256];

	count = 0;
	if (pids != NULL)
		*pids = NULL;
	if ((p = strrchr(name, '/')) != NULL) name = p + 1;
	if ((dir = opendir("/proc")) != NULL) {
		while ((de = readdir(dir)) != NULL) {
			i = strtol(de->d_name, &e, 10);
			if (*e != 0) continue;
			if (strcmp(name, ps_name(i, buf, sizeof(buf))) == 0) {
				if (pids == NULL) {
					count = i;
					break;
				}
				if ((*pids = realloc(*pids, sizeof(pid_t) * (count + 1))) == NULL) {
					return -1;
				}
				(*pids)[count++] = i;
			}
		}
	}
	closedir(dir);
	return count;
}

int pidof(const char *name)
{
	pid_t p;

	p = _pidof(name, NULL);
	if (p < 1) {
		usleep(10 * 1000);
		p = _pidof(name, NULL);
		if (p < 1)
			p = _pidof(name, NULL);
	}
	if (p < 1)
		return -1;
	return p;
}

int ppid(int pid) {
	char buf[512];
	char path[64];
	int ppid = 0;

	buf[0] = 0;
	sprintf(path, "/proc/%d/stat", pid);
	if ((read_string(path, buf, sizeof(buf)) > 4))
		sscanf(buf, "%*d %*s %*c %d", &ppid);

	return ppid;
}


int killall(const char *name, int sig)
{
	pid_t *pids;
	int i;
	int r;

	if ((i = _pidof(name, &pids)) > 0) {
		r = 0;
		do {
			r |= kill(pids[--i], sig);
		} while (i > 0);
		free(pids);
		return r;
	}
	return -2;
}

