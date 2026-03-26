#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_PROCESSES 16 
#define MAX_LEVELS 4 // log2(16) = 4

// Dynamically allocated arrays
static int* tournament_locks = 0;           // (n - 1) locks
static int** roles = 0;                     // roles[process_id][level]
static int num_levels = 0;
static int num_processes = 0;
int my_index = -1; 

// Helper: check if number is a power of 2
int is_power_of_two(int n) {
    return n > 0 && n != 1 && (n & (n - 1)) == 0 ;
}

int tournament_create(int processes) {
    if (!is_power_of_two(processes) || processes > MAX_PROCESSES) {
        return -1;
    }

    num_processes = processes;

    // Compute number of levels: log2(processes)
    num_levels = 0;
    for (int x = processes; x > 1; x >>= 1) {
        num_levels++;
    }

    // Allocate lock array (internal nodes = processes - 1) , dynamic alocation
    tournament_locks = malloc(sizeof(int) * (processes - 1));
    if (tournament_locks == 0) return -1;

    for (int i = 0; i < processes - 1; i++) {
        tournament_locks[i] = peterson_create();
        if (tournament_locks[i] < 0) {
            return -1;
        }
    }

    // Allocate roles array , also dynamically
    roles = malloc(sizeof(int*) * processes);
    if (roles == 0) return -1;
    for (int i = 0; i < processes; i++) {
        roles[i] = malloc(sizeof(int) * num_levels);
        if (roles[i] == 0) return -1;
    }

    // Fork N child processes and assign each an index (0 to N-1)
    for (int i = 0; i < processes; i++) {
        int pid = fork();
        if (pid < 0) {
            printf("Fork failed\n");
            return -1;
        }   
        if (pid == 0) {
            // Child process
            my_index = i;

            // Assigning roles for each level
            for (int l = 0; l < num_levels; l++) {
                // Role = 0 (left) or 1 (right)
                roles[my_index][l] = ((my_index  & (1 << (num_levels - l - 1))) >> (num_levels - l - 1));
            }

            return my_index;  // Child continues from here
        }
        // Parent continues to fork next child
    }

    return processes; // Parent returns
}

// each process tries to acquire the locks from the button to the root ;
int tournament_acquire(void) {
    if (my_index < 0) return -1;

    for (int level = num_levels - 1; level >= 0; level--) {
        //int lock_index = my_index >> (num_levels - level);
        int lock_index = (1 << level) - 1 + (my_index >> (num_levels - level));
        int role = roles[my_index][level];

        if (peterson_acquire(tournament_locks[lock_index], role) < 0) {
            return -1;
        }
    }
    return 0;
}

// releasing the locks in reverse order , from the root to the buttom ;
int tournament_release(void) {
    if (my_index < 0) return -1;

    for (int level = 0 ; level < num_levels ; level++) {
        //int lock_index = my_index >> (num_levels - level);
        int lock_index = (1 << level) - 1 + (my_index >> (num_levels - level));
        int role = roles[my_index][level];

        if (peterson_release(tournament_locks[lock_index], role) < 0) {
            return -1;
        }
    }
    return 0;
}
