/*
 * File: excalibur.h
 *
 * This file is a machine generated address map
 * for a CPU named enet_nios.
 * D:/qdesigns_5_1/CycloneII/emc_chip_12010201/dual_processor.ptf
 * Generated: 2010.07.12 17:05:34

 DO NOT MODIFY THIS FILE

 Changing this file will have subtle consequences
 which will almost certainly lead to a nonfunctioning
 system. If you do modify this file, you MUST
 change both excalibur.h,
 excalibur.s, and excalibur.mk identically.
 Or better yet:

 DO NOT MODIFY THIS FILE

 */

#ifndef _excalibur_
#define _excalibur_

// Legacy SDK will not be supported for Nios II in version 6.0 and beyond.
// Please migrate your software to use the HAL System Library.
// See the Nios II Software Developer's Handbook.

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// The Memory Map

#define na_enet_nios                 ((void *)          0x00000000) // altera_nios
#define na_enet_nios_base                               0x00000000
#define na_MUL_enet_nios             ((void *)          0x00000000) // altera_nios_multiply
#define na_MUL_enet_nios_base                           0x00000000
#define na_MUL_enet_nios_end         ((void *)          0x00000004)
#define na_MUL_enet_nios_size                           0x00000004
#define na_ad_ram                    ((void *)          0x00209000) // altera_avalon_onchip_memory
#define na_ad_ram_base                                  0x00209000
#define na_ad_ram_end                ((void *)          0x00209800)
#define na_ad_ram_size                                  0x00000800
#define na_ad_result_ram             ((void *)          0x00209800) // altera_avalon_onchip_memory
#define na_ad_result_ram_base                           0x00209800
#define na_ad_result_ram_end         ((void *)          0x00209840)
#define na_ad_result_ram_size                           0x00000040
#define na_ad_setup_ram              ((void *)          0x00209900) // altera_avalon_onchip_memory
#define na_ad_setup_ram_base                            0x00209900
#define na_ad_setup_ram_end          ((void *)          0x00209960)
#define na_ad_setup_ram_size                            0x00000060
#define na_ad_cmd_ram                ((void *)          0x00209a00) // altera_avalon_onchip_memory
#define na_ad_cmd_ram_base                              0x00209a00
#define na_ad_cmd_ram_end            ((void *)          0x00209a04)
#define na_ad_cmd_ram_size                              0x00000004
#define na_asmi                      ((np_asmi *)       0x0020b000) // altera_avalon_asmi
#define na_asmi_base                                    0x0020b000
#define na_asmi_irq                                     31
#define na_timer1                    ((np_timer *)      0x0020c200) // altera_avalon_timer
#define na_timer1_base                                  0x0020c200
#define na_timer1_irq                                   16
#define na_uart1                     ((np_uart *)       0x0020c300) // altera_avalon_uart
#define na_uart1_base                                   0x0020c300
#define na_uart1_irq                                    26
#define na_uart2                     ((np_uart *)       0x0020c320) // altera_avalon_uart
#define na_uart2_base                                   0x0020c320
#define na_uart2_irq                                    17
#define na_dither_timer              ((np_timer *)      0x0020c340) // altera_avalon_timer
#define na_dither_timer_base                            0x0020c340
#define na_dither_timer_irq                             24
#define na_button_pio                ((np_pio *)        0x0020c400) // altera_avalon_pio
#define na_button_pio_base                              0x0020c400
#define na_seven_seg_pio             ((np_pio *)        0x0020c500) // altera_avalon_pio
#define na_seven_seg_pio_base                           0x0020c500
#define na_misc_outs                 ((np_pio *)        0x0020c600) // altera_avalon_pio
#define na_misc_outs_base                               0x0020c600
#define na_timer3                    ((np_timer *)      0x0020c700) // altera_avalon_timer
#define na_timer3_base                                  0x0020c700
#define na_timer3_irq                                   23
#define na_dac_spi                   ((np_spi *)        0x0020ca00) // altera_avalon_spi
#define na_dac_spi_base                                 0x0020ca00
#define na_dac_spi_irq                                  59
#define na_misc_ins                  ((np_pio *)        0x0020cc00) // altera_avalon_pio
#define na_misc_ins_base                                0x0020cc00
#define na_pv_unit1                  ((np_usersocket *) 0x0020cd00) // altera_avalon_user_defined_interface
#define na_pv_unit1_base                                0x0020cd00
#define na_opto_data                 ((np_pio *)        0x0020ce00) // altera_avalon_pio
#define na_opto_data_base                               0x0020ce00
#define na_opto_control              ((np_pio *)        0x0020cf00) // altera_avalon_pio
#define na_opto_control_base                            0x0020cf00
#define na_axis1_int                 ((np_pio *)        0x0020d000) // altera_avalon_pio
#define na_axis1_int_base                               0x0020d000
#define na_axis1_int_irq                                19
#define na_one_ms_timer              ((np_timer *)      0x0020d100) // altera_avalon_timer
#define na_one_ms_timer_base                            0x0020d100
#define na_one_ms_timer_irq                             21
#define na_timer2                    ((np_timer *)      0x0020d200) // altera_avalon_timer
#define na_timer2_base                                  0x0020d200
#define na_timer2_irq                                   22
#define na_real_time_data_timer      ((np_timer *)      0x0020d300) // altera_avalon_timer
#define na_real_time_data_timer_base                    0x0020d300
#define na_real_time_data_timer_irq                     27
#define na_pv_unit2                  ((np_usersocket *) 0x0020d400) // altera_avalon_user_defined_interface
#define na_pv_unit2_base                                0x0020d400
#define na_o_scope_timer             ((np_timer *)      0x0020d500) // altera_avalon_timer
#define na_o_scope_timer_base                           0x0020d500
#define na_o_scope_timer_irq                            28
#define na_axis1_div_pio             ((np_pio *)        0x0020d600) // altera_avalon_pio
#define na_axis1_div_pio_base                           0x0020d600
#define na_axis2_div_pio             ((np_pio *)        0x0020d700) // altera_avalon_pio
#define na_axis2_div_pio_base                           0x0020d700
#define na_control_int               ((np_pio *)        0x0020d800) // altera_avalon_pio
#define na_control_int_base                             0x0020d800
#define na_control_int_irq                              25
#define na_bounceback_int            ((np_pio *)        0x0020d900) // altera_avalon_pio
#define na_bounceback_int_base                          0x0020d900
#define na_bounceback_int_irq                           18
#define na_ls_int_input              ((np_pio *)        0x0020da00) // altera_avalon_pio
#define na_ls_int_input_base                            0x0020da00
#define na_ls_int_input_irq                             20
#define na_cont_int_output           ((np_pio *)        0x0020db00) // altera_avalon_pio
#define na_cont_int_output_base                         0x0020db00
#define na_lvdt_demod1_phase         ((np_pio *)        0x0020dc00) // altera_avalon_pio
#define na_lvdt_demod1_phase_base                       0x0020dc00
#define na_lvdt_demod2_phase         ((np_pio *)        0x0020dd00) // altera_avalon_pio
#define na_lvdt_demod2_phase_base                       0x0020dd00
#define na_lvdt_osc_phase            ((np_pio *)        0x0020de00) // altera_avalon_pio
#define na_lvdt_osc_phase_base                          0x0020de00
#define na_samp_interface            ((np_pio *)        0x0020df00) // altera_avalon_pio
#define na_samp_interface_base                          0x0020df00
#define na_lan91c111_0               ((void *)          0x0020e000) // altera_avalon_lan91c111
#define na_lan91c111_0_base                             0x0020e000
#define na_lan91c111_0_end           ((void *)          0x0020e020)
#define na_lan91c111_0_size                             0x00000020
#define na_lan91c111_0_irq                              29
#define na_reboot                    ((np_pio *)        0x0020e100) // altera_avalon_pio
#define na_reboot_base                                  0x0020e100
#define na_enet_boot_rom             ((void *)          0x00210000) // altera_avalon_onchip_memory
#define na_enet_boot_rom_base                           0x00210000
#define na_enet_boot_rom_end         ((void *)          0x002109c4)
#define na_enet_boot_rom_size                           0x000009c4
#define na_germs_rom                 ((void *)          0x00220000) // altera_avalon_onchip_memory
#define na_germs_rom_base                               0x00220000
#define na_germs_rom_end             ((void *)          0x00220600)
#define na_germs_rom_size                               0x00000600
#define na_sram_0                    ((void *)          0x02000000) // altera_nios_dev_kit_stratix_edition_sram2
#define na_sram_0_base                                  0x02000000
#define na_sram_0_end                ((void *)          0x02100000)
#define na_sram_0_size                                  0x00100000
 
