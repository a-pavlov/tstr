
typedef enum {
    BEGIN = 0,
    MESSAGE = 1,
    SOURCE = 2,
    TRANSLATION = 3,
    FINISH = 4
} state_t;

typedef struct {
    state_t state;
    char data[8096];
    char force;
    char translation_block;	// c - completed, u - unfinished
    void (*data_callback)(const char* b, const char* e);
} message_t;

void init_message(message_t* msg);
void process_line(message_t* msg, const char* line);
