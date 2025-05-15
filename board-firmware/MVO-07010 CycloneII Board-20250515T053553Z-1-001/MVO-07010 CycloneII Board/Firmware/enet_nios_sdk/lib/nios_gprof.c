/*-
 * Copyright (c) 1991 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. [rescinded 22 July 1999]
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*  File: nios_gprof.c
 *  Author: Caio Villela
 *  Description: this file was adapted from the Solaris mcount.c
 *               module.
 */

/*
 * The profiling code uses a timer named na_timer1 to periodically
 * sample the program counter. The rate of sampling is set by
 * the constant TIMER_SAMPLE_RATE, below, in interrupts
 * per second.
 *
 * Each timer interrupt increments one of many "buckets",
 * (counter in the profinfo struct) which represent ranges 
 * of code memory (code chunk size). The default for Nios 32 
 * is 2 bytes for a code chunk size, which is a single Nios 
 * instruction. This increases your data memory footprint 
 * by 100% of your code size. For Nios 16, due to memory
 * restrictions, this has a default set to 8 (or 4 Nios 
 * instructions), causing the counter buffer to equal 25% 
 * of code size.
 *
 * You can, however, use a larger code chunk size (for instance,
 * set HISTFRACTION to 16), with the negative side effect that
 * the profile will occasionally attribute a sample to a different
 * function than the program counter was actually in. This has the
 * positive side effect of reducing the amount of memory you need.
 *
 * To change the code chunk size, assign a larger value to
 * the constant HISTFRACTION, below.
 *
 * dvb 2001
 */

/* Mangled into a form that works on the Nios development environment
 * for Altera Support, September 2001.
 */

#include "excalibur.h"

/* This specifies the timer sample rate in number of ints per second:  */
#define TIMER_SAMPLE_RATE 10000

/* Define a few MACROS : */

#define PTR2LONG(x)  ((long) ((unsigned int) x))

/* convert an addr to an index */
#define PROFIDX(pc, base, scale)                                \
({                                                              \
    unsigned int i = (pc - base) / 2;	                        \
    if (sizeof (unsigned long long int) > sizeof (unsigned int)) \
        i = (unsigned long long int) i * scale / 65536;         \
    else                                                        \
        i = i / 65536 * scale + i % 65536 * scale / 65536;      \
    i;                                                          \
})

/* convert an index into an address */
#define PROFADDR(idx, base, scale)                          \
    ((base) + ((((idx) << 16) / (scale)) << 1))

/* convert a bin size into a scale */
#define PROFSCALE(range, bins)       (((bins) << 16) / ((range) >> 1))

typedef void *_NIOSHANDLE;

struct profinfo {
    _NIOSHANDLE targthr;            /* thread to profile */
    _NIOSHANDLE profthr;            /* profiling thread */
    unsigned short *counter;        /* profiling counters */
    unsigned long lowpc, highpc;    /* range to be profiled */
    long scale;                     /* scale value of bins */
};

/* global profinfo for profil() call */
static struct profinfo prof;

static int not_enough_memory = 0;

/* Some function prototypes:  */

static void moncontrol	(int);
void monstartup	(char *, char *);
void _mcleanup	(void);

char * sbrk (int amount);
void nios_write (int stream, char *msg, int size_of_message);
void nios_profil (unsigned short *bigbuffer, unsigned long buff_size, 
                  unsigned long base, long buf_scale);

void nios_profile_ctl(struct profinfo *, char *, unsigned long, unsigned long, long);

void profile_on (struct profinfo *p);
void profile_off (struct profinfo *p);

static void DoEnableTimerInterrupt(void);
static void DoDisableTimerInterrupt(void);
void nios_prof_hdr_timer_ISR(int context,int irq_number, int interruptee);
int bad_sbrk(void *ptr);

/* CBV - the elements in phdr need to be 32 bit long, not 16. The     */
/* structure originaly defined the first 2 as char pointers, which    */
/* works for nios32 but not nios16. In order to have the 16 bit gprof */
/* work as well, I am changing all the elements to long and casting   */
/* their occurrences in the code.                                     */

struct phdr {
  long lpc;
  long hpc;
  long ncnt;
};

/* Since nios32 addresses a lot more memory, we can be a bit more wastefull */
#ifdef __nios32__

#define HISTFRACTION 2
#define HASHFRACTION 1
#define ARCDENSITY 2
#define MINARCS 50

#else     /* for nios 16, use smaller arrays : */

#define HISTFRACTION 8
#define HASHFRACTION 4
#define ARCDENSITY 2
#define MINARCS 50

