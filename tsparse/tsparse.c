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

const char* TT = "Test translation";


void print_callback(const char* b, int len) {
	char buff[1024];
	memset(buff, 0, 1024);
	memcpy(buff, b, len);
	printf("%s", buff);
}

const char* translator(const char* src) {
	return TT;
}

void init_message(message_t* msg) {
    msg->state = BEGIN;
    msg->pass = &print_callback;
    msg->translate = &translator;
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

			msg->pass(p, end - p);
			p = end;
		}
		else if (msg->state == MESSAGE) {
			const char* psrc = end_of_token(p, BSRC);
			const char* ptrn = translation_block(msg, p);
			const char* pmsg;

			if (psrc != NULL && (ptrn == NULL || ptrn > psrc)) {
				msg->state = SOURCE;
				msg->pass(p, psrc - p);
				p = psrc;
			}
			else if (ptrn != NULL && (psrc == NULL || psrc > ptrn)) {
				msg->state = TRANSLATION;
				msg->pass(p, ptrn - p);
				p = ptrn;
			}
			else if ((pmsg = strstr(p, EMSG)) != NULL) {
				msg->state = BEGIN;
				msg->pass(p, pmsg - p);
				// translate source if translation block is not completed
				if (msg->translation_block != 'c') {
					msg->pass("<translation>", strlen("<translation>"));
					const char* tran = msg->translate(msg->source);
					msg->pass(tran, strlen(tran));
					msg->pass("</translation>\n", strlen("</translation>\n"));
				}
				msg->pass(pmsg, strlen(EMSG));
				p = pmsg + strlen(EMSG);
				init_message(msg);
			} else {
				msg->pass(p, strlen(p));
				p += strlen(p);
			}
		}
		else if (msg->state == SOURCE) {
			const char* end = strstr(p, ESRC);
			if (end != NULL) {
				strncat(msg->source, p, end - p);
				msg->pass(p, end - p);
				msg->pass(end,  strlen(ESRC));
				msg->state = MESSAGE;
				end += strlen(ESRC);
			}
			else {
				end = p + strlen(p);
				strncat(msg->source, p, strlen(p));
				msg->pass(p, strlen(p));
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
			if (*end == '\n') end++;
			p = end;
		}
	}
}
