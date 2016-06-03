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

    const char* data_ptr = line;

    if (msg->state == MESSAGE) {
        const char* ss = strstr(line, "<source>");

        if (ss != NULL) {
            data_ptr = ss + strlen("<source>");
        }

        const char* p = strstr(line, "</message>");
        // prepare translation here
        if (p != NULL) msg->state = BEGIN;
        return line;
    }

    if (msg->state == SOURCE) {
        const char* es = strstr(line, "</source>");
        if (es != NULL) {
            // strcat remains
        }

        msg->state = MESSAGE;
    }

    return line;
}
