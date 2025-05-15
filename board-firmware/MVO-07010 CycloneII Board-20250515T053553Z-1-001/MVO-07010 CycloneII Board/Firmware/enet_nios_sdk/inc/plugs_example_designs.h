// file: plugs_example_designs.h
//
// This header file sets a few #defines depending
// which example design you're building it for.
// It only recognizes the names of peripheral
// actually in our example designs, but at least
// lets the same example software work for
// them.
//
// It should be clear enough what you need to change
// for your design...
//
// -- dvb@altera.com 2002.07.16
//
// ex:set tabstop=4:
//

#ifndef _plugs_example_designs_

#include "excalibur.h"

#if defined(na_cs8900)

	// Single cs8900 10BaseT design

	#define __adapter__ na_cs8900
	#define __adapter_irq__ na_cs8900_irq
	#define __adapter_struct_addr__ &ng_cs8900
	#include "cs8900.h"

#elif defined(na_cs8900_0)

	// Stacked cs8900 10BaseT design

	#if PLUGS_USE_TOP // reverse the two adapters?
		#define __adapter__ na_cs8900_1
		#define __adapter_irq__ na_cs8900_1_irq
		#define __adapter2__ na_cs8900_0
		#define __adapter2_irq__ na_cs8900_0_irq
	#else
		#define __adapter__ na_cs8900_0
		#define __adapter_irq__ na_cs8900_0_irq
		#define __adapter2__ na_cs8900_1
		#define __adapter2_irq__ na_cs8900_1_irq
	#endif

	#define __adapter_struct_addr__ &ng_cs8900
	#include "cs8900.h"

#elif defined(na_lan91c111)

	// Single lan91c111 100BaseT design

	#define __adapter__ na_lan91c111
	#define __adapter_irq__ na_lan91c111_irq
	#define __adapter_struct_addr__ &ng_lan91c111
	#include "lan91c111.h"

#elif defined(na_lan91c111_0)

	// Stacked lan91c111 100BaseT design

	#if PLUGS_USE_TOP // reverse the two adapters?
		#define __adapter__ na_lan91c111_1
		#define __adapter_irq__ na_lan91c111_1_irq
		#define __adapter2__ na_lan91c111_0
		#define __adapter2_irq__ na_lan91c111_0_irq
	#else
		#define __adapter__ na_lan91c111_0
		#define __adapter_irq__ na_lan91c111_0_irq
		#define __adapter2__ na_lan91c111_1
		#define __adapter2_irq__ na_lan91c111_1_irq
	#endif

	#define __adapter_struct_addr__ &ng_lan91c111
	#include "lan91c111.h"

#else
	
    //
    // We used to do a #error here, but now the plugs
    // library actually uses this file for plugs_dhcp.c, and
    // may be compiled inadvertenly for just about any
    // Nios/SOPC-Builder system.
    //
    // So, we just ignore it here
    // 

#endif

#endif // _plugs_example_designs_

// End Of File
