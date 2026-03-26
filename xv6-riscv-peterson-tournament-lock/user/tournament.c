#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_PROCESSES 64

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: tournament <num_processes>\n");
        exit(1);
    }

    int num_processes = atoi(argv[1]);

    if (num_processes < 1 || num_processes > MAX_PROCESSES || (num_processes & (num_processes - 1)) != 0) {
        printf("Number of processes must be a power of 2 and <= 16\n");
        exit(1);
    }

    int proc_id = tournament_create(num_processes);
    if (proc_id < 0) {
        printf("Error creating tournament tree\n");
        exit(1);
    }

    if (proc_id < num_processes) {
        // Child process logic
        if (tournament_acquire() < 0) {
            printf("[PID %d] Error acquiring lock\n", getpid());
            exit(1);
        }

        printf("[PID %d] Acquired lock (ID assigned for the process %d)\n", getpid(), proc_id);
        sleep(1); // Simulate critical section
        printf("[PID %d] Releasing lock (ID assigned for the process %d)\n", getpid(), proc_id);

        if (tournament_release() < 0) {
            printf("[PID %d] Error releasing lock\n", getpid());
            exit(1);
        }

        exit(0);
    } else {
        // Parent process waits for all children
        for (int i = 0; i < num_processes; i++) {
            wait(0);
        }
    }
    // optional : destroying all the locks , making it possible to excute tournament multiple times;

    for(int i = 0 ; i < 63 ; i++){
        peterson_destroy(i);
    }
    exit(0);
}