#define na_null                           0
#define nasys_device_family               "CYCLONE"
#define nasys_asmi_count                  1
#define nasys_asmi_0                      na_asmi
#define nasys_asmi_0_irq                  31
#define nasys_pio_count                   18
#define nasys_pio_0                       na_button_pio
#define nasys_pio_1                       na_seven_seg_pio
#define nasys_pio_2                       na_misc_outs
#define nasys_pio_3                       na_misc_ins
#define nasys_pio_4                       na_opto_data
#define nasys_pio_5                       na_opto_control
#define nasys_pio_6                       na_axis1_int
#define nasys_pio_6_irq                   19
#define nasys_pio_7                       na_axis1_div_pio
#define nasys_pio_8                       na_axis2_div_pio
#define nasys_pio_9                       na_control_int
#define nasys_pio_9_irq                   25
#define nasys_pio_10                      na_bounceback_int
#define nasys_pio_10_irq                  18
#define nasys_pio_11                      na_ls_int_input
#define nasys_pio_11_irq                  20
#define nasys_pio_12                      na_cont_int_output
#define nasys_pio_13                      na_lvdt_demod1_phase
#define nasys_pio_14                      na_lvdt_demod2_phase
#define nasys_pio_15                      na_lvdt_osc_phase
#define nasys_pio_16                      na_samp_interface
#define nasys_pio_17                      na_reboot
#define nasys_spi_count                   1
#define nasys_spi_0                       na_dac_spi
#define nasys_spi_0_irq                   59
#define nasys_timer_count                 7
#define nasys_timer_0                     na_timer1
#define nasys_timer_0_irq                 16
#define nasys_timer_1                     na_dither_timer
#define nasys_timer_1_irq                 24
#define nasys_timer_2                     na_timer3
#define nasys_timer_2_irq                 23
#define nasys_timer_3                     na_one_ms_timer
#define nasys_timer_3_irq                 21
#define nasys_timer_4                     na_timer2
#define nasys_timer_4_irq                 22
#define nasys_timer_5                     na_real_time_data_timer
#define nasys_timer_5_irq                 27
#define nasys_timer_6                     na_o_scope_timer
#define nasys_timer_6_irq                 28
#define nasys_uart_count                  2
#define nasys_uart_0                      na_uart1
#define nasys_uart_0_irq                  26
#define nasys_uart_1                      na_uart2
#define nasys_uart_1_irq                  17
#define nasys_usersocket_count            2
#define nasys_usersocket_0                na_pv_unit1
#define nasys_usersocket_1                na_pv_unit2
#define nasys_vector_table      ((int *)  0x020fff00)
#define nasys_vector_table_size           0x00000100
#define nasys_vector_table_end  ((int *)  0x02100000)
#define nasys_reset_address     ((void *) 0x00210000)
#define nasys_clock_freq                  33330000
#define nasys_clock_freq_1000             33330
#define nasys_debug_core                  0
#define nasys_printf_uart                 na_uart1
#define nasys_printf_uart_irq             na_uart1_irq
#define nm_printf_txchar                  nr_uart_txchar
#define nm_printf_rxchar                  nr_uart_rxchar
#define nasys_debug_uart                  na_uart2
#define nasys_debug_uart_irq              na_uart2_irq
#define nasys_program_mem       ((void *) 0x02000000)
#define nasys_program_mem_size            0x000fff00
#define nasys_program_mem_end   ((void *) 0x020fff00)
#define nasys_data_mem          ((void *) 0x02000000)
#define nasys_data_mem_size               0x000fff00
#define nasys_data_mem_end      ((void *) 0x020fff00)
#define nasys_stack_top         ((void *) 0x020fff00)
 