#endif

#define HISTCOUNTER unsigned short

struct tostruct {
  char *selfpc;
  long count;
  unsigned short link;
};
struct rawarc {
    unsigned long  raw_frompc;
    unsigned long  raw_selfpc;
    long           raw_count;
};
#define ROUNDDOWN(x,y)  (((x)/(y))*(y))
#define ROUNDUP(x,y)    ((((x)+(y)-1)/(y))*(y))

    /* see profil(2) where this is describe (incorrectly) */
#define		SCALE_1_TO_1	0x10000L

#define	MSG "No space for profiling buffer(s)\n"

char *minbrk;

    /*
     *	froms is actually a bunch of unsigned shorts indexing tos
     */
static int		profiling = 3;
static unsigned short	*froms;
static struct tostruct	*tos = 0;
static long		tolimit = 0;
static char		*s_lowpc = 0;
static char		*s_highpc = 0;
static unsigned long	s_textsize = 0;
static unsigned long   current_stack = 0xFFFF;
static int	ssiz;
static char	*sbuf;
static long	s_scale;

int bad_sbrk(void *ptr)
{
    if(!ptr)
        return 1;
    if(ptr == (void *)-1)
        return 1;

    return 0;
}

void monstartup(lowpc, highpc)
    char    *lowpc;
    char    *highpc;
{
    int     monsize;
    char    *buffer;
    char    *tmp_ptr;
    long    o;

        /*
         *      round lowpc and highpc to multiples of the density we're using
         *      so the rest of the scaling (here and in gprof) stays in ints.
         */
    lowpc = (char *) 
            ((unsigned int) ROUNDDOWN((unsigned int)lowpc, 
		     HISTFRACTION*sizeof(HISTCOUNTER)));

    s_lowpc = lowpc;
    highpc = (char *)
            ((unsigned int) ROUNDUP((unsigned int)highpc, 
	                    HISTFRACTION*sizeof(HISTCOUNTER)));
    s_highpc = highpc;
    s_textsize = highpc - lowpc;

    monsize = (s_textsize / HISTFRACTION) + sizeof(struct phdr);

    buffer =  (char *) sbrk (monsize);

    if ( bad_sbrk(buffer) ) {
//        printf("1 - Error allocating %d at 0x%08x \n", monsize,  buffer);
        not_enough_memory = 1;
        printf("%s \n", MSG);   /* no more room, print error message */
        return;
    }

    tmp_ptr = buffer;     /* zero out the memory ... */
    for (o=0; o < monsize; o++) {
        *tmp_ptr++ = 0;
    }

    froms = (unsigned short *) sbrk( s_textsize / HASHFRACTION );

    if ( bad_sbrk(froms) ) {
//        printf("2 - Error Allocating %d at 0x%08x \n",
//            (short) ( s_textsize / HASHFRACTION ),  (void *)froms);
        printf("%s \n", MSG);   /* no more room, print error message */
        not_enough_memory = 1;
        froms = 0;
        return;
    }
    tmp_ptr = (unsigned char *)froms;     /* zero out the memory ... */
    for (o=0; o < (int)(s_textsize / HASHFRACTION); o++) {
        *tmp_ptr++ = 0;
    }

    tolimit = s_textsize * ARCDENSITY / 100;
    if ( tolimit < MINARCS ) {
        tolimit = MINARCS;
    } else if ( tolimit > 65534 ) {
        tolimit = 65534;
    }

    tos = (struct tostruct *) sbrk( tolimit * sizeof( struct tostruct ) );

    if ( bad_sbrk(tos)) {
//        printf("3 - Error Allocating %d at 0x%08x \n", 
//                     (short) ( tolimit * sizeof( struct tostruct )),  tos);
        printf("%s \n", MSG);   /* no more room, print error message */
        not_enough_memory = 1;
        froms = 0;
        tos = 0;
        return;
    }

    tmp_ptr = (unsigned char *) tos;     /* zero out the memory ... */
    for (o=0; o < (int)(tolimit * sizeof( struct tostruct )); o++) {
        *tmp_ptr++ = 0;
    }

    minbrk = (char *)sbrk(0);
    tos[0].link = 0;
    sbuf = buffer;
    ssiz = monsize;
    ( (struct phdr *) buffer ) -> lpc  = PTR2LONG (lowpc);
    ( (struct phdr *) buffer ) -> hpc  = PTR2LONG (highpc);
    ( (struct phdr *) buffer ) -> ncnt = (long) ssiz;
    monsize -= sizeof(struct phdr);
    if ( monsize <= 0 )
        return;
    o = highpc - lowpc;
    if( monsize < o )
    {
        long quot = o / monsize;

        if (quot >= 0x10000)
                s_scale = 1;
        else if (quot >= 0x100)
                s_scale = 0x10000 / quot;
        else if (o >= 0x800000)
                s_scale = 0x1000000 / (o / (monsize >> 8));
        else
                s_scale = 0x1000000 / ((o << 8) / monsize);
    }
    else
        s_scale = SCALE_1_TO_1;

    moncontrol(1);
}

