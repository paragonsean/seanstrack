# File: make_quartus_sw_project.tcl
#
# This is a machine-generated script to
# populate a Quartus Software Mode Project
# with the appropriate files.
#
# Generated: 2008.04.28 16:15:15
# CPU: enet_nios
# Toolchain: gnu
# Quartus Project: emc_chip_12010200
# SDK Directory: D:/qdesigns_5_1/CycloneII/emc_chip_12010201/enet_nios_sdk
#

puts "#"
puts "# This script is setting up Quartus Software Mode"
puts "# CPU: enet_nios"
puts "# Toolchain: gnu"
puts "# Quartus Project: emc_chip_12010200"
puts "# SDK Directory : D:/qdesigns_5_1/CycloneII/emc_chip_12010201/enet_nios_sdk"
puts "#"

#
# If the sdk_directory doesn't exist, this tcl script
# was probably built on a different computer
#

if {![file exists "D:/qdesigns_5_1/CycloneII/emc_chip_12010201/enet_nios_sdk"]} then {
		puts "#=========================================="
		puts "# ERROR: Directories appear to have moved!"
		puts "#        You must regenerate the SDK"
		puts "#        in SOPC Builder for this script"
		puts "#        to work on this computer."
		puts "#"
		return -1
	}

# First, remove all software assignments


puts "# Removing all software files from project"

set a [project get_all_assignments "" "" "" ""]
foreach z $a {
	set z3 [lindex $z 3]
	set z4 [lindex $z 4]
	if {$z3 == "C_FILE" 
		|| $z3 == "CPP_FILE"
		|| $z3 == "ASM_FILE"
		|| $z3 == "CPP_INCLUDE_FILE"} then {
		project remove_assignment "" "" "" "" $z3 $z4
		}
	}


# Then, add back in the new ones

puts "# Adding in library and header files to project..."


project add_assignment "" "" "" "" "CPP_INCLUDE_FILE" "enet_nios_sdk/inc/excalibur.h";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/inc/excalibur.s";
project add_assignment "" "" "" "" "CPP_INCLUDE_FILE" "enet_nios_sdk/inc/lan91c111.h";
project add_assignment "" "" "" "" "CPP_INCLUDE_FILE" "enet_nios_sdk/inc/nios_cache.h";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/inc/nios_macros.s";
project add_assignment "" "" "" "" "CPP_INCLUDE_FILE" "enet_nios_sdk/inc/pio_lcd16207.h";
project add_assignment "" "" "" "" "CPP_INCLUDE_FILE" "enet_nios_sdk/inc/plugs.h";
project add_assignment "" "" "" "" "CPP_INCLUDE_FILE" "enet_nios_sdk/inc/plugs_dhcp.h";
project add_assignment "" "" "" "" "CPP_INCLUDE_FILE" "enet_nios_sdk/inc/plugs_example_designs.h";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/asmi.c";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/jtag_rxchar.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/jtag_tx_ready.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/jtag_txchar.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/jtag_txcr.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/jtag_txhex.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/jtag_txhex16.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/jtag_txhex32.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/jtag_txstring.s";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/lan91c111.c";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_atexit.s";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/nios_cache.c";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_close.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_copyrange.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_cstubs.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_cwpmanager.s";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/nios_debug.c";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_delay.s";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/nios_fprintf.c";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_fstat.s";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/nios_gdb_stub.c";
project add_assignment "" "" "" "" "CPP_INCLUDE_FILE" "enet_nios_sdk/lib/nios_gdb_stub.h";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/nios_gdb_stub_io.c";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_gdb_stub_isr.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_getctlreg.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_getpid.s";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/nios_gprof.c";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_isatty.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_isrmanager.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_jumptoreset.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_jumptostart.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_kill.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_lseek.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_math1.s";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/nios_more_cstubs.c";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/nios_premain_hook.c";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/nios_printf.c";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_sbrk.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_setjmp.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_setup.s";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/nios_sprintf.c";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/nios_uart.c";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/nios_zerorange.s";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/pio_lcd16207.c";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/pio_showhex.c";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/plugs.c";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/plugs_dhcp.c";
project add_assignment "" "" "" "" "C_FILE" "enet_nios_sdk/lib/plugs_print.c";
project add_assignment "" "" "" "" "CPP_INCLUDE_FILE" "enet_nios_sdk/lib/plugs_private.h";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/spi_rxchar.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/spi_txchar.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/timer_milliseconds.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/uart_rxchar.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/uart_txchar.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/uart_txcr.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/uart_txhex.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/uart_txhex16.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/uart_txhex32.s";
project add_assignment "" "" "" "" "ASM_FILE" "enet_nios_sdk/lib/uart_txstring.s";


# Setup for gnu

puts "# Setting build options..."

#
# Roundabout way to remove the
# "Release" target
#

if {![project swb_exists Release]} {
	project create_swb Release;
}
project set_active_swb Release;
project remove_assignment "" "" "" "" "SOFTWARE_SETTINGS" "Release"

#
# Software Build Assignments for Debug
# Make sure there is a Debug target
#

if {![project swb_exists Debug]} {
	project create_swb Debug;
}
project set_active_swb Debug;


swb add_assignment "" "" "" "BYTE_ORDER" "LITTLE ENDIAN"
swb add_assignment "" "" "" "DO_POST_BUILD_COMMAND_LINE" "Off"
swb add_assignment "" "" "" "GNUPRO_NIOS_ASM_COMMAND_LINE" "-I. -ID:/qdesigns_5_1/CycloneII/emc_chip_12010201/enet_nios_sdk/inc -ID:/qdesigns_5_1/CycloneII/emc_chip_12010201/enet_nios_sdk/lib -ID:/qdesigns_5_1/CycloneII/emc_chip_12010201/enet_nios_sdk/src --defsym __nios32__=1"
swb add_assignment "" "" "" "GNUPRO_NIOS_CPP_COMMAND_LINE" "-g -I. -ID:/qdesigns_5_1/CycloneII/emc_chip_12010201/enet_nios_sdk/inc -ID:/qdesigns_5_1/CycloneII/emc_chip_12010201/enet_nios_sdk/lib -ID:/qdesigns_5_1/CycloneII/emc_chip_12010201/enet_nios_sdk/src -D__nios32__=1"
swb add_assignment "" "" "" "GNUPRO_NIOS_LINK_COMMAND_LINE" "-g -L C:/altera/quartus51/sopc_builder/bin/nios-gnupro/nios-elf/lib/m32 -L C:/altera/quartus51/sopc_builder/bin/nios-gnupro/lib/gcc-lib/nios-elf/2.9-nios-010801-20030718/m32 -lc -lgcc -T C:/altera/quartus51/sopc_builder/bin/excalibur.ld"
swb add_assignment "" "" "" "OUTPUT_FILE_NAME" "Debug/emc_chip_12010200_application.srec"
swb add_assignment "" "" "" "OUTPUT_TYPE" "S-records"
swb add_assignment "" "" "" "PROCESSOR" "Nios 32 bit"
swb add_assignment "" "" "" "PROGRAMMING_FILE_TYPE" "FLASH PROGRAMMING FILE"
swb add_assignment "" "" "" "TOOLSET" "GNUPro for Nios"
swb add_assignment "" "" "" "USE_C_PREPROCESSOR_FOR_GNU_ASM_FILES" "OFF"


puts "# Added 68 library and header files."
puts "# To reduce code footprint, you may remove unused libraries manually."
puts "# (You must still provide a main routine.)"
puts "# Bye."

# End of file
