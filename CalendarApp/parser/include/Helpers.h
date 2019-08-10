#include "CalendarParser.h"
#include <ctype.h>

bool compareProperty(const void *first, const void *second);
bool diffProperty(const void *first, const void *second);
ICalErrorCode errorCode(Calendar *cal, FILE *file, Calendar **obj, ICalErrorCode err);
void unfoldLine(char *lineClear);

char* getEvent(void);
void printFunc(void);
int addTwo(int n);
void putDesc(char* str);
char* getDT(void);
char* getEvent(void);
char* getDesc(void);
