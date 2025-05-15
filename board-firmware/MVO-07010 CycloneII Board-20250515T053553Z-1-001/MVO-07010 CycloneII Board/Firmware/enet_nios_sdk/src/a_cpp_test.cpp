// file: a_cpp_test.cpp
//
// dvb@altera.com, 2000 October
//
// This is a simple program that
// checks c++ functionality, at
// least a little.
//
// Specifically, it verifies
// that:
//     1. a foo.cpp file compiles at all
//     2. that the "static initializers" are called.
//
// A static initializer is the call to
// "new" or other constructor
// function for a class which gets
// called *before* main() for statically
// allocated instances of the class.
//
#include <stdio.h>
#include "excalibur.h"


// -------------------------------
// Class definition

class c_trivial
	{
public:
	c_trivial(void)
		{
		c_trivial::m_instance_count++;
		}
	
	static int m_instance_count;

	static int get_instance_count(void)
		{
		return c_trivial::m_instance_count;
		}

	};

// -------------------------------
// Declaration for static members of class

int c_trivial::m_instance_count = 0;


// -------------------------------
// Statically declared instances of
// the class: this should cause the
// constructor function to be called
// before main.


c_trivial o_t1;
c_trivial o_t2;

// -------------------------------
// the main routine, which ascertains
// if the static constructors got called.

int main(void)
	{
	c_trivial o_t3;
	int c;

	#if NIOS_GDB
		nios_gdb_install(1);
		nios_gdb_breakpoint();
	#endif

	c = c_trivial::get_instance_count();

	printf("\n\n\n");

	printf("c_trivial::m_instance_count is %d\n",c);
	
	printf("\n\n\n");

	switch(c)
		{
		case 1:
			printf("   Static constructor NOT called.\n");
			printf("   This is expected only if static constructors\n");
			printf("   are turned off in the runtime library\n");
			printf("   to make a smaller footprint.\n");
			break;

		case 3:
			printf("   Static constructor called.\n");
			printf("   This is expected,\n");
			printf("   unless turned off in the runtime library\n");
			printf("   to make a smaller footprint.\n");
			break;

		default:
			printf("   It appears something is broken...\n");
			break;
		}
	printf("\n\n\n");
	}

