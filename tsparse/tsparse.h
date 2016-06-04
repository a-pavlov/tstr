
typedef enum {
    BEGIN = 0,
    MESSAGE = 1,
    SOURCE = 2,
    TRANSLATION = 3,
    FINISH = 4
} state_t;

typedef struct {
    state_t state;
    char source[1024];
    char force;
    char translation_block;	// c - completed, u - unfinished
    void (*pass)(const char* b, int len);
    const char* (*translate)(const char* src);

} message_t;

void init_message(message_t* msg);
void process_line(message_t* msg, const char* line);