void _mcleanup()
{
    long           fromindex;
    long           endfrom;
    char           *frompc;
    long           toindex;
    struct rawarc  rawarc;

    moncontrol(0);

    if (not_enough_memory) return;  /* if the malloc did not work, forget it */

    nios_write( 2 , sbuf , ssiz ); /* this is the array used by convert */

    endfrom = s_textsize / (HASHFRACTION * sizeof(*froms));

    for ( fromindex = 0 ; fromindex < endfrom ; fromindex++ ) {
        if ( froms[fromindex] == 0 ) {
            continue;
        }

        frompc = s_lowpc + (fromindex * HASHFRACTION * sizeof(*froms));

        for (toindex=froms[fromindex]; toindex!=0; toindex=tos[toindex].link) {

            rawarc.raw_frompc = PTR2LONG (frompc);
            rawarc.raw_selfpc = PTR2LONG (tos[toindex].selfpc);
            rawarc.raw_count = (long) tos[toindex].count;

            nios_write( 2 , (char *) &rawarc , sizeof rawarc );
        }
    }
}

/*
 * A "call" simply puts the return address in %o7 expecting the
 * "save" in the procedure to shift it into %i7; this means that 
 * before the "save" occurs, %o7 contains the address of the call to 
 * nios_mcount, and %i7 still contains the caller above that. 
 * The asm mcount here simply saves those registers in argument 
 * registers and branches to internal_mcount, simulating a call 
 * with arguments.
 * 	Kludges:
 * 	1) the branch to internal_mcount is hard coded; it should be
 * possible to tell asm to use the assembler-name of a symbol.
 * 	2) in theory, the function calling mcount could have saved %i7
 * somewhere and reused the register; in practice, I *think* this will
 * break longjmp (and maybe the debugger) but I'm not certain. (I take
 * some comfort in the knowledge that it will break the native mcount
 * as well.)
 * 	3) if builtin_return_address worked, this could be portable.
 * However, it would really have to be optimized for arguments of 0
 * and 1 and do something like what we have here in order to avoid the
 * trap per function call performance hit. 
 * 	4) the atexit and monsetup calls prevent this from simply
 * being a leaf routine that doesn't do a "save" (and would thus have
 * access to %o7 and %i7 directly) but the call to nios_write() at the end
 * would have also prevented this.
 *
 */

static void internal_mcount (char *, unsigned short *);