#define na_asmi_64K                 0      // ASMI part is 64k bits
#define na_asmi_1M                  0      // ASMI part is 1M bits
#define na_asmi_4M                  1      // ASMI part is 4M bits
#define LAN91C111_REGISTERS_OFFSET  0x0000 // offset 0 or 0x300, depending on address bus wiring
#define LAN91C111_DATA_BUS_WIDTH    16     // width 16 or 32, depending on data bus wiring
#define __nios_catch_irqs__         1      // Include panic handler for all irqs (needs uart)
#define __nios_use_constructors__   0      // Call c++ static constructors
#define __nios_use_cwpmgr__         1      // Handle register window underflows
#define __nios_use_fast_mul__       1      // Faster but larger int multiply routine
#define __nios_use_small_printf__   1      // Smaller non-ANSI printf, with no floating point
#define __nios_use_multiply__       1      // Use MUL instruction for 16x16
#define __nios_use_mstep__          0      // Use MSTEP instruction for 16x16
#define __nios_jtag_stdio__         0      // True if Host Communication is OCI and not UART
#define nasys_nios_num_regs         256    // Number of CPU registers
#define nasys_has_icache            0      // True if instruction cache present
#define nasys_icache_size           0      // Size in bytes of instruction cache
#define nasys_icache_line_size      2      // Size in bytes of each icache line
#define nasys_has_dcache            0      // True if instruction cache present
#define nasys_dcache_size           0      // Size in bytes of data cache
#define nasys_dcache_line_size      4      // Size in bytes of each dcache line
#define nasys_nios_register_count   512
#define PLUGS_PLUG_COUNT            10     // Maximum number of plugs
#define PLUGS_ADAPTER_COUNT         2      // Maximum number of adapters
#define PLUGS_DNS                   0      // Have routines for DNS lookups
#define PLUGS_PING                  0      // Respond to icmp echo (ping) messages
#define PLUGS_TCP                   1      // Support tcp in/out connections
#define PLUGS_IRQ                   0      // Run at interrupte level
#define PLUGS_DEBUG                 1      // Support debug routines



#define nm_system_name_string "dual_processor"
#define nm_cpu_name_string "enet_nios"
#define nm_monitor_string "cyc_chip_17010201"
#define nm_cpu_architecture nios_32
#define nm_cpu_architecture_string "nios_32"
#define nios_32 1

// Structures and Routines For Each Peripheral

#define nm_divi(_x,_y) ({\
	int __x = (_x), __y = (_y);\
	asm volatile("usr0 %0,%2 ; does divi" \
	: "=r" (__x) \
	: "0" (__x), "r" (__y));\
	__x;\
	})

// Nios CPU Routines
void nr_installcwpmanager(void);	// called automatically at by nr_setup.s
void nr_delay(int milliseconds);	// approximate timing based on clock speed
void nr_zerorange(char *rangeStart,int rangeByteCount);
void nr_jumptoreset(void);

typedef void (*nios_callfromresetproc)(void);

void nr_callfromreset(nios_callfromresetproc procptr);

// Nios ISR Manager Routines
typedef void (*nios_isrhandlerproc)(int context);
typedef void (*nios_isrhandlerproc2)(int context,int irq_number,int interruptee_pc);
void nr_installuserisr(int trapNumber,nios_isrhandlerproc handlerProc,int context);
void nr_installuserisr2(int trapNumber,nios_isrhandlerproc2 handlerProc,int context);
int nr_setirqenable(int onoff);

// Nios GDB Stub Functions
void nios_gdb_install(int active);
#define nios_gdb_breakpoint() asm("TRAP 5")

// Nios OCI Defines
#define nios_oci_breakpoint() asm("TRAP 0")

// Default UART routines
void nr_txchar(int c);
void nr_txchar2(int c, int channel);
void nr_txstring(char *s);
int nr_rxchar(void);

// Debug UART routines
void nr_debug_txchar(int c);
void nr_debug_txstring(char *s);
int nr_debug_rxchar(void);

// JTAG IO routines
int nr_jtag_rxchar(const void* ociBase);
int nr_jtag_tx_ready(const void* ociBase);
void nr_jtag_txchar(int c,const void* ociBase); 
void nr_jtag_txcr(void);
void nr_jtag_txhex(int x); 
void nr_jtag_txhex16(short x);
void nr_jtag_txhex32(long x);
void nr_jtag_txstring(const char *s);

// Nios Private Printf Routines
int nr_printf(const char *fmt,...);
//int nr_fprintf( FILE *fp, const char *fmt, ... )
int nr_sprintf(char *sOut,const char *fmt,...);

#define	nk_stdout       1
#define nk_stderr       2

#if __nios_use_small_printf__
	#define printf nr_printf
	#define fprintf nr_fprintf
	#define sprintf nr_sprintf
    #define setbuf(v1,v2) nr_setbuf(v1,v2)   // does nothing
#endif

#ifdef na_enet
void nios_gdb_install_ethernet(int active);
int nr_debug_plugs_idle ();
#endif

#if __nios_debug__
	#define NIOS_GDB_SETUP 		\
		nios_gdb_install(1);	\
		nios_gdb_breakpoint();
#else
	#define NIOS_GDB_SETUP
#endif

// debug Core Declarations

#define nasys_debug_core_irq                8

// debug registers offsets from base
enum
{
    np_debug_interrupt =		0,		//read-only,  4 bits, reading stops trace
    np_debug_n_samples_lsb,			//read-only, 16 bits
    np_debug_n_samples_msb,			//read-only, 16 bits
    np_debug_data_valid,				//read-only, 1 bit, 
    						//true when trace registers contain valid sample
    np_debug_trace_address,			//read-only, 16 or 32 bits
    np_debug_trace_data,				//read-only, 16 or 32 bits
    np_debug_trace_code,				//read-only, 16 or 32 bits
    np_debug_write_status,			//read-only, 1 bit, 
    						//true when read to readback tracedata
    np_debug_start,				//write-only, write any value to start
    np_debug_stop,				//write-only, write any value to stop
    np_debug_read_sample,				//write-only, write any value to read
    np_debug_trace_mode,				//write-only, 1 bit
    np_debug_mem_int_enable,			//write-only, 16 or 32 bits ?????
    np_debug_ext_brk_enable,			//write-only, 1 bit
    np_debug_sw_reset,				//write-only, reset sampels and trace memory

    np_debug_address_pattern_0 =	16,		//write-only, 16 or 32 bits
    np_debug_address_mask_0,			//write-only, 16 or 32 bits
    np_debug_data_pattern_0,			//write-only, 16 or 32 bits
    np_debug_data_mask_0,				//write-only, 16 or 32 bits
    np_debug_code_0,				//write-only, 16 or 32 bits

    np_debug_address_pattern_1 =	24,		//write-only, 16 or 32 bits
    np_debug_address_mask_1,			//write-only, 16 or 32 bits
    np_debug_data_pattern_1,			//write-only, 16 or 32 bits
    np_debug_data_mask_1,				//write-only, 16 or 32 bits
    np_debug_code_1,				//write-only, 16 or 32 bits
};

