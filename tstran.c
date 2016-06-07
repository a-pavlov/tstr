#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tsparse.h"
#include "jsmn.h"
#include <curl/curl.h>

CURL *curl;

static char response[2048];
static char errors[CURL_ERROR_SIZE];
static long http_code;

size_t write_callback(void *data, size_t size, size_t nmemb, void *userdata) {
  char* r = (char*)userdata;
  strncat(r, (char*)data, size*nmemb);
  return (size * nmemb);
}

const char* online_translation(const char* source) {
	response[0] = 0;
	char url[1024];
	memset(response, 0, 2048);
	memset(url, 0, 1024);
	strcat(url, "https://translate.yandex.net/api/v1.5/tr.json/translate?key=trnsl.1.1.20160605T163909Z.a355c2ff3ca0e595.88ea3c4368c470b7c90031700cda2b9dea944b88&lang=en-ru&text=");
	strcat(url, source);
	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "tstr");
	curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    struct curl_slist *header = NULL;
    header = curl_slist_append(header, "Accept: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &errors);


    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(header);
    if (res != 0) return NULL;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    jsmn_parser parser;
	jsmntok_t tokens[10];
	jsmn_init(&parser);
	int tks = jsmn_parse(&parser, response, strlen(response), tokens, 10);
	if (tks == 8) {
		int length = tokens[7].end - tokens[7].start;
		int index = 0;
		while(index < length) {
			response[index] = response[tokens[7].start + index];
			++index;
		}
		response[index] = 0;
	}

	return response;
}

int main(void) {
	CURLcode res;
	curl = curl_easy_init();

	if (curl) {

		const char* ot = online_translation("");
		message_t msg;
		init_message(&msg);
		msg.translate = &online_translation;

		size_t read;
		char line[1024];
		size_t n = 0;
		while((fgets(line, 1024, stdin))) {
            strcat(line, "\n");
			process_line(&msg, line);            
		}

		curl_easy_cleanup(curl);
	}
}