/* i7 == last ret, -> frompcindex */
/* o7 == current ret, -> selfpc */

 asm(".global nios_mcount
        nios_mcount: 
                mov    %o1,%i7 
                mov    %o0,%o7 
                save   %sp,-23
                mov    %o0,%i0
                mov    %o1,%i1
                pfx    %hi(internal_mcount@h)
                movi   %o2, %lo(internal_mcount@h)
		"
#ifdef __nios32__
                "pfx    %xhi(internal_mcount@h)
                movhi  %o2, %xlo(internal_mcount@h)
		"
#endif
                "call   %o2
                nop
                restore
                jmp    %o7
                nop
                ");


/*
 *      Parameters are the return address for mcount,
 *      and the return address for mcount's caller.
 */
static void internal_mcount(selfpc, frompcindex)
register char            *selfpc;
register unsigned short  *frompcindex;
{
    register struct tostruct *top;
    register struct tostruct *prevtop;
    register long   toindex;
    static char already_setup;

/* A little error checking first: if the stack pointer is below the
   data, then we corrupted data and all bets are off: 
*/
    long a, b;
    int x;
    a = PTR2LONG (sbrk (0));
    b = PTR2LONG (&x);

    if ( a >  b) {
        printf("Error - stack underflow - memory corruption\n !!");
	printf("End of data = 0x%08lx , Stack = 0x%08lx \n", (long) a, (long) b);

    }

/* CBV - the frompcindex and selfpc addresses are currently divided
   by 2. So, in order to bring them back in sync with the rest
   of the code, I must make them from jump adddresses to absolute
   addresses.
*/
    frompcindex = (unsigned short *) (2 * ((unsigned int) frompcindex));

    selfpc = (unsigned char *) (2 * ((unsigned int) selfpc));

    if(!already_setup) {

        extern char etext[];

        already_setup = 1;

        monstartup((char *)(unsigned int) nasys_program_mem, 
                   (char *)((unsigned int) etext));


/* CBV - We don't have the operating system on_exit  */
/* call, so force it to use at_exit.  */
/*#ifdef USE_ONEXIT  */
/*          on_exit(_mcleanup, 0);  */
/*#else  */

          atexit(_mcleanup); 

/*#endif  */

    }
        /*
         *      check that we are profiling
         *      and that we aren't recursively invoked.
         */
    if (profiling) {
            goto out;
    }
    profiling++;

        /*
         *      check that frompcindex is a reasonable pc value.
         *      for example:    signal catchers get called from the stack,
         *                      not from text space.  too bad.
         */

    frompcindex = (unsigned short *)(((unsigned int) frompcindex) - 
                  ((unsigned int) s_lowpc));

    if ((unsigned int)frompcindex > s_textsize) {
        goto done;
    }

    frompcindex = (unsigned short *) ((unsigned int) &froms[
                  ((unsigned int) frompcindex) / 
                  (unsigned int) (HASHFRACTION * sizeof(*froms))]);

    toindex = PTR2LONG (*frompcindex);

    if (((unsigned int) toindex) == 0) {
            /*
             *      first time traversing this arc
             */
        toindex = ++tos[0].link;
        if (toindex >= tolimit) {
            goto overflow;
        }

        *frompcindex = (unsigned short) toindex;
        top = &tos[toindex];
        top->selfpc = selfpc;
        top->count = 1;
        top->link = 0;
        goto done;
    }
    top = &tos[toindex];

    if (top->selfpc == selfpc) {
        /*
         *      arc at front of chain; usual case.
         */
        top->count++;
        goto done;
    }
        /*
         *      have to go looking down chain for it.
         *      top points to what we are looking at,
         *      prevtop points to previous top.
         *      we know it is not at the head of the chain.
         */
    for (; /* goto done */; ) {
        if (top->link == 0) {
            /*
             *      top is end of the chain and none of the chain
             *      had top->selfpc == selfpc.
             *      so we allocate a new tostruct
             *      and link it to the head of the chain.
             */
            toindex = ++tos[0].link;
            if (toindex >= tolimit) {
                goto overflow;
            }
            top = &tos[toindex];
            top->selfpc = selfpc;
            top->count = 1;
            top->link = *frompcindex;
            *frompcindex = (unsigned short) toindex;
            goto done;
        }
        /*
         *      otherwise, check the next arc on the chain.
         */
        prevtop = top;
        top = &tos[top->link];
        if (top->selfpc == selfpc) {
        /*
         *      there it is.
         *      increment its count
         *      move it to the head of the chain.
         */
            top->count++;
            toindex = prevtop->link;
            prevtop->link = top->link;
            top->link = *frompcindex;
            *frompcindex = (unsigned short) toindex;
            goto done;
        }
    }
done:
    profiling--;
    /* and fall through */
out:
    return;         /* normal return restores saved registers */

overflow:
    profiling++; /* halt further profiling */
#   define      TOLIMIT "mcount: tos overflow\n"
    nios_write(2, TOLIMIT, sizeof(TOLIMIT));
    goto out;
}


/*
 * Control profiling
 *	profiling is what mcount checks to see if
 *	all the data structures are ready.
 */
static void moncontrol(mode)
    int mode;
{
    if (mode) {
	/* start */
        nios_profil((unsigned short *)(sbuf + sizeof(struct phdr)),
               (unsigned long)(ssiz - sizeof(struct phdr)),
               (unsigned long)((unsigned int)s_lowpc), s_scale);
	profiling = 0;
    } else {
	/* stop */
	nios_profil((unsigned short *)0, 0L, 0L, 0L);
	profiling = 3;
    }
}


void nios_write (int stream, char *msg, int size_of_message)
{
    int i;

    for(i = 0; i < size_of_message; i++) {
        if((i & 0x000f) == 0) {
            printf("\n### ");  /* 3 #'s for convert program */
        }
        printf("%02x ",msg[i] & 0x000000ff);
    }
    printf("*\n");
}

/*
 * start or stop profiling
 *
 * profiling goes into the SAMPLES buffer of size SIZE (which is treated
 * as an array of unsigned shorts of size size/2)
 * 
 * each bin represents a range of pc addresses from OFFSET.  The number
 * of pc addresses in a bin depends on SCALE.  (A scale of 65536 maps
 * each bin to two addresses, A scale of 32768 maps each bin to 4 addresses,
 * a scale of 1 maps each bin to 128k addreses).  Scale may be 1 - 65536,
 * or zero to turn off profiling
 */
void
nios_profile_ctl (struct profinfo * p, char *samples, unsigned long size,
	     unsigned long offset, long scale)
{
    unsigned long maxbin;

    if (scale > 65536) {
      printf ("Error - scale factor overflow!!! \n");
    }

    profile_off (p);
    if (scale) {
        memset (samples, 0, size);
        memset (p, 0, sizeof *p);
        maxbin = size >> 1;
        prof.counter = (unsigned short *) samples;
        prof.lowpc = offset;
        prof.highpc = PROFADDR (maxbin, offset, scale);
        prof.scale = scale;

        profile_on (p);
    }
}

/* Equivalent to unix profil() 
   Every SLEEPTIME interval, the user's program counter (PC) is examined: 
   offset is subtracted and the result is multiplied by scale.  
   The word pointed to by this address is incremented.  Buf is unused. */

void
nios_profil (unsigned short *bigbuffer, unsigned long buff_size, 
                  unsigned long base, long buf_scale)
{
    nios_profile_ctl (&prof, (char *) bigbuffer, buff_size, base, buf_scale);
}

void profile_off (struct profinfo *p)
{
    /* Disable the timer */
    DoDisableTimerInterrupt();
}


#ifndef na_timer1     /* If the timer is not defined, stubb out timer funcs */

void profile_on (struct profinfo *p)
{
	printf("\n\n---->> WARNING!!! na_timer1 NOT defined. The");
	printf(" output generated by nios_gprof will NOT contain");
	printf(" any usefull information!!!");
}

void nios_prof_hdr_timer_ISR(int context, int irq_number, int interruptee)
{}
static void DoEnableTimerInterrupt(void)
{}
static void DoDisableTimerInterrupt(void)
{}

#else


void profile_on (struct profinfo *p)
{
    np_timer *timer = na_timer1;
    long timerPeriod = nasys_clock_freq / TIMER_SAMPLE_RATE;

    timer->np_timerperiodh = timerPeriod >> 16;
    timer->np_timerperiodl = timerPeriod & 0xffff;

    timer->np_timercontrol = np_timercontrol_start_mask |
                             np_timercontrol_cont_mask;

    DoEnableTimerInterrupt();
}

typedef struct
{
    long interruptCount;    // increment with each interrupt
    np_timer *timer;
} TimerISRContext;

static TimerISRContext gC = {0,0};

void nios_prof_hdr_timer_ISR(int context,int irq_number, int interruptee)
{
    struct profinfo *p = (struct profinfo *) &prof;
    unsigned int pc, idx;

    TimerISRContext *c;

    long a, b;
    int x;
    a = PTR2LONG (sbrk (0));
    b = PTR2LONG (&x);

    if (a > b) {
        printf("Error - stack underflow - memory corruption\n !!");
	printf("End of data = 0x%08lx , Stack = 0x%08lx \n",(long) a, (long) b);

    }

    c = (TimerISRContext *)context;

    c->interruptCount++;

  /* first, we must get the PC from the return address before the interrupt */

    pc = 2 * interruptee; /* Convert return address into absolute address */

  /* Now increment the appropriate counter slot: */
    if (pc >= p->lowpc && pc < p->highpc)
    {
        idx = PROFIDX (pc, p->lowpc, p->scale);
        p->counter[idx]++;
    }

    c->timer->np_timerstatus = 0;   // write anything to clear the IRQ
}

// Turn on interrupts: enable interrupt bit, and
// install a user ISR

static void DoEnableTimerInterrupt(void)
{
    gC.timer = na_timer1;
    nr_installuserisr2(na_timer1_irq,(nios_isrhandlerproc2)nios_prof_hdr_timer_ISR, (unsigned int)&gC);

    gC.timer->np_timercontrol = gC.timer->np_timercontrol | 
                                np_timercontrol_ito_mask;
}

static void DoDisableTimerInterrupt(void)
{
    gC.timer = na_timer1;
    nr_installuserisr2(na_timer1_irq,0,0);

    gC.timer->np_timercontrol = gC.timer->np_timercontrol & 
                                ~np_timercontrol_ito_mask;
}

#endif  /* na_timer */
