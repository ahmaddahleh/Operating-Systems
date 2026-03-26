#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // getting the size using syscall before allocating more memory
  int beforeAlloc = memsize();
  printf("Initial memory size: %d bytes\n", beforeAlloc);

  // the allocations of the new memory
  int size = 20 * 1024;
  char *arr = malloc(size);

  int afterAlloc = memsize();
  printf("Memory size after malloc: %d bytes\n", afterAlloc);

  //"freeing memory" -> which actually doesn't shrink the memory size back to the original as we can see
  free(arr);

  int afterReleasing = memsize();
  printf("Memory size after free: %d bytes\n", afterReleasing);

  exit(0,"");
}
