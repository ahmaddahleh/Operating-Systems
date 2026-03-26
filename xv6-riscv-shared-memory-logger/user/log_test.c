#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

#define BUFSIZE 4096

#define MAKE_HEADER(idx, len) (((idx & 0xFFFF) << 16) | (len & 0xFFFF))
#define GET_INDEX(h) ((h >> 16) & 0xFFFF)
#define GET_LENGTH(h) (h & 0xFFFF)

int child_main(int index, int parentId, void *shared_buf)
{
    void *va = map_shared_pages(parentId, getpid(), shared_buf, BUFSIZE);
    if (!va)
    {
        printf("Failed to map shared memory in child %d\n", index);
        exit(1);
    }

    char *msg ;
    if (index == 0)
        msg = malloc(BUFSIZE+1);
    else
        msg = "Hello daddy!";
    uint32 msg_len = strlen(msg);
    uint8 *start = (uint8 *)va;
    uint8 *end = start + BUFSIZE;
    uint8 *ptr = start;

    while (ptr + 4 + msg_len <= end)
    {
        uint32 *header_ptr = (uint32 *)ptr;

        if (__sync_val_compare_and_swap(header_ptr, 0, MAKE_HEADER(index, msg_len)) == 0)
        {
            // Successfully claimed a slot
            char *msg_ptr = (char *)(ptr + 4);
            *(msg_ptr + msg_len) = '\0'; // Null-terminate the message
            memcpy(msg_ptr, msg, msg_len);
            break;
        }

        // Move to next aligned slot
        ptr += 4 + msg_len;
        ptr = (uint8 *)(((uint64)ptr + 3) & ~3); // Align to 4-byte boundary
    }

    exit(0);
}

int main(int argc, char *argv[])
{
    int NCHILD = atoi(argv[1]); // Number of children to fork
    void *shared_buf = malloc(BUFSIZE); // allocated after fork
    memset(shared_buf, 0, BUFSIZE);
    int parent_pid = getpid();

    // Fork children first
    for (int i = 0; i < NCHILD; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            //sleep(10); // give parent time to map
            // Read shared_buf from known VA or pipe (simplified here)
            child_main(i, parent_pid, shared_buf); // assume shared_buf is passed/set
        }
    }

    // Parent scans messages
    sleep(10); // let children finish
    uint8 *base = (uint8 *)shared_buf;
    uint8 *end = base + BUFSIZE;
    uint8 *raw_ptr = base;

    while (raw_ptr + 4 <= end)
    {
        uint32 *ptr = (uint32 *)raw_ptr;
        uint32 header = *ptr;
        if (header != 0)
        {
            int idx = GET_INDEX(header);
            int len = GET_LENGTH(header);
            char *msg = (char *)(ptr + 1);
            printf("[child %d] %s\n", idx, msg);

            int padded_len = (len + 3) & ~3;
            raw_ptr = (uint8 *)(ptr + 1) + padded_len;
            raw_ptr = (uint8 *)(((uint64)raw_ptr + 3) & ~3);
        }
        else
        {
            raw_ptr = (uint8 *)(ptr + 1);
            raw_ptr = (uint8 *)(((uint64)raw_ptr + 3) & ~3);
        }
    }
    exit(0);
}
