#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include "tpool.h"

char default_root[] = ".";

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
	int thread_pool = 5, buffer_size = 5;
	tpool_t *tm;
	char *sched_algo;

    
    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1)
	switch (c) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	case 't':
		thread_pool = atoi(optarg);
		printf("thread pool size: %d\n", thread_pool);
		tm = tpool_create(thread_pool);
		break;
	case 'b':
		buffer_size = atoi(optarg);
		printf("buffer size: %d\n", buffer_size);
		break;
	case 's':
		sched_algo = malloc(4);
		strncpy(sched_algo, optarg, 4);
		
		break;
	default:
	    fprintf(stdout, "usage: wserver [-d basedir] [-p port]\n");
	    exit(1);
	}

    // run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		if (!tpool_add_work(tm, request_handle, &conn_fd))
			printf("could not add work\n");

    }
    return 0;
}


    


 
