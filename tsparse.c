#include "tsparse.h"
#include <string.h>
#include <stdlib.h>

void init_message(message_t* msg) {
    msg->state = BEGIN;
}

const char* process_line(message_t* msg, const char* line) {
    if (msg->state == BEGIN) {
        const char* p = strstr(line, "<message>");
        if (p != NULL) msg->state = MESSAGE;
        return line;
    }

    if (msg->state == MESSAGE) {
        const char* s = strstr(line, "<source>");
        //if (s != NULL)

        const char* p = strstr(line, "</message>");
        // prepare translation here
        if (p != NULL) msg->state = BEGIN;
        return line;
    }

    return line;
}
