
#include "kernel/types.h"
#include "user/user.h"

//int map_shared_pages(int dst_pid, void *src_va, int size);
//int unmap_shared_pages(int dst_pid, void *dst_va, int size);

#define PGSIZE 4096

int main(int argc, char *argv[]) {
    int skip_unmap = 1;
    if (argc > 1 && strcmp(argv[1], "-u") == 0)
        skip_unmap = 0;
        
    uint64 parent_before_mapping = -1;
    uint64 parent_after_mapping = -1;
    uint64 parent_after_unmapping = -1;

    uint64 child_before_mapping = -1;
    uint64 child_after_mapping = -1;
    uint64 child_after_unmap = -1;
    uint64 child_after_malloc = -1;

    char *shared = malloc(PGSIZE);

    int parent_pid = getpid();
    int pid = fork();

    if (pid < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // ----------------- CHILD --------------------
        sleep(1); // give parent time to set up
        child_before_mapping = (uint64) sbrk(0);
        printf("[Child] sz before mapping: %d\n", child_before_mapping);
        
        void* child_va = map_shared_pages(parent_pid, getpid(),  shared, PGSIZE);

        child_after_mapping = (uint64) sbrk(0);
        printf("[Child] sz after mapping: %d\n", child_after_mapping);

        strcpy((char *)child_va, "Hello daddy");
        printf("[Child] wrote to shared mem: \"%s\"\n", (char *)child_va);

        printf("[Child] read from shared before unmap: \"%s\"\n", (char *)child_va);

        if (!skip_unmap) {
            
            if (unmap_shared_pages((void*) child_va, PGSIZE) < 0)
                printf("[Child] unmap failed\n");
            else
                printf("[Child] unmapped shared memory\n");

            child_after_unmap = (uint64) sbrk(0);
            printf("[Child] sz after unmap: %d\n", child_after_unmap);

            char *p = malloc(PGSIZE + 90000);
            *p = 'X';
            child_after_malloc = (uint64) sbrk(0);
            printf("[Child] sz after malloc: %d\n", child_after_malloc);

        } else {
            printf("[Child] skipping unmap, exiting with mapped pages\n");
        }

        exit(0);
    } else {
        // ----------------- PARENT --------------------
        
        parent_before_mapping = (uint64) sbrk(0);
        printf("[Parent] sz before map: %d\n", parent_before_mapping);

        wait(0); // wait for child
        parent_after_mapping = (uint64) sbrk(0);
        printf("[Parent] sz after map: %d\n", parent_after_mapping);

        printf("[Parent] read from shared: \"%s\"\n", shared);

        // Unmap shared pages from child (if child didn't)
        if (!skip_unmap) {
            if (unmap_shared_pages((void*) shared, PGSIZE) < 0)
                printf("[Parent] unmap failed\n");
            else
                printf("[Parent] unmapped from child (cleanup)\n");

            parent_after_unmapping = (uint64) sbrk(0);
            printf("[Parent] sz after unmap: %d\n", parent_after_unmapping);
        } else {
            printf("[Parent] skipping unmap to test cleanup on exit\n");
        }

        printf("\nTest complete. Run again to verify no leaks.\n");
    }

    exit(0);
}

