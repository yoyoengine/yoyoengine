#include <time.h>
#include <stdio.h>

#include <yoyoengine/yoyoengine.h>
#include <Nuklear/nuklear.h>

#ifdef _WIN32
#include <windows.h>
#endif

// ANSI escape codes for color.
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

FILE *logFile = NULL;
int linesWritten = 0;

enum logLevel logThreshold = 5; // by defualt log NOTHING to console

#ifdef _WIN32
void enableVirtualTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;

    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

const char* getTimestamp() {
    static char datetime_str[20];
    time_t now = time(NULL);
    struct tm local_time = *localtime(&now);

    strftime(datetime_str, sizeof(datetime_str), "%Y-%m-%d %H:%M:%S", &local_time);

    return datetime_str;
}

void openLog(){
    // open the log file
    char *logpath = ye_get_engine_resource_static("debug.log");
    logFile = fopen(logpath, "w");
    if (logFile == NULL) {
        printf("%sError opening logfile at: %s\n",logpath,RED);
    }
}

void closeLog(){
    fclose(logFile);
}

void logMessage(enum logLevel level, const char *text){
    openLog();
    switch (level) {
        case debug:
            fprintf(logFile, "[%s] [DEBUG]: %s", getTimestamp(), text);
            if(logThreshold <= level){
                printf("%s[%s] [%sDEBUG%s]: %s", RESET, getTimestamp(), MAGENTA, RESET, text);
            }
            break;
        case info:
            fprintf(logFile, "[%s] [INFO]:  %s", getTimestamp(), text);
            if(logThreshold <= level){
                printf("%s[%s] [%sINFO%s]:  %s", RESET, getTimestamp(), GREEN, RESET, text);
            }
            break;
        case warning:
            fprintf(logFile, "[%s] [WARNING]: %s", getTimestamp(), text);
            if(logThreshold <= level){
                printf("%s[%s] [%sWARNING%s]: %s", YELLOW, getTimestamp(), YELLOW, RESET, text);
            }
            break;
        case error:
            fprintf(logFile, "[%s] [ERROR]: %s", getTimestamp(), text);
            if(logThreshold <= level){
                printf("%s[%s] [%sERROR%s]: %s", RED, getTimestamp(), RED, RESET, text);
            }
            break;
        default:
            fprintf(logFile, "[%s] [ERROR]: %s", getTimestamp(), "Invalid log level\n");
            printf("%s[%s] [%sERROR%s]: %s", RED, getTimestamp(), RED, RESET, "Invalid log level\n");
            break;
    }
    linesWritten++;

    // Add to the log buffer
    addToLogBuffer(level, text);

    closeLog();
}


// buffer and console ui related ops

#define LOG_BUFFER_SIZE 100

struct LogMessage {
    char timestamp[20];
    enum logLevel level;
    char text[256]; // Adjust the size as needed
};

struct LogMessage logBuffer[LOG_BUFFER_SIZE];
int logBufferIndex = 0;

/*
    shifts old messages out of buffer
*/
void addToLogBuffer(enum logLevel level, const char *text) {
    // Initialize a new message
    struct LogMessage message;
    snprintf(message.timestamp, sizeof(message.timestamp), "%s", getTimestamp());
    message.level = level;

    // Remove newline characters from the text
    size_t text_length = strnlen(text, sizeof(message.text));
    if (text_length > 0 && text[text_length - 1] == '\n') {
        text_length--; // Remove the trailing newline
    }
    snprintf(message.text, sizeof(message.text), "%.*s", (int)text_length, text);

    // If the buffer is full, shift messages to make space for the new one
    if (logBufferIndex == LOG_BUFFER_SIZE) {
        // Shift all messages except the first one
        for (int i = 1; i < LOG_BUFFER_SIZE; i++) {
            logBuffer[i - 1] = logBuffer[i];
        }
        logBufferIndex--; // Decrease the index to overwrite the last message
    }

    // Add the new message to the buffer
    logBuffer[logBufferIndex] = message;
    logBufferIndex++;
}

