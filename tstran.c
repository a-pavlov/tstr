#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tsparse.h"
#include "jsmn.h"
#include "argparse.h"
#include <curl/curl.h>

CURL *curl;

static char response[2048];
static char errors[CURL_ERROR_SIZE];
static long http_code;
const char* translation = NULL;
const char* api_key = NULL;
const char* default_api_key = "trnsl.1.1.20160605T163909Z.a355c2ff3ca0e595.88ea3c4368c470b7c90031700cda2b9dea944b88";

size_t write_callback(void *data, size_t size, size_t nmemb, void *userdata) {
  char* r = (char*)userdata;
  strncat(r, (char*)data, size*nmemb);
  return (size * nmemb);
}

const char* online_translation(const char* source) {
	response[0] = 0;
	char url[1024];
    url[0] = 0;
    // TODO(a-pavlov) check url buffer size!
    strcat(url, "https://translate.yandex.net/api/v1.5/tr.json/translate?key=");
    if (api_key != NULL) {
        strcat(url, api_key);
    }
    else {
        strcat(url, default_api_key);
    }

    assert(translation != NULL);
    strcat(url, "&lang=");
    strcat(url, translation);
    strcat(url, "&text=");
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
    // TODO - add better result processing
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

static const char *const usage[] = {
    "tstr [options] [[--] args]",
    "tstr [options]",
    NULL,
};

int main(int argc, const char **argv) {
    int force = 0;

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Basic options"),
        OPT_BOOLEAN('f', "force", &force, "re-translate completed translation blocks"),
        OPT_STRING('t', "translation", &translation, "translation direction src-dst, for example: en-ru"),
        OPT_STRING('k', "apikey", &api_key, "Yandex translation api key. If not set will use default key"),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\tstr translates Qt translation files *.ts using Yandex translation API.", "");
    argc = argparse_parse(&argparse, argc, argv);
    //if (force != 0) printf("force: %d\n", force);
    //if (translation != NULL) printf("path: %s\n", translation);
    //if (api_key != NULL) printf("api key: %s\n", api_key);

    if (translation == NULL) {
        printf("Please provide translation direction using -t option\n");
        return 1;
    }

	curl = curl_easy_init();

	if (curl) {
		const char* ot = online_translation("");
		message_t msg;
		init_message(&msg);
        if (force != 0) msg.force = 'y'; else msg.force = 'n';
		msg.translate = &online_translation;

		char line[1024];
		size_t n = 0;
		while((fgets(line, 1024, stdin))) {
			process_line(&msg, line);            
		}

		curl_easy_cleanup(curl);
	}
}
