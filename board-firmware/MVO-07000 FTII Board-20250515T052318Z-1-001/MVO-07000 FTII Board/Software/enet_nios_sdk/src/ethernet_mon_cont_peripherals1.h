// 				  ethernet_mon_cont_peripherals1.h

//				Copyright 2001,2002 Visi-Trak Worldwide 





// Digital I/O structure definition.
typedef volatile struct
{
short unsigned int mon_dio; 	// Read / write	16 bit value	
short unsigned int cont_dio; 	// Read / write	16 bit value	
} vp_dio; /* Digital I/O interface */



// Rename nios.h generated definitions.
#define va_opto_io_interface ((vp_dio * ) 0x00000430)



