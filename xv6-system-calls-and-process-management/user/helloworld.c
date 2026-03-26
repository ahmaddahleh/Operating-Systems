#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // used write instead of printf because it is lowlevel func that doesn't use global vars(so is more safe!)  
  char printToScreen[] = "Hello World xv6\n";
  write(1, printToScreen, strlen(printToScreen));
    
  exit(0,"");
}