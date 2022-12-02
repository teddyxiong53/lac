#ifndef __MYLOG_H__
#define __MYLOG_H__

extern char *basename(char *s);
#include <stdio.h>

#define myprintf(...) do {\
                        printf("[%s][%s][%d]: ", basename(__FILE__), __func__, __LINE__);\
                        printf(__VA_ARGS__);\
                        printf("\n");\
                       }while(0)
#define mylogd(...) do {\
                            printf("[DEBUG][%s][%s][%d]: ", basename(__FILE__), __func__, __LINE__);\
                            printf(__VA_ARGS__);\
                            printf("\n");\
                           }while(0)
#define myloge(...) do {\
                            printf("[ERROR][%s][%s][%d]: ", basename(__FILE__), __func__, __LINE__);\
                            printf(__VA_ARGS__);\
                            printf("\n");\
                           }while(0)
#define mylogw(...) do {\
                            printf("[WARN][%s][%s][%d]: ", basename(__FILE__), __func__, __LINE__);\
                            printf(__VA_ARGS__);\
                            printf("\n");\
                           }while(0)        
#endif