// debug Register Bits/Codes
enum 
{ 
    /************************************************/
    // debug_interrupt register
    // bit numbers
    np_debug_interrupt_code_dbp0_bit = 0,
    np_debug_interrupt_code_dbp1_bit = 1,
    np_debug_interrupt_code_ibp0_bit = 2,
    np_debug_interrupt_code_ibp1_bit = 3,
    np_debug_interrupt_code_mem_bit = 4,

    // bit masks
    np_debug_interrupt_code_ext_mask = (0), 
    np_debug_interrupt_code_dbp0_mask = (1<<0),
    np_debug_interrupt_code_dbp1_mask = (1<<1),
    np_debug_interrupt_code_ibp0_mask = (1<<2),
    np_debug_interrupt_code_ibp1_mask = (1<<3),
    np_debug_interrupt_code_mem_mask = (1<<4),

    /************************************************/
    // debug_trace_code register
    // bit numbers
    np_debug_trace_code_skp_bit = 1,
    np_debug_trace_code_fifo_full_bit = 2,
    np_debug_trace_code_bus_bit = 3,
    np_debug_trace_code_rw_bit = 4,
    np_debug_trace_code_intr_bit = 5,
    
    // bit masks
    np_debug_trace_code_skp_mask = (1<<0),
    np_debug_trace_code_fifo_full_mask = (1<<1),
    np_debug_trace_code_data_trans_mask = (1<<2), //instr trans if 0
    np_debug_trace_code_write_mask = (1<<3),	//read if 0
    np_debug_trace_code_intr_mask = (1<<4),
#ifdef __nios32__
    np_debug_trace_code_skp_cnt_mask = (63<<2),
#else
    np_debug_trace_code_skp_cnt_mask = (31<<2),
#endif

    // useful constants
    np_debug_trace_code_op_mask = (np_debug_trace_code_data_trans_mask|np_debug_trace_code_write_mask),
    np_debug_trace_code_read = np_debug_trace_code_data_trans_mask,
    np_debug_trace_code_write = (np_debug_trace_code_data_trans_mask|np_debug_trace_code_write_mask),
    np_debug_trace_code_fetch = 0,

    /************************************************/
    // debug_code_* registers
    // bit numbers
    np_debug_break_code_read_bit = 0,
    np_debug_break_code_write_bit = 1,
    np_debug_break_code_fetch_bit = 2,

    // bit masks
    np_debug_break_code_read_mask = (1<<0),
    np_debug_break_code_write_mask = (1<<1),
    np_debug_break_code_fetch_mask = (1<<2),

    /************************************************/
    // debug_write_status register
    // bit numbers
    np_debug_write_status_writing_bit = 0,
    np_debug_write_status_nios32_bit = 1,
    np_debug_write_status_trace_bit = 2,

    // bit masks
    np_debug_write_status_writing_mask = (1<<0),
    np_debug_write_status_nios32_mask = (1<<1),
    np_debug_write_status_trace_mask = (1<<2)
};

// debug Routine


//Get the number of trace samples 
unsigned long nr_debug_num_samples (void);	

//Stop debug core - this must be a function so that a branch is the last
//                  thing in the fifo.  Otherwise a skip or a read/write
//                  might be logged without the ability to sync it up
void nr_debug_stop (void);

//Read a trace sample
void nr_debug_get_sample (unsigned int *trace_addr, 
			unsigned int *trace_data, 
			unsigned char *trace_code);

//must have a uart for these functions
#ifdef nasys_uart_count
#if (nasys_uart_count > 0)

  //print cause of break
  void nr_debug_show_break (void *uart);
                                  // 0 for default printf uart

  //dump the trace memory
  void nr_debug_dump_trace (void *uart);
                             // 0 for defualt printf uart

  //ISR for debug interrupts
  //Show cause of break, dump trace, and halt
  void nr_debug_isr_halt (int context);		
                            // 0 for default printf uart otherwise uart base address

  //Show cause of break, dump trace, and continue
  void nr_debug_isr_continue (int context);		
                                // 0 for default printf uart otherwise uart base address

#endif
#endif

// debug macros
//Read a debug register
#define nm_debug_get_reg(ret, offset) 		\
		asm volatile (			\
			"PFX 3 \n		\
			WRCTL %1 \n		\
			PFX 4 \n		\
			RDCTL %0;"		\
			:"=r" (ret)		\
			:"r"(offset)		\
		);

//Write a debug register
#define nm_debug_set_reg(val,offset) 		\
		asm volatile (			\
			"PFX 3 \n		\
			WRCTL %1 \n		\
			PFX 4 \n		\
			WRCTL %0;"		\
			: /* no outputs */	\
			:"r"(val),"r"(offset)	\
		);

//Set breakpoint 0
#define nm_debug_set_bp0(ap,am,dp,dm,cd) nm_debug_set_reg(ap,np_debug_address_pattern_0);\
				       nm_debug_set_reg(am,np_debug_address_mask_0);\
				       nm_debug_set_reg(dp,np_debug_data_pattern_0);\
				       nm_debug_set_reg(dm,np_debug_data_mask_0);\
				       nm_debug_set_reg(cd,np_debug_code_0);

//Set breakpoint 1
#define nm_debug_set_bp1(ap,am,dp,dm,cd) nm_debug_set_reg(ap,np_debug_address_pattern_1);\
				       nm_debug_set_reg(am,np_debug_address_mask_1);\
				       nm_debug_set_reg(dp,np_debug_data_pattern_1);\
				       nm_debug_set_reg(dm,np_debug_data_mask_1);\
				       nm_debug_set_reg(cd,np_debug_code_1);

//Set extended trace mode
#define nm_debug_set_extended_trace nm_debug_set_reg(1,np_debug_trace_mode);

//Set memory interrupt point
#define nm_debug_set_wrap_point(size) nm_debug_set_reg((size>>2),np_debug_mem_int_enable);

// Include nios cache-control definitions and macros

#include "nios_cache.h"

// ASMI Registers
typedef volatile struct
  {
  int np_asmirxdata;       // Read-only, 1-16 bit
  int np_asmitxdata;       // Write-only, same width as rxdata
  int np_asmistatus;       // Read-only, 9-bit
  int np_asmicontrol;      // Read/Write, 9-bit
  int np_asmireserved;     // reserved
  int np_asmislaveselect;  // Read/Write, 1-16 bit, master only
  int np_asmiendofpacket;  // Read/write, same width as txdata, rxdata.
  } np_asmi;

