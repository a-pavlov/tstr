#include "tsparse.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BMSG "<message>"
#define BSRC "<source>"
#define BTRN "<translation type=\"unfinished\">"
#define EMSG "</message>"
#define ESRC "</source>"
#define ETRN "</translation>"

const char* TT = "<translation>Test translation</translation>\n";


void print_callback(const char* b, const char* e) {
	char buff[1024];
	memset(buff, 0, 1024);
	memcpy(buff, b, e - b);
	printf("%s", buff);
}

void init_message(message_t* msg) {
    msg->state = BEGIN;
    msg->data_callback = &print_callback;
    msg->force='y';
    msg->translation_block = 0;
}

const char* end_of_token(const char* src, const char* keyword) {
	const char* p = strstr(src, keyword);
	if (p != NULL) p += strlen(keyword);
	return p;
}

const char* translation_block(message_t* msg, const char* src) {
	const char* translation = strstr(src, "<translation>");
	const char* u_translation = strstr(src, BTRN);

	if (translation != NULL && msg->force != 'y') msg->translation_block = 'c';
	//if (u_translation) msg->translation_block = 'u';

	if (translation && msg->force == 'y') return translation;
	return u_translation;
}

void process_line(message_t* msg, const char* line) {
	const char* p = line;

	while(*p != 0) {
		if (msg->state == BEGIN) {
			const char* pmsg = end_of_token(p, BMSG);
			const char* end;

			if (pmsg != NULL) {
				msg->state = MESSAGE;
				end = pmsg;
			} else {
				end = p + strlen(p);
			}

			msg->data_callback(p, end);
			p = end;
		}
		else if (msg->state == MESSAGE) {
			const char* psrc = end_of_token(p, BSRC);
			const char* ptrn = translation_block(msg, p);
			const char* pmsg;

			if (psrc != NULL && (ptrn == NULL || ptrn > psrc)) {
				msg->state = SOURCE;
				msg->data_callback(p, psrc);
				p = psrc;
			}
			else if (ptrn != NULL && (psrc == NULL || psrc > ptrn)) {
				msg->state = TRANSLATION;
				msg->data_callback(p, ptrn);
				p = ptrn;
			}
			else if ((pmsg = strstr(p, EMSG)) != NULL) {
				msg->state = BEGIN;
				msg->data_callback(p, pmsg);
				// translate source if translation block is not completed
				if (msg->translation_block != 'c') msg->data_callback(TT, TT + strlen(TT));
				msg->data_callback(pmsg, pmsg + strlen(EMSG));
				p = pmsg + strlen(EMSG);
			} else {
				msg->data_callback(p, p + strlen(p));
				p += strlen(p);
			}
		}
		else if (msg->state == SOURCE) {
			const char* end = strstr(p, ESRC);
			if (end != NULL) {
				strncat(msg->data, p, end - p);
				msg->data_callback(p, end);
				msg->data_callback(end, end + strlen(ESRC));
				msg->state = MESSAGE;
				end += strlen(ESRC);
			}
			else {
				end = p + strlen(p);
				strncat(msg->data, p, strlen(p));
				msg->data_callback(p, p + strlen(p));
			}

			p = end;
			continue;
		}
		else if (msg->state == TRANSLATION) {
			const char* end = end_of_token(p, ETRN);

			if (end != NULL) {
				msg->state = MESSAGE;
			} else {
				end = p + strlen(p);
			}

			// skip trailing new line
			if (*p == '\n') p++;
			p = end;
		}
	}
}
