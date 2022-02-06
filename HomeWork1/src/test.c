#include <unistd.h>
#include <stdio.h>
#include <limits.h>


int main() {
   const int i = 256;
   char cwd[256];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
   } else {
       perror("getcwd() error");
       return 1;
   }
   return 0;
}