// File: hello_stdio.c
//
// Contents: examples of using fprintf and getchar
//
//

#include "excalibur.h"
#include <stdio.h>


int main(void)
	{
#ifdef nasys_printf_uart
	int OCI_host_comm = 0;

	printf ("\n\nHello from Nios.\n\n");

#ifdef nasys_oci_core
	OCI_host_comm = ( (void*)nasys_oci_core == (void*)nasys_printf_uart );
#endif

	printf( "printfs coming to you via %s.\n\n",
			( OCI_host_comm ? "JTAG/OCI" : "UART" ) );
	
	fprintf( stdout, "This is being explicitly fprintf'ed to stdout.\n" );
	fprintf( stderr, "And this is being explicitly fprintf'ed to stderr.\n\n" );

#ifdef nasys_oci_core
	if ( ! OCI_host_comm )
	{
		printf( "\nPress 'Y' to fprintf to nasys_oci_core.\n" );
		printf( "It will not return unless an OCI stdio task is open.\n" );
		printf( "Use: 'nios-run -j -t' to display the outout.\n" );
		printf( "Any other key to exit:  " );

		if ( tolower( getchar() ) != 'y' )
			goto done;
	}
	fprintf( nasys_oci_core, "\nThis is being explicitly fprintf'ed to nasys_oci_core.\n" );
#endif	

#ifdef na_uart1
	if ( OCI_host_comm )
	{
		printf( "\nPress 'Y' to fprintf to na_uart1.\n" );
		printf( "Use: 'nios-run -r -t' to display the outout.\n" );
		printf( "Any other key to exit:  " );

		if ( tolower( getchar() ) != 'y' )
			goto done;
	}
	fprintf( na_uart1, "\n\nThis is being explicitly fprintf'ed to na_uart1.\n" );
#endif

done:
	printf( "\n\nExiting.\n\n" );

#endif // #ifdef nasys_printf_uart
	}

// end of file
