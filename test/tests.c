#include "test.h"
#include "tsparse.h"

static int parse() {
	return 1;
}

char out_buffer[1024];

const char* test_translator(const char* src) {
	return "test translation";
}

void test_output(const char* src, int len) {
	strncat(out_buffer, src, len);
}

void test_init_msg(message_t* msg, char force) {
	msg->state = BEGIN;
	msg->pass = &test_output;
	msg->translate = &test_translator;
	msg->force=force;
	msg->translation_block = 0;
}

int test_initial() {
	check(parse());
	return 0;
}

int main(void) {
	test(test_initial, "initial test");
	return (test_failed > 0);
}
