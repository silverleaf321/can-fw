#ifndef XBEE_PLATFORM_HEADER
#define XBEE_PLATFORM_HEADER
    #include <stdbool.h>
    #include <string.h>
    #define strcmpi         strcasecmp
    #define strncmpi        strncasecmp
    // change this back
    #define PACKED_STRUCT		struct __attribute__ ((__packed__))
	#define XBEE_PACKED(name, decl)	PACKED_STRUCT name decl
    #define _f_memcpy        memcpy
    #define _f_memset        memset
    #include <stdint.h>
    typedef int bool_t;
    #include <inttypes.h>
    // enable the Wi-Fi code by default
    #ifndef XBEE_WIFI_ENABLED
        #define XBEE_WIFI_ENABLED 1
    #endif
#include "xbee/platform.h"

    // enable the cellular code by default
    #ifndef XBEE_CELLULAR_ENABLED
        #define XBEE_CELLULAR_ENABLED 1
    #endif
    #define FAR

    typedef struct xbee_serial_t
    {
        uint32_t                    baudrate;
    } xbee_serial_t;

    // We'll use 1/1/2000 as the epoch, to match ZigBee.
    #define ZCL_TIME_EPOCH_DELTA    0
    #define XBEE_MS_TIMER_RESOLUTION 1

    XBEE_BEGIN_DECLS

    uint16_t _xbee_get_unaligned16( const void FAR *p);
    uint32_t _xbee_get_unaligned32( const void FAR *p);
    void _xbee_set_unaligned16( void FAR *p, uint16_t value);
    void _xbee_set_unaligned32( void FAR *p, uint32_t value);

    // change this back
    #define xbee_get_unaligned16( p)    _xbee_get_unaligned16( p) 
    #define xbee_get_unaligned32( p)    _xbee_get_unaligned32( p)
    #define xbee_set_unaligned16( p, v) _xbee_set_unaligned16( p, v)
    #define xbee_set_unaligned32( p, v) _xbee_set_unaligned32( p, v)

    int xbee_platform_init( void);
    #define XBEE_PLATFORM_INIT() xbee_platform_init()

    XBEE_END_DECLS
#endif