// ASMI Status Register Bits
enum
  {
  np_asmistatus_eop_bit  = 9,
  np_asmistatus_e_bit    = 8,
  np_asmistatus_rrdy_bit = 7,
  np_asmistatus_trdy_bit = 6,
  np_asmistatus_tmt_bit  = 5,
  np_asmistatus_toe_bit  = 4,
  np_asmistatus_roe_bit  = 3,

  np_asmistatus_eop_mask  = (1 << 9),
  np_asmistatus_e_mask    = (1 << 8),
  np_asmistatus_rrdy_mask = (1 << 7),
  np_asmistatus_trdy_mask = (1 << 6),
  np_asmistatus_tmt_mask  = (1 << 5),
  np_asmistatus_toe_mask  = (1 << 4),
  np_asmistatus_roe_mask  = (1 << 3),
  };

// ASMI Control Register Bits
enum
  {
  np_asmicontrol_sso_bit   = 10,
  np_asmicontrol_ieop_bit  = 9,
  np_asmicontrol_ie_bit    = 8,
  np_asmicontrol_irrdy_bit = 7,
  np_asmicontrol_itrdy_bit = 6,
  np_asmicontrol_itoe_bit  = 4,
  np_asmicontrol_iroe_bit  = 3,

  np_asmicontrol_sso_mask   = (1 << 10),
  np_asmicontrol_ieop_mask  = (1 << 9),
  np_asmicontrol_ie_mask    = (1 << 8),
  np_asmicontrol_irrdy_mask = (1 << 7),
  np_asmicontrol_itrdy_mask = (1 << 6),
  np_asmicontrol_itoe_mask  = (1 << 4),
  np_asmicontrol_iroe_mask  = (1 << 3),
  };


//ASMI memory definitions
#if na_asmi_64K
#define na_asmi_bulk_size   (0x2000)
#define na_asmi_sector_size (na_asmi_bulk_size >> 2)
#define na_asmi_page_size   0x20

#elif na_asmi_1M
#define na_asmi_bulk_size   (0x20000)
#define na_asmi_sector_size (na_asmi_bulk_size >> 2)
#define na_asmi_page_size   0x100

#elif na_asmi_4M
#define na_asmi_bulk_size   (0x80000)
#define na_asmi_sector_size (na_asmi_bulk_size >> 3)
#define na_asmi_page_size   0x100
#endif

//ASMI memory instructions
#define na_asmi_read    (unsigned char)0x03
#define na_asmi_write   (unsigned char)0x02
#define na_asmi_wren    (unsigned char)0x06
#define na_asmi_wrdi    (unsigned char)0x04
#define na_asmi_rdsr    (unsigned char)0x05
#define na_asmi_wrsr    (unsigned char)0x01
#define na_asmi_se      (unsigned char)0xd8
#define na_asmi_be      (unsigned char)0xc7
#define na_asmi_dp      (unsigned char)0xb9

//ASMI memory status register bit masks
#if (na_asmi_64K) || (na_asmi_1M)
#define na_asmi_bp      (unsigned char)0xc
#else
#define na_asmi_bp      (unsigned char)0x1c
#endif
#define na_asmi_wel     (unsigned char)0x2
#define na_asmi_wip     (unsigned char)0x1

//ASMI function error codes
#define na_asmi_success                 0
#define na_asmi_err_device_not_present  1
#define na_asmi_err_device_not_ready    2
#define na_asmi_err_timedout            3
#define na_asmi_err_write_failed        4
#define na_asmi_invalid_config          5

//ASMI protection masks
#define na_asmi_protect_none        0
#if (na_asmi_64K) || (na_asmi_1M)
#define na_asmi_protect_top_quarter 0x4
#define na_asmi_protect_top_half    0x8
#define na_asmi_protect_all         0xc
#else
#define na_asmi_protect_top_eighth  0x4
#define na_asmi_protect_top_quarter 0x8
#define na_asmi_protect_top_half    0xc
#define na_asmi_protect_all         0x10
#endif

//ASMI macros
//returns the protect bits shifted into the lsbs
#define nm_asmi_prot_sect(t) ((t & na_asmi_bp) >> 2)

//ASMI library routines
//

extern unsigned char nr_asmi_read_status ();
extern unsigned long nr_asmi_lowest_protected_address();
extern int nr_asmi_write_status (unsigned char value);
extern int nr_asmi_protect_region (int bpcode);
extern int nr_asmi_read_byte (unsigned long address, unsigned char *data);
extern int nr_asmi_write_byte (unsigned long address, unsigned char data);
extern int nr_asmi_erase_sector (unsigned long address);
extern int nr_asmi_erase_bulk ();
extern int nr_asmi_read_buffer (unsigned long address, int length, unsigned char *data);
extern int nr_asmi_write_page (unsigned long address, int length, unsigned char *data);
extern int nr_asmi_write_buffer (unsigned long address, int length, unsigned char *data);
extern int nr_asmi_address_past_config (unsigned long *addr);
        

// ----------------------------------------------
// Timer Peripheral

// Timer Registers
typedef volatile struct
	{
	int np_timerstatus;  // read only, 2 bits (any write to clear TO)
	int np_timercontrol; // write/readable, 4 bits
	int np_timerperiodl; // write/readable, 16 bits
	int np_timerperiodh; // write/readable, 16 bits
	int np_timersnapl;   // read only, 16 bits
	int np_timersnaph;   // read only, 16 bits
	} np_timer;

// Timer Register Bits
enum
	{
	np_timerstatus_run_bit    = 1, // timer is running
	np_timerstatus_to_bit     = 0, // timer has timed out

	np_timercontrol_stop_bit  = 3, // stop the timer
	np_timercontrol_start_bit = 2, // start the timer
	np_timercontrol_cont_bit  = 1, // continous mode
	np_timercontrol_ito_bit   = 0, // enable time out interrupt

	np_timerstatus_run_mask    = (1<<1), // timer is running
	np_timerstatus_to_mask     = (1<<0), // timer has timed out

	np_timercontrol_stop_mask  = (1<<3), // stop the timer
	np_timercontrol_start_mask = (1<<2), // start the timer
	np_timercontrol_cont_mask  = (1<<1), // continous mode
	np_timercontrol_ito_mask   = (1<<0)  // enable time out interrupt
	};

// Timer Routines
int nr_timer_milliseconds(void);	// Starts on first call, hogs timer1.


