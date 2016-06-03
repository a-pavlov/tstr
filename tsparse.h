
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
} message_t;

void init_message(message_t* msg);
const char* process_line(message_t* msg, const char* line);
