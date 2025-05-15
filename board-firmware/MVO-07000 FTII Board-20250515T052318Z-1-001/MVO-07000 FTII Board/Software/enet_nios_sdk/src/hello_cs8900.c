


#include "excalibur.h"
#include "cs8900.h"

#include "plugs_example_designs.h"

enum
	{
	ne_cs8900_line_status =   0x0134, // r   phy status
	};

int main(void)
	{
	int c;
	int line_status;
	int last_line_status = 0;
	int counter = 0;
	void *e;



	e = __adapter__;

	*(char *)e = 0;

	nr_cs8900_reset(e,0); // no network settings
	nr_cs8900_dump_registers(e);

	printf("press <SPACE> to dump registers again\n");

	while(1)
		{
		line_status = nr_cs8900_read_register(e,ne_cs8900_line_status);
		line_status &= 0x0080;
		if(line_status != last_line_status)
			{
			printf("Line status is: %s\n",line_status ? "up" : "down");
			last_line_status = line_status;
			}

		if(nr_uart_rxchar(0) == ' ')
			nr_cs8900_dump_registers(e);
		}

	}