// UART Registers
typedef volatile struct
	{
	int np_uartrxdata;      // Read-only, 8-bit
	int np_uarttxdata;      // Write-only, 8-bit
	int np_uartstatus;      // Read-only, 8-bit
	int np_uartcontrol;     // Read/Write, 9-bit
	int np_uartdivisor;     // Read/Write, 16-bit, optional
	int np_uartendofpacket; // Read/Write, end-of-packet character
	} np_uart;

// UART Status Register Bits
enum
	{
	np_uartstatus_eop_bit  = 12,
	np_uartstatus_cts_bit  = 11,
	np_uartstatus_dcts_bit = 10,
	np_uartstatus_e_bit    = 8,
	np_uartstatus_rrdy_bit = 7,
	np_uartstatus_trdy_bit = 6,
	np_uartstatus_tmt_bit  = 5,
	np_uartstatus_toe_bit  = 4,
	np_uartstatus_roe_bit  = 3,
	np_uartstatus_brk_bit  = 2,
	np_uartstatus_fe_bit   = 1,
	np_uartstatus_pe_bit   = 0,

	np_uartstatus_eop_mask  = (1<<12),
	np_uartstatus_cts_mask  = (1<<11),
	np_uartstatus_dcts_mask = (1<<10),
	np_uartstatus_e_mask    = (1<<8),
	np_uartstatus_rrdy_mask = (1<<7),
	np_uartstatus_trdy_mask = (1<<6),
	np_uartstatus_tmt_mask  = (1<<5),
	np_uartstatus_toe_mask  = (1<<4),
	np_uartstatus_roe_mask  = (1<<3),
	np_uartstatus_brk_mask  = (1<<2),
	np_uartstatus_fe_mask   = (1<<1),
	np_uartstatus_pe_mask   = (1<<0)
	};

// UART Control Register Bits
enum
	{
	np_uartcontrol_ieop_bit  = 12,
	np_uartcontrol_rts_bit   = 11,
	np_uartcontrol_idcts_bit = 10,
	np_uartcontrol_tbrk_bit  = 9,
	np_uartcontrol_ie_bit    = 8,
	np_uartcontrol_irrdy_bit = 7,
	np_uartcontrol_itrdy_bit = 6,
	np_uartcontrol_itmt_bit  = 5,
	np_uartcontrol_itoe_bit  = 4,
	np_uartcontrol_iroe_bit  = 3,
	np_uartcontrol_ibrk_bit  = 2,
	np_uartcontrol_ife_bit   = 1,
	np_uartcontrol_ipe_bit   = 0,

	np_uartcontrol_ieop_mask  = (1<<12),
	np_uartcontrol_rts_mask   = (1<<11),
	np_uartcontrol_idcts_mask = (1<<10),
	np_uartcontrol_tbrk_mask  = (1<<9),
	np_uartcontrol_ie_mask    = (1<<8),
	np_uartcontrol_irrdy_mask = (1<<7),
	np_uartcontrol_itrdy_mask = (1<<6),
	np_uartcontrol_itmt_mask  = (1<<5),
	np_uartcontrol_itoe_mask  = (1<<4),
	np_uartcontrol_iroe_mask  = (1<<3),
	np_uartcontrol_ibrk_mask  = (1<<2),
	np_uartcontrol_ife_mask   = (1<<1),
	np_uartcontrol_ipe_mask   = (1<<0)
	};

// UART Routines
int nr_uart_rxchar(np_uart *uartBase);        // 0 for default UART
void nr_uart_txcr(void);
void nr_uart_txchar(int c,np_uart *uartBase); // 0 for default UART
void nr_uart_txhex(int x);                     // 16 or 32 bits
void nr_uart_txhex16(short x);
void nr_uart_txhex32(long x);
void nr_uart_txstring(char *s);

// PIO Peripheral

// PIO Registers
typedef volatile struct
	{
	int np_piodata;          // read/write, up to 32 bits
	int np_piodirection;     // write/readable, up to 32 bits, 1->output bit
	int np_piointerruptmask; // write/readable, up to 32 bits, 1->enable interrupt
	int np_pioedgecapture;   // read, up to 32 bits, cleared by any write
	} np_pio;

// PIO Routines
void nr_pio_showhex(int value); // shows low byte on pio named na_seven_seg_pio

// SPI Registers
typedef volatile struct
  {
  int np_spirxdata;       // Read-only, 1-16 bit
  int np_spitxdata;       // Write-only, same width as rxdata
  int np_spistatus;       // Read-only, 9-bit
  int np_spicontrol;      // Read/Write, 9-bit
  int np_spireserved;     // reserved
  int np_spislaveselect;  // Read/Write, 1-16 bit, master only
  int np_spiendofpacket;  // Read/write, same width as txdata, rxdata.
  } np_spi;

// SPI Status Register Bits
enum
  {
  np_spistatus_eop_bit  = 9,
  np_spistatus_e_bit    = 8,
  np_spistatus_rrdy_bit = 7,
  np_spistatus_trdy_bit = 6,
  np_spistatus_tmt_bit  = 5,
  np_spistatus_toe_bit  = 4,
  np_spistatus_roe_bit  = 3,

  np_spistatus_eop_mask  = (1 << 9),
  np_spistatus_e_mask    = (1 << 8),
  np_spistatus_rrdy_mask = (1 << 7),
  np_spistatus_trdy_mask = (1 << 6),
  np_spistatus_tmt_mask  = (1 << 5),
  np_spistatus_toe_mask  = (1 << 4),
  np_spistatus_roe_mask  = (1 << 3),
  };

// SPI Control Register Bits
enum
  {
  np_spicontrol_sso_bit   = 10,
  np_spicontrol_ieop_bit  = 9,
  np_spicontrol_ie_bit    = 8,
  np_spicontrol_irrdy_bit = 7,
  np_spicontrol_itrdy_bit = 6,
  np_spicontrol_itoe_bit  = 4,
  np_spicontrol_iroe_bit  = 3,

  np_spicontrol_sso_mask   = (1 << 10),
  np_spicontrol_ieop_mask  = (1 << 9),
  np_spicontrol_ie_mask    = (1 << 8),
  np_spicontrol_irrdy_mask = (1 << 7),
  np_spicontrol_itrdy_mask = (1 << 6),
  np_spicontrol_itoe_mask  = (1 << 4),
  np_spicontrol_iroe_mask  = (1 << 3),
  };

