/*

  file: dma.c

  contents: implementation of 4 simple dma routines for your
  reading pleasure

  author: david van brink \ altera corporation
  2001 december 10

*/

#include "nios.h"


// +-----------------------------
// | dma_shared does the real work; the four
// | little routines that call it just change
// | the very last parameter, which translates
// | directly into setting the rcon or wcon
// | bits in the control register.
// |
// | (rcon and wcon specify that read or write
// | address is constant, respectively.)
// |
// | These routines are all variations on
// | memcpy functionality, really.
// |
// | They're probably not really all that
// | useful in themselves, but they do provide
// | a good example of how to set up and use
// | the dma peripheral.
// |

static void dma_shared
    (
    np_dma *dma,
    int bytes_per_transfer,
    void *source_address,
    void *destination_address,
    int transfer_count,
    int mode
    )
  {
  int control_bits = 0;

  // |
  // | 1. Halt anything that's going on
  // |

  dma->np_dmacontrol = 0;

  // |
  // | 2. Set up everything but the go-bar
  // |

  dma->np_dmastatus = 0;
  dma->np_dmareadaddress = (int)source_address;
  dma->np_dmawriteaddress = (int)destination_address;
  dma->np_dmalength = transfer_count * bytes_per_transfer;

  // |
  // | 3. construct the control word...
  // |

  control_bits =
      mode        // wcon, rcon bits
      | (bytes_per_transfer &  7) // low three bits of control reg
      | ((bytes_per_transfer &  8) ? np_dmacontrol_doubleword_mask : 0)
      | ((bytes_per_transfer & 16) ? np_dmacontrol_quadword_mask : 0)
      | np_dmacontrol_leen_mask // enable length (else runs forever)
      | np_dmacontrol_go_mask;  // and... go!

  dma->np_dmacontrol = control_bits;

  // |
  // | 4. Wait til it's all done
  // |

  while((dma->np_dmastatus & np_dmastatus_busy_mask) != 0)
    ;

  return;
  }

// +-----------------------------

void nr_dma_copy_1_to_1
    (
    np_dma *dma,
    int bytes_per_transfer,
    void *source_address,
    void *destination_address,
    int transfer_count
    )
  {
  dma_shared(dma,bytes_per_transfer,source_address,destination_address,transfer_count,
      np_dmacontrol_rcon_mask | np_dmacontrol_wcon_mask);
  }


// +-----------------------------

void nr_dma_copy_1_to_range
    (
    np_dma *dma,
    int bytes_per_transfer,
    void *source_address,
    void *first_destination_address,
    int transfer_count
    )
  {
  dma_shared(dma,bytes_per_transfer,source_address,first_destination_address,transfer_count,
      np_dmacontrol_rcon_mask);
  }


// +-----------------------------

void nr_dma_copy_range_to_range
    (
    np_dma *dma,
    int bytes_per_transfer,
    void *first_source_address,
    void *first_destination_address,
    int transfer_count
    )
  {
  dma_shared(dma,bytes_per_transfer,first_source_address,first_destination_address,transfer_count,
      0);
  }


// +-----------------------------

void nr_dma_copy_range_to_1
    (
    np_dma *dma,
    int bytes_per_transfer,
    void *first_source_address,
    void *destination_address,
    int transfer_count
    )
  {
  dma_shared(dma,bytes_per_transfer,first_source_address,destination_address,transfer_count,
      np_dmacontrol_wcon_mask);
  }



// End of file.
