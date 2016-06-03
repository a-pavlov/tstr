#include <stdio.h>
#include <stdlib.h>
#include "tsparse.h"

int main(void) {
   message_t msg;
   init_message(&msg);

   ssize_t read;
   char* line = NULL;
   size_t n = 0;
   while((read = getline(&line, &n, stdin)) != -1) {
       printf("line: %s", process_line(&msg, line));

       if (msg.state == FINISH) {
           init_message(&msg);
       }
   }
}
