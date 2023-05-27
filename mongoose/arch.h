#ifndef __MG_ARCH_H__
#define __MG_ARCH_H__

#include "mongoose_config.h"

#define MG_ARCH_CUSTOM 0       // User creates its own mongoose_custom.h
#define MG_ARCH_UNIX 1         // Linux, BSD, Mac, ...
#define MG_ARCH_WIN32 2        // Windows
#define MG_ARCH_ESP32 3        // ESP32
#define MG_ARCH_ESP8266 4      // ESP8266
#define MG_ARCH_FREERTOS 5     // FreeRTOS
#define MG_ARCH_AZURERTOS 6    // MS Azure RTOS
#define MG_ARCH_ZEPHYR 7       // Zephyr RTOS
#define MG_ARCH_NEWLIB 8       // Bare metal ARM
#define MG_ARCH_CMSIS_RTOS1 9  // CMSIS-RTOS API v1 (Keil RTX)
#define MG_ARCH_TIRTOS 10      // Texas Semi TI-RTOS
#define MG_ARCH_RP2040 11      // Raspberry Pi RP2040
#define MG_ARCH_ARMCC 12       // Keil MDK-Core with Configuration Wizard
#define MG_ARCH_CMSIS_RTOS2 13 // CMSIS-RTOS API v2 (Keil RTX5, FreeRTOS)


#define MG_ARCH MG_ARCH_UNIX


#define MG_BIG_ENDIAN (*(uint16_t *) "\0\xff" < 0x100)

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(MG_ENABLE_EPOLL) && MG_ENABLE_EPOLL
#include <sys/epoll.h>
#elif defined(MG_ENABLE_POLL) && MG_ENABLE_POLL
#include <poll.h>
#else
#include <sys/select.h>
#endif

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


#endif // __ARCH_H__
