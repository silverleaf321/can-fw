/* xbee_platform_stm32.c*/
#include "xbee/platform.h" 
#include "platform_config.h"
#include "stm32h7xx_hal.h" 


// Global millisecond counter
static volatile uint32_t MS_TIMER = 0;

// SysTick interrupt handler to update millisecond counter

/**
 * @brief Returns the number of seconds since the system started.
 * @return Elapsed time in seconds.
 */
uint32_t xbee_seconds_timer(void)
{
    return MS_TIMER / 1000;
}

/**
 * @brief Returns the number of milliseconds since the system started.
 * @return Elapsed time in milliseconds.
 */
uint32_t xbee_millisecond_timer(void)
{
    return MS_TIMER;
}
