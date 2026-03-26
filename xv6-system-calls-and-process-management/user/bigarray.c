#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 65536
#define CHILDREN 4

int main(void)
{
    int *array = malloc(sizeof(int) * N);
    if (!array) {
        printf("Error: memory allocation failed\n");
        exit(1, "");
    }

    // Initialize array 
    for (int i = 0; i < N; i++) {
        array[i] = i;
    }

    int pids[CHILDREN];
    int ret = forkn(CHILDREN, pids);
    if (ret < 0) {
        printf("Error: forkn failed\n");
        free(array);
        exit(1, "");
    }

    if (ret == 0) {
        // Parent logic
        printf("Parent: created children with PIDs: ");
        for (int i = 0; i < CHILDREN; i++) {
            printf("%d ", pids[i]);
        }
        printf("\n");

        int statuses[64]; // NPROC in kernel/param.h
        int finished = 0;
        if (waitall(&finished, statuses) < 0) {
            printf("Error: waitall failed\n");
            free(array);
            exit(1, "");
        }

        if (finished != CHILDREN) {
            printf("Error: expected %d children, but got %d\n", CHILDREN, finished);
            free(array);
            exit(1, "");
        }

        int total = 0;
        for (int i = 0; i < finished; i++) {
            total += statuses[i];
        }

        printf("Parent: Final sum = %d\n", total);
        free(array);
        exit(0, "");
    } else {
        // Child logic

        int child_num = ret; // 1 to CHILDREN

        int base_size = N / CHILDREN;
        int remainder = N % CHILDREN;

        // Calculate start and end index carefully
        int start = (child_num - 1) * base_size + (child_num - 1 < remainder ? child_num - 1 : remainder);
        int end = start + base_size + (child_num <= remainder ? 1 : 0);

        int sum = 0;
        for (int i = start; i < end; i++) {
            sum += array[i];
        }

        sleep(child_num);
        printf("Child %d (PID %d): partial sum = %d\n", child_num, getpid(), sum);

        free(array);
        exit(sum, "");
    }
}