// SPI Routines.
int nr_spi_rxchar(np_spi *spiBase);
int nr_spi_txchar(int i, np_spi *spiBase);



// ----------------------------------------------
// User Socket

typedef void *np_usersocket;





// ===========================================================
// Parameters for Each Peripheral, Excerpted From The PTF File



// ------------------
// Parameters for altera_nios_multiply named MUL_enet_nios

// ci_cycles = 3



// ------------------
// Parameters for altera_nios_custom_instr_divide named USR0_enet_nios

// Module_Name             = divide_instruction_unit
// ci_macro_name           = divi
// ci_operands             = 2
// ci_cycles               = 35
// Has_Prefix              = 0
// Synthesize_Imported_HDL = 1
// ci_instr_format         = RR



// ------------------
// Parameters for altera_nios named enet_nios

// CPU_Architecture     = nios_32
// mstep                = 0
// multiply             = 1
// rom_decoder          = 1
// wvalid_wr            = 0
// num_regs             = 512
// do_generate          = 1
// include_debug        = 0
// include_trace        = 0
// include_oci          = 0
// oci_offchip_trace    = 0
// oci_onchip_trace     = 0
// oci_num_xbrk         = 0
// oci_num_dbrk         = 0
// oci_dbrk_trace       = 0
// oci_dbrk_pairs       = 0
// oci_debugreq_signals = 0
// reset_slave          = enet_boot_rom/s1
// reset_offset         = 0x00000000
// vecbase_slave        = sram_0/s1
// vecbase_offset       = 0x000FFF00
// support_interrupts   = 1
// implement_forward_b1 = 1
// support_rlc_rrc      = 0
// advanced             = 1
// cache_has_icache     = 0
// cache_icache_size_k  = 0
// cache_has_dcache     = 0
// cache_dcache_size_k  = 0
// CONSTANTS            =
// mainmem_slave        = sram_0/s1
// datamem_slave        = sram_0/s1
// maincomm_slave       = uart1/s1
// debugcomm_slave      = uart2/s1
// germs_monitor_id     = cyc_chip_17010201



// ------------------
// Parameters for altera_plugs_library named altera_plugs_library

// CONSTANTS =



// ------------------
// Parameters for altera_avalon_onchip_memory named ad_ram

// Writeable                         = 1
// Size_Value                        = 2
// Size_Multiple                     = 1024
// Contents                          = blank
// Shrink_to_fit_contents            = 0
// use_altsyncram                    = 1
// use_ram_block_type                = M4K
// altsyncram_ram_block_type         = AUTO
// dual_port                         = 0
// allow_mram_sim_contents_only_file = 0
// CONTENTS                          = srec



// ------------------
// Parameters for altera_avalon_onchip_memory named ad_result_ram

// Writeable                         = 1
// Size_Value                        = 64
// Size_Multiple                     = 1
// Contents                          = blank
// Shrink_to_fit_contents            = 0
// use_altsyncram                    = 1
// use_ram_block_type                = M4K
// altsyncram_ram_block_type         = AUTO
// dual_port                         = 0
// allow_mram_sim_contents_only_file = 0
// CONTENTS                          = srec



// ------------------
// Parameters for altera_avalon_onchip_memory named ad_setup_ram

// Writeable                         = 1
// Size_Value                        = 96
// Size_Multiple                     = 1
// Contents                          = blank
// Shrink_to_fit_contents            = 0
// use_altsyncram                    = 1
// use_ram_block_type                = M4K
// altsyncram_ram_block_type         = AUTO
// dual_port                         = 0
// allow_mram_sim_contents_only_file = 0
// CONTENTS                          = srec



// ------------------
// Parameters for altera_avalon_onchip_memory named ad_cmd_ram

// Writeable                         = 1
// Size_Value                        = 4
// Size_Multiple                     = 1
// Contents                          = blank
// Shrink_to_fit_contents            = 0
// use_altsyncram                    = 1
// use_ram_block_type                = M4K
// altsyncram_ram_block_type         = AUTO
// dual_port                         = 0
// allow_mram_sim_contents_only_file = 0
// CONTENTS                          = srec



// ------------------
// Parameters for altera_avalon_asmi named asmi

// databits      = 8
// targetclock   = 20
// clockunits    = MHz
// clockmult     = 1000000
// numslaves     = 1
// ismaster      = 1
// clockpolarity = 0
// clockphase    = 0
// lsbfirst      = 0
// extradelay    = 0
// targetssdelay = 100
// delayunits    = us
// delaymult     = 1e-006
// prefix        = asmi_
// CONSTANTS     =
// clockunit     = kHz
// delayunit     = us



// ------------------
// Parameters for altera_avalon_timer named timer1

// always_run           = 0
// fixed_period         = 0
// snapshot             = 1
// period               = 1
// period_units         = ms
// reset_output         = 0
// timeout_pulse_output = 0
// mult                 = 0.001



// ------------------
// Parameters for altera_avalon_uart named uart1

// baud             = 115200
// data_bits        = 8
// fixed_baud       = 1
// parity           = N
// stop_bits        = 2
// use_cts_rts      = 0
// use_eop_register = 0
// sim_true_baud    = 0
// sim_char_stream  = g40000\n



// ------------------
// Parameters for altera_avalon_uart named uart2

// baud             = 9600
// data_bits        = 8
// fixed_baud       = 0
// parity           = N
// stop_bits        = 1
// use_cts_rts      = 0
// use_eop_register = 0
// sim_true_baud    = 0
// sim_char_stream  =



// ------------------
// Parameters for altera_avalon_timer named dither_timer

// always_run           = 0
// fixed_period         = 0
// snapshot             = 0
// period               = 1
// period_units         = ms
// reset_output         = 0
// timeout_pulse_output = 0
// mult                 = 0.001



// ------------------
// Parameters for altera_avalon_pio named button_pio

// has_tri              = 0
// has_out              = 0
// has_in               = 1
// edge_type            = NONE
// irq_type             = NONE
// capture              = 0
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named seven_seg_pio

// has_tri              = 0
// has_out              = 1
// has_in               = 0
// edge_type            = NONE
// irq_type             = NONE
// capture              = 0
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named misc_outs

// has_tri              = 0
// has_out              = 1
// has_in               = 0
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_timer named timer3

// always_run           = 0
// fixed_period         = 0
// snapshot             = 0
// period               = 1
// period_units         = ms
// reset_output         = 0
// timeout_pulse_output = 0
// mult                 = 0.001



// ------------------
// Parameters for altera_avalon_spi named dac_spi

