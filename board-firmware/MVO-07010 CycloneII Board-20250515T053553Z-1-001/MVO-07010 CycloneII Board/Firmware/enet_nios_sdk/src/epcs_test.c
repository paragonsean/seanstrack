
// file: epcs_test.c
//
// Writes byte_count number of bytes from buf to epcs beginning at 0+offset.  So far sectors 0-7 are the only ones we can 
// write.  Kind of acting like we're writing an epcs4! - have to figure this out.


#include "excalibur.h"
#include "my_epcs.h"
#include "my_epcs.c"

int main(void)
    {
    int i;
    int j;
    int byte_count = 65536;
    int err;
    int t, buf=0x12345678;
	int per_line = 16;
	int offset=0x70000;

	r_epcs_protect_region (epcs_protect_none);
	//r_epcs_erase_bulk();
	r_epcs_erase_sector(offset);

    printf("starting to write a pattern into epcs...\n");
    t = nr_timer_milliseconds();
    
    j = 0;
    for(i = 0; i < byte_count; i+=4)
        {
        //if(err = r_epcs_write_byte (i , 0x55)) printf("Error during write, addr 0x%x\n", i);
		if(err=r_epcs_write_buffer (i+offset, 4, (unsigned char *)&buf)) printf("Error during write, addr 0x%x\n", i);
        j += 17;
        j &= 0x000000ff;
        }

    t = nr_timer_milliseconds() - t;

    printf("all done. writing %d bytes to epcs took %d milliseconds\n\n",
            byte_count,
            t);
    printf("(note: this is using the slowest nonbulk byte-by-byte write method.)\n\n");


	// Read what we wrote
	printf("Now reading back what we wrote\n");
	
	for(i = 0; i < byte_count; i+= per_line)
        {
        printf("%08x : ",i+offset);
        for(j = i; j < i + per_line; j+=4)
            {
            unsigned char b;

			// For byte reads
			//err = r_epcs_read_byte(j,&b);
			//printf("%02x ",b);

			// For buffer reads
			err = r_epcs_read_buffer (j+offset, 4, (unsigned char *)&buf);
			printf("%08x ",buf);
            }
        printf("| ");
        for(j = i; j < i + per_line; j++)
            {
            unsigned char b;
            err = r_epcs_read_byte(j+offset,&b);
            printf("%c",(b > 0x20 && b < 0x7f) ? b : '.');
            }
        printf("\n");
		//nr_delay(50);
        }

    return 0;
    }

// end of file
