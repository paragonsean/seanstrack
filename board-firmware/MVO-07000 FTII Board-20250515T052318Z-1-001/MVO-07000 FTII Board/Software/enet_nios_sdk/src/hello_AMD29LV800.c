#include "nios_map.h"
#include "nios_peripherals.h"

/*
This test program erases the entire flash, then writes
random data over the first 64K of flash memory space,
and verifies the written data.
*/
int main(void)
{
#ifdef __nios32__
  const int size = 0x4000 / sizeof(unsigned short);
#else
  // The sector based at 0x4000 in the AMD29LV800 is only 0x2000 bytes.
  const int size = 0x2000 / sizeof(unsigned short);
#endif

  unsigned short buf[size];
  int i;

#ifdef nasys_main_flash
  unsigned short *flash_base = nasys_main_flash;
#else
  // Odd, no main flash.  Try 0.  That probably won't work!
  unsigned short *flash_base = 0;
#endif // nasys_main_flash

  printf ("\n\nNios Flash Test.\n\n");

  printf("erasing flash sector 0 [0x%X, 0x%X)\n",
    (int)flash_base, (int)flash_base + sizeof(unsigned short) * size);

  // Erase flash

  if (!nr_flash_erase_sector(flash_base, flash_base))
  {
    printf("couldn't erase flash sector at 0x%X\n", flash_base);
  }
  else
  {
    printf("erased flash sector at 0x%X.\n", flash_base);
  }

  printf("writing flash...\n");
  srand(0x0440);
  for (i = 0; i < size; ++i)
  {
#ifdef __nios32__
    buf[i] = rand() >> 16;
#else
    buf[i] = (rand() & 0xFF00) | (rand() >> 8);
#endif
  }

  if (!nr_flash_write_buffer(flash_base, flash_base, buf, size))
  {
    printf("failed to write flash.\n");
    return 0;
  }

  printf("reading flash.\n");

  for (i = 0; i < size; ++i)
  {
    if (*((unsigned short*)flash_base + i) != buf[i])
    {
      printf("verify failure at location %d\n", i);
    }
  }

  printf("done reading flash\n");

  printf("goodbye.\n\004");
}

