#include <stdio.h>
#include <stdlib.h>
#include "tsparse.h"
#include <curl/curl.h>

int main(void) {
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_cleanup(curl);
	}

	message_t msg;
	init_message(&msg);

	ssize_t read;
	char* line = NULL;
	size_t n = 0;
	while((read = getline(&line, &n, stdin)) != -1) {
		process_line(&msg, line);

		if (msg.state == FINISH) {
			init_message(&msg);
		}
	}
}