#define MAX_INPUT_LENGTH 100
char userInput[MAX_INPUT_LENGTH];

bool color_code = true;

// paint log panel
void paint_console(struct nk_context *ctx){
    // Create the GUI layout
    if (nk_begin(ctx, "Console", nk_rect(300, 200, 800, 633),
        NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
        
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_checkbox_label(ctx, "Color by log level", &color_code);

        // Calculate available space for log and input field
        float inputHeight = 35.0f; // Height of the input field
        float logHeight =  500.0f; // ctx->current->layout->row.height - inputHeight;

        // TODO: allow resizing again and auto calculate size of log panel
        // printf("height: %f\n",ctx->current->bounds.h);

        nk_layout_row_dynamic(ctx, logHeight, 1);
        nk_group_begin(ctx, "Log", NK_WINDOW_BORDER);

        for (int i = 0; i < logBufferIndex; i++) {
            nk_layout_row_dynamic(ctx, 15, 1);

            // Create a formatted log entry with timestamp and color
            char formattedLog[256];
            snprintf(formattedLog, sizeof(formattedLog), "<%s> %s", logBuffer[i].timestamp, logBuffer[i].text);

            // Display the formatted log entry
            if(color_code){
                switch(logBuffer[i].level){
                    case info:
                        nk_label_colored(ctx, formattedLog, NK_TEXT_LEFT, nk_rgb(0, 255, 0));  // Green text
                        break;
                    case debug:
                        nk_label_colored(ctx, formattedLog, NK_TEXT_LEFT, nk_rgb(150, 0, 255));  // Purple text
                        break;
                    case warning:
                        nk_label_colored(ctx, formattedLog, NK_TEXT_LEFT, nk_rgb(255, 255, 0));  // Yellow text
                        break;
                    case error:
                        nk_label_colored(ctx, formattedLog, NK_TEXT_LEFT, nk_rgb(255, 0, 0));  // Red text
                        break;
                    default:
                        break;
                }
            }
            else{
                nk_label_colored(ctx, formattedLog, NK_TEXT_LEFT, nk_rgb(255, 255, 255));  // white text
            }
        }

        nk_group_end(ctx);
        // Input command section
        static char userInput[MAX_INPUT_LENGTH];
        nk_layout_row_dynamic(ctx, inputHeight, 1);
        if (nk_input_is_key_pressed(&ctx->input, NK_KEY_ENTER)) {
            // Handle user input when Enter key is pressed
            if (strlen(userInput) > 0) {
                // Process the user's command here
                // For example, you can parse userInput and execute corresponding actions

                // create new string that says "executed command: <userInput>"
                char proc[256];
                snprintf(proc, sizeof(proc), "executed command: %s\n", userInput);
                // log the new string
                logMessage(info, proc);
                
                // Clear the input buffer
                memset(userInput, 0, sizeof(userInput));
            }
        }
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, userInput, sizeof(userInput), nk_filter_ascii);
    }
    nk_end(ctx);
}

// INIT AND SHUTDOWN ////////////////////////

void log_init(enum logLevel level){
    logThreshold = level;

    // windows specific tweak to enable ansi colors
    #ifdef _WIN32
    enableVirtualTerminal();
    #endif

    // open log file the first time in w mode to overwrite any existing log
    char *logpath = ye_get_engine_resource_static("debug.log");
    logFile = fopen(logpath, "w");
    if (logFile == NULL) {
        printf("%sError opening logfile at: %s\n",logpath,RED);
    }
    closeLog();
    logMessage(info, "Logging initialized\n");
    linesWritten=1; // reset our counter because not all outputs have actually been written to the log file yet
}

void log_shutdown(){
    logMessage(info, "Logging shutdown\n");
    linesWritten++;
}