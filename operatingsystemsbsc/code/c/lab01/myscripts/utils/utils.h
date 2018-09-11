#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

/* Example data structure to be to be used in exercises */
#define STRLEN 256

struct test_pipe {
	int child_id;
	int process_id;
	float data[2];
	char msg[STRLEN];
};

void fill_test_pipe(struct test_pipe *t, int cid, int pid, float d0, float d1, char *msg);
void clear_test_pipe(struct test_pipe *t);
void zprintf(int fd, const char *fmt, ...);

#endif