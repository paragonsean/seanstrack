// file: nios_cache.c
//
// +---------------------------------------------
// | This file contains routines for manipulating
// | the Nios memory cache hardware
// |
// | dvb 2002, based on macros by tw 2002
// |
// | These routines are in assembly because they
// | make such intimate use of processor
// | control registers
//
// ex:set tabstop=4:
// ex:set shiftwidth=4:
// ex:set expandtab:
        
#include "nios_cache.h"

// +------------------
// | Routines

void nr_icache_init(void)
    {
    int i;

    nm_icache_disable();

    for(i = 0; i < nasys_icache_size; i+= nasys_icache_line_size)
        nm_icache_invalidate_line(i);

    nm_icache_enable();
    }

void nr_dcache_init(void)
    {
    int i;

    nm_dcache_disable();

    for(i = 0; i < nasys_dcache_size; i+= nasys_dcache_line_size)
        nm_dcache_invalidate_line(i);

    nm_dcache_enable();
    }

void nr_icache_invalidate_lines(void *low_address, void *high_address)
    {
    int low_int = (int) low_address;
    int high_int = (int) high_address;

    // If the range we are invalidating is less than the
    // full cache size, then we invalidate only some of
    // the lines. Else, we invalidate all of the lines.

    if( (high_int - low_int) > nasys_icache_size)
        {
        low_int = 0;
        high_int = nasys_icache_size;
        }

    while(low_int < high_int)
        {
        nm_icache_invalidate_line(low_int);
        low_int += nasys_icache_line_size;
        }
    }

void nr_dcache_invalidate_lines(void *low_address, void *high_address)
    {
    int low_int = (int) low_address;
    int high_int = (int) high_address;

    // If the range we are invalidating is less than the
    // full cache size, then we invalidate only some of
    // the lines. Else, we invalidate all of the lines.

    if( (high_int - low_int) > nasys_dcache_size)
        {
        low_int = 0;
        high_int = nasys_dcache_size;
        }

    while(low_int < high_int)
        {
        nm_dcache_invalidate_line(low_int);
        low_int += nasys_dcache_line_size;
        }
    }


void nr_dcache_enable(void)
    {
    nm_dcache_enable();
    }

void nr_dcache_disable(void)
    {
    nm_dcache_disable();
    }

void nr_icache_enable(void)
    {
    nm_icache_enable();
    }

void nr_icache_disable(void)
    {
    nm_icache_disable();
    }



// end of file