// databits      = 16
// targetclock   = 6
// clockunits    = MHz
// clockmult     = 1000000
// numslaves     = 4
// ismaster      = 1
// clockpolarity = 0
// clockphase    = 0
// lsbfirst      = 0
// extradelay    = 1
// targetssdelay = 90
// delayunits    = us
// delaymult     = 1e-006
// clockunit     = kHz
// delayunit     = us
// prefix        = spi_



// ------------------
// Parameters for altera_avalon_pio named misc_ins

// has_tri              = 0
// has_out              = 0
// has_in               = 1
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_user_defined_interface named pv_unit1

// Imported_Wait           = 0
// Nios_Gen_Waits          = 1
// Synthesize_Imported_HDL = 0
// Port_Type               = Avalon Slave
// HDL_Import              = 0
// Timing_Units            = cycles
// Address_Width           = 32
// Module_List             =
// Component_Desc          =
// Component_Name          =
// Module_Name             =
// Technology              =
// Simulate_Imported_HDL   = 0
// Unit_Multiplier         = 30.003000300030003
// Setup_Value             = 0
// Hold_Value              = 0
// Wait_Value              = 1
// Show_Streaming          = 0
// Show_Latency            = 0
// File_Count              = 0
// Port_Count              = 6



// ------------------
// Parameters for altera_avalon_pio named opto_data

// has_tri              = 1
// has_out              = 0
// has_in               = 0
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named opto_control

// has_tri              = 0
// has_out              = 1
// has_in               = 0
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named axis1_int

// has_tri              = 0
// has_out              = 0
// has_in               = 1
// capture              = 1
// edge_type            = RISING
// irq_type             = EDGE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_timer named one_ms_timer

// always_run           = 1
// fixed_period         = 0
// snapshot             = 0
// period               = 1
// period_units         = ms
// reset_output         = 0
// timeout_pulse_output = 0
// mult                 = 0.001



// ------------------
// Parameters for altera_avalon_timer named timer2

// always_run           = 0
// fixed_period         = 0
// snapshot             = 1
// period               = 1
// period_units         = ms
// reset_output         = 0
// timeout_pulse_output = 0
// mult                 = 0.001



// ------------------
// Parameters for altera_avalon_timer named real_time_data_timer

// always_run           = 1
// fixed_period         = 1
// snapshot             = 0
// period               = 500
// period_units         = ms
// reset_output         = 0
// timeout_pulse_output = 0
// mult                 = 0.001



// ------------------
// Parameters for altera_avalon_user_defined_interface named pv_unit2

// Imported_Wait           = 0
// Nios_Gen_Waits          = 1
// Synthesize_Imported_HDL = 0
// Port_Type               = Avalon Slave
// HDL_Import              = 0
// Timing_Units            = cycles
// Address_Width           = 32
// Module_List             =
// Component_Desc          =
// Component_Name          =
// Module_Name             =
// Technology              =
// Simulate_Imported_HDL   = 0
// Unit_Multiplier         = 30.003000300030003
// Setup_Value             = 0
// Hold_Value              = 0
// Wait_Value              = 1
// Show_Streaming          = 0
// Show_Latency            = 0
// File_Count              = 0
// Port_Count              = 6



// ------------------
// Parameters for altera_avalon_timer named o_scope_timer

// always_run           = 0
// fixed_period         = 0
// snapshot             = 0
// period               = 1
// period_units         = ms
// reset_output         = 0
// timeout_pulse_output = 0
// mult                 = 0.001



// ------------------
// Parameters for altera_avalon_pio named axis1_div_pio

// has_tri              = 0
// has_out              = 1
// has_in               = 0
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named axis2_div_pio

// has_tri              = 0
// has_out              = 1
// has_in               = 0
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named control_int

// has_tri              = 0
// has_out              = 0
// has_in               = 1
// capture              = 1
// edge_type            = RISING
// irq_type             = EDGE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named bounceback_int

// has_tri              = 0
// has_out              = 0
// has_in               = 1
// capture              = 1
// edge_type            = RISING
// irq_type             = EDGE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named ls_int_input

// has_tri              = 0
// has_out              = 0
// has_in               = 1
// capture              = 1
// edge_type            = ANY
// irq_type             = EDGE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named cont_int_output

// has_tri              = 0
// has_out              = 1
// has_in               = 0
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named lvdt_demod1_phase

// has_tri              = 0
// has_out              = 1
// has_in               = 0
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named lvdt_demod2_phase

// has_tri              = 0
// has_out              = 1
// has_in               = 0
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named lvdt_osc_phase

// has_tri              = 0
// has_out              = 1
// has_in               = 0
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_pio named samp_interface

// has_tri              = 0
// has_out              = 1
// has_in               = 0
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE
// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000



// ------------------
// Parameters for altera_avalon_lan91c111 named lan91c111_0

// Is_Ethernet_Mac = 1
// CONSTANTS       =



// ------------------
// Parameters for altera_avalon_pio named reboot

// Do_Test_Bench_Wiring = 0
// Driven_Sim_Value     = 0x0000
// has_tri              = 0
// has_out              = 1
// has_in               = 0
// capture              = 0
// edge_type            = NONE
// irq_type             = NONE



// ------------------
// Parameters for altera_avalon_onchip_memory named enet_boot_rom

// Writeable                         = 0
// Size_Value                        = 2500
// Size_Multiple                     = 1
// Contents                          = blank
// Shrink_to_fit_contents            = 0
// use_altsyncram                    = 1
// use_ram_block_type                = M4K
// altsyncram_ram_block_type         = AUTO
// dual_port                         = 0
// allow_mram_sim_contents_only_file = 0
// CONTENTS                          = srec



// ------------------
// Parameters for altera_avalon_onchip_memory named germs_rom

// Writeable                         = 0
// Size_Value                        = 1536
// Size_Multiple                     = 1
// Contents                          = blank
// Shrink_to_fit_contents            = 0
// use_altsyncram                    = 1
// use_ram_block_type                = M4K
// altsyncram_ram_block_type         = AUTO
// dual_port                         = 0
// allow_mram_sim_contents_only_file = 0
// CONTENTS                          = srec



// ------------------
// Parameters for altera_nios_dev_kit_stratix_edition_sram2 named sram_0

// sram_memory_size  = 1024
// sram_memory_units = 1024
// sram_data_width   = 32
// MAKE              =



#ifdef __cplusplus
}
#endif

#endif //_excalibur_

// end of file
