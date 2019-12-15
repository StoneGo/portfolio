#ifndef STR_H
#define STR_H

#include "portfolio_global.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifdef _UNIX
extern PORTFOLIO_API char* strcat_s(char* buf, const size_t buflen, const char* src);
extern PORTFOLIO_API char* strcpy_s(char* buf, const size_t buflen, const char* str);
extern PORTFOLIO_API char* strncpy_s(char* buf, const size_t buflen, const char* str, const int n);
extern PORTFOLIO_API void itoa(unsigned long val, char *buf, unsigned radix);
extern PORTFOLIO_API int fopen_s(FILE** fp, const char* filepath, const char* mode);
extern PORTFOLIO_API void localtime_s(struct tm* ttm, time_t* tt);

#endif //UNIX

extern PORTFOLIO_API char* strtrim(char* buf);
extern PORTFOLIO_API char* strltrim(char* buf);
extern PORTFOLIO_API char* strrtrim(char* buf);
extern PORTFOLIO_API void itoa_s(int num, char* buf, size_t len, int base);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // STR_H
