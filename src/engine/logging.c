#include <time.h>
#include <stdio.h>

#include <yoyoengine/yoyoengine.h>

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
    char *logpath = getEngineResourceStatic("debug.log");
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
    closeLog();
}

void log_init(enum logLevel level){
    logThreshold = level;

    // windows specific tweak to enable ansi colors
    #ifdef _WIN32
    enableVirtualTerminal();
    #endif

    // open log file the first time in w mode to overwrite any existing log
    char *logpath = getEngineResourceStatic("debug.log");
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