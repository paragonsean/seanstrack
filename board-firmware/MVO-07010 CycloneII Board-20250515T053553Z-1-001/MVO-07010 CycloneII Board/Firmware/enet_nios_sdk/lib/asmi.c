//TODO: Increase address from 2 bytes to 3
#include "excalibur.h"

//wait until all the data is shifted out
void nr_asmi_wait_tx_complete ()
{
    while (!(na_asmi->np_asmistatus & np_asmistatus_tmt_mask));
}

//set the chip-select
void nr_asmi_set_cs ()
{
    na_asmi->np_asmicontrol = np_asmicontrol_sso_mask;
}

//clear the chip-select
void nr_asmi_clear_cs ()
{
    int value;
    nr_asmi_wait_tx_complete (na_asmi);

    //clear only the sso bit
    value = na_asmi->np_asmicontrol;
    value = value & (~np_asmicontrol_sso_mask);

    na_asmi->np_asmicontrol = value;
}

//send 8 bits to the memory
void nr_asmi_txchar (unsigned char data)
{
    while (!(na_asmi->np_asmistatus & np_asmistatus_trdy_mask));
    na_asmi->np_asmitxdata = data;
}

//receive 8 bits from the memory
unsigned char nr_asmi_rxchar ()
{
    while (!(na_asmi->np_asmistatus & np_asmistatus_rrdy_mask));
    return (na_asmi->np_asmirxdata);
}

//read the memories status register
unsigned char nr_asmi_read_status ()
{
    unsigned char value;
    // set chip select
    nr_asmi_set_cs ();

    // send READ command
    nr_asmi_txchar (na_asmi_rdsr);
    nr_asmi_rxchar ();   // throw out the garbage
    // read the byte
    nr_asmi_txchar (0); // send out some garbage while were reading
    value = nr_asmi_rxchar ();

    // clear chip select
    nr_asmi_clear_cs ();

    return (value);
}

//wait until the write operation is finished
int nr_asmi_wait_write_complete ()
{
    volatile unsigned long int i=0;

    // Compute a maximum timeout value for all callers of this function.
    // This value is computed for a maximum timeout of 10s (EPCS4 bulk
    // erase).  The calculation goes as follows:
    //
    // maximum ASMI clock rate: 20MHz
    // read status time: 16 clocks + 1/2 clock period on either side =
    //   17/20M = 850ns
    // 10s / 850ns = 11,764,706 read status commands.
    const unsigned long i_timeout=11770000;

    while ((nr_asmi_read_status() & na_asmi_wip) && (i<i_timeout))
    {
        i++;
    }
    if (i >= i_timeout)
        return (na_asmi_err_timedout);
    else
        return (na_asmi_success);
}

//returns true if the chip is present else false
int nr_asmi_is_device_present ()
{
    volatile int i;

    // set chip select
    nr_asmi_set_cs ();
    // send WRITE ENABLE command
    nr_asmi_txchar (na_asmi_wren);
    nr_asmi_rxchar ();  // throw out the garbage
    //clear chip select to set write enable latch
    nr_asmi_clear_cs ();
    // some delay between chip select change
    for (i=0;i<20;i++);

    //not check the value
    i = nr_asmi_read_status ();
    if (i & na_asmi_wel)
    {
        // previous write worked,
        // but if the pins are floating,
        // we don't know what the value might be,
        // so lets switch values and check again
        // set chip select
        nr_asmi_set_cs ();
        // send WRITE DISABLE command
        nr_asmi_txchar (na_asmi_wrdi);
        nr_asmi_rxchar ();  // throw out the garbage
        //clear chip select to set write enable latch
        nr_asmi_clear_cs ();
        // some delay between chip select change
        for (i=0;i<20;i++);

        //now check the value again
        i = nr_asmi_read_status ();
        if ((i & na_asmi_wel) == 0)
            return (1);
    }
    //if we made it this far then the device is not present
    return (0);
}

//returns the lowest protected address
unsigned long nr_asmi_lowest_protected_address()
{
    int bp;

    //verify device presence
    if (!nr_asmi_is_device_present())
        return (na_asmi_err_device_not_present);

    bp = nr_asmi_read_status () & na_asmi_bp;
    switch (bp)
    {
#if ((na_asmi_64K) || (na_asmi_1M))
        case na_asmi_protect_all:
            return (0);
        default:
            return (na_asmi_bulk_size - (na_asmi_sector_size * (bp>>2)));
#else
        case na_asmi_protect_none:
        case na_asmi_protect_top_eighth:
        case na_asmi_protect_top_quarter:
            return (na_asmi_bulk_size - (na_asmi_sector_size * (bp>>2)));
        case na_asmi_protect_top_half:
            return (na_asmi_bulk_size >> 1);
        default:
            return (0);
#endif
    }
    //this should never happen, but to make the compiler happy...
    return (0);
}

int nr_asmi_write_status (unsigned char value)
{
    volatile int i;

    //verify device presence
    if (!nr_asmi_is_device_present())
        return (na_asmi_err_device_not_present);

    // set chip select
    nr_asmi_set_cs ();
    // send WRITE ENABLE command
    nr_asmi_txchar (na_asmi_wren);
    nr_asmi_rxchar ();  // throw out the garbage
    // clear chip select to set write enable latch
    nr_asmi_clear_cs ();
    // some dealy between chip select change
    for (i=0; i<20; i++);

    // set chip select
    nr_asmi_set_cs ();

    // send WRSR command
    nr_asmi_txchar (na_asmi_wrsr);
    nr_asmi_rxchar ();   // throw out the garbage
    // write the byte
    nr_asmi_txchar (value);
    nr_asmi_rxchar ();

    // clear chip select
    nr_asmi_clear_cs ();

    //now wait until the write operation is finished
    if (i=nr_asmi_wait_write_complete ())
        return (i);

    //now verify that the write was successful
    i = nr_asmi_read_status ();
    if (i != (int)value) return (na_asmi_err_write_failed);

    return (na_asmi_success);
}

//set the status register to protect the selected region
int nr_asmi_protect_region (int bpcode)
{
    unsigned char value;

    //verify device presence
    if (!nr_asmi_is_device_present())
        return (na_asmi_err_device_not_present);

    value = nr_asmi_read_status ();
    value = value & (!na_asmi_bp);
    value |= bpcode;
    return (nr_asmi_write_status (value));
}

//read 1 byte from the memory
int nr_asmi_read_byte (unsigned long address, unsigned char *data)
{
    //verify device presence
    if (!nr_asmi_is_device_present())
        return (na_asmi_err_device_not_present);

    // set chip select
    nr_asmi_set_cs ();

    // send READ command
    nr_asmi_txchar (na_asmi_read);
    nr_asmi_rxchar ();
    // send high byte of address
    nr_asmi_txchar ((address >> 16)&0x000000ff);
    nr_asmi_rxchar ();
    // send middle byte of address
    nr_asmi_txchar ((address >> 8)&0x000000ff);
    nr_asmi_rxchar ();
    // send low byte of address
    nr_asmi_txchar (address&0x000000ff);
    nr_asmi_rxchar ();
    // read the byte
    nr_asmi_txchar (0); // send out some garbage while were reading
    *data = (unsigned char) nr_asmi_rxchar ();

    // clear chip select
    nr_asmi_clear_cs ();

    return (na_asmi_success);
}

//write 1 byte to the memroy
int nr_asmi_write_byte (unsigned long address, unsigned char data)
{
    volatile int i;
    unsigned char verify;

    //verify device presence
    if (!nr_asmi_is_device_present())
        return (na_asmi_err_device_not_present);

    // make sure the address is writable
    if (address >= nr_asmi_lowest_protected_address())
        return (na_asmi_err_write_failed);

    // set chip select
    nr_asmi_set_cs ();
    // send WRITE ENABLE command
    nr_asmi_txchar (na_asmi_wren);
    nr_asmi_rxchar ();  // throw out the garbage
    // clear chip select to set write enable latch
    nr_asmi_clear_cs ();
    //maybe we need some delay here?
    for (i=0; i<20; i++);

    //set chip select
    nr_asmi_set_cs ();
    // send WRITE command
    nr_asmi_txchar (na_asmi_write);
    nr_asmi_rxchar ();   // throw out the garbage
    // send high byte of address
    nr_asmi_txchar ((address >> 16)&0x000000ff);
    nr_asmi_rxchar ();   // throw out the garbage
    // send middle byte of address
    nr_asmi_txchar ((address >> 8)&0x000000ff);
    nr_asmi_rxchar ();   // throw out the garbage
    // send low byte of address
    nr_asmi_txchar (address&0x000000ff);
    nr_asmi_rxchar ();   // throw out the garbage
    // send the data
    nr_asmi_txchar (data);
    nr_asmi_rxchar ();   // throw out the garbage

    // clear chip select to set complete the write cycle
    nr_asmi_clear_cs ();

    //now wait until the write operation is finished
    if (i=nr_asmi_wait_write_complete ())
        return (i);

    //now verify that the write was successful
    nr_asmi_read_byte (address, &verify);
    if (verify != data) return (na_asmi_err_write_failed);

    return (na_asmi_success);
}

//erase sector which contains address
int nr_asmi_erase_sector (unsigned long address)
{
    volatile int i;

    //verify device presence
    if (!nr_asmi_is_device_present())
        return (na_asmi_err_device_not_present);

    // make sure the address is writable
    if (address >= nr_asmi_lowest_protected_address())
        return (na_asmi_err_write_failed);

    // set chip select
    nr_asmi_set_cs ();
    // send WRITE ENABLE command
    nr_asmi_txchar (na_asmi_wren);
    nr_asmi_rxchar ();  // throw out the garbage
    // clear chip select to set write enable latch
    nr_asmi_clear_cs ();
    //maybe we need some delay here?
    for (i=0; i<20; i++);

    //set chip select
    nr_asmi_set_cs ();
    // send SE command
    nr_asmi_txchar (na_asmi_se);
    nr_asmi_rxchar ();   // throw out the garbage
    // send high byte of address
    nr_asmi_txchar ((address >> 16)&0x000000ff);
    nr_asmi_rxchar ();   // throw out the garbage
    // send middle byte of address
    nr_asmi_txchar ((address >> 8)&0x000000ff);
    nr_asmi_rxchar ();   // throw out the garbage
    // send low byte of address
    nr_asmi_txchar (address&0x000000ff);
    nr_asmi_rxchar ();   // throw out the garbage

    // clear chip select to complete the write cycle
    nr_asmi_clear_cs ();

    //now wait until the write operation is finished
    if (i=nr_asmi_wait_write_complete ())
        return (i);

    return (na_asmi_success);
}

//erase entire chip
int nr_asmi_erase_bulk ()
{
    volatile int i;

    //verify device presence
    if (!nr_asmi_is_device_present())
        return (na_asmi_err_device_not_present);

    // make sure the address is writable
    if (nr_asmi_lowest_protected_address() != na_asmi_bulk_size)
        return (na_asmi_err_write_failed);

    // set chip select
    nr_asmi_set_cs ();
    // send WRITE ENABLE command
    nr_asmi_txchar (na_asmi_wren);
    nr_asmi_rxchar ();  // throw out the garbage
    // clear chip select to set write enable latch
    nr_asmi_clear_cs ();
    //maybe we need some delay here?
    for (i=0; i<20; i++);

    //set chip select
    nr_asmi_set_cs ();
    // send BE command
    nr_asmi_txchar (na_asmi_be);
    nr_asmi_rxchar ();   // throw out the garbage

    // clear chip select to complete the write cycle
    nr_asmi_clear_cs ();

    //now wait until the write operation is finished
    if (i=nr_asmi_wait_write_complete ())
        return (i);

    return (na_asmi_success);
}

//read buffer from memory
int nr_asmi_read_buffer (unsigned long address, int length,
            unsigned char *data)
{
    volatile int i;

    //verify device presence
    if (!nr_asmi_is_device_present())
        return (na_asmi_err_device_not_present);

    // set chip select
    nr_asmi_set_cs ();

    // send READ command
    nr_asmi_txchar (na_asmi_read);
    nr_asmi_rxchar ();
    // send high byte of address
    nr_asmi_txchar ((address >> 16)&0x000000ff);
    nr_asmi_rxchar ();
    // send middle byte of address
    nr_asmi_txchar ((address >> 8)&0x000000ff);
    nr_asmi_rxchar ();
    // send low byte of address
    nr_asmi_txchar (address&0x000000ff);
    nr_asmi_rxchar ();
    while (length-- > 0)
    {
        // read the byte
        nr_asmi_txchar (0); // send out some garbage while were reading
        *data++ = nr_asmi_rxchar ();
    }

    // clear chip select
    nr_asmi_clear_cs ();

    return (na_asmi_success);
}
//write page to memory
int nr_asmi_write_page (unsigned long address, int length,
            unsigned char *data)
{
    volatile int i;
    unsigned char verify[na_asmi_page_size];
    int my_length = length;
    unsigned char *my_data = data;

    //verify device presence
    if (!nr_asmi_is_device_present())
        return (na_asmi_err_device_not_present);

    // set chip select
    nr_asmi_set_cs ();
    // send WRITE ENABLE command
    nr_asmi_txchar (na_asmi_wren);
    nr_asmi_rxchar ();  // throw out the garbage
    // clear chip select to set write enable latch
    nr_asmi_clear_cs ();
    //maybe we need some delay here?
    for (i=0; i<20; i++);

    //set chip select
    nr_asmi_set_cs ();
    // send WRITE command
    nr_asmi_txchar (na_asmi_write);
    nr_asmi_rxchar ();   // throw out the garbage
    // send high byte of address
    nr_asmi_txchar ((address >> 16)&0x000000ff);
    nr_asmi_rxchar ();   // throw out the garbage
    // send middle byte of address
    nr_asmi_txchar ((address >> 8)&0x000000ff);
    nr_asmi_rxchar ();   // throw out the garbage
    // send low byte of address
    nr_asmi_txchar (address&0x000000ff);
    nr_asmi_rxchar ();   // throw out the garbage
    // send the data
    while (my_length-- > 0)
    {
        // write the byte
        nr_asmi_txchar (*my_data++);
        nr_asmi_rxchar ();   // throw out the garbage
    }

    // clear chip select to complete the write cycle
    nr_asmi_clear_cs ();

    //now wait until the write operation is finished
    if (i=nr_asmi_wait_write_complete ())
        return (i);

    //verify that the data was correctly written
    nr_asmi_read_buffer (address, length, verify);
    for (i=0; i<length; i++)
    {
        if (verify[i] != data[i]) return (na_asmi_err_write_failed);
    }
    return (na_asmi_success);
}

//write buffer to memroy
int nr_asmi_write_buffer (unsigned long address, int length,
            unsigned char *data)
{
    volatile int i;
    int status;

    //verify device presence
    if (!nr_asmi_is_device_present())
        return (na_asmi_err_device_not_present);

    // make sure the full address range is writable
    if ((address >= nr_asmi_lowest_protected_address()) ||
        ((address+length-1) >= nr_asmi_lowest_protected_address()))
        return (na_asmi_err_write_failed);

    //send partial first page (if there is one)
    i = address % na_asmi_page_size;
    if ((i != 0) && (i+length > na_asmi_page_size))
    {
        i = na_asmi_page_size-i;
        if (status = nr_asmi_write_page (address, i, data))
            return (status); //write failed
        address += i;
        data += i;
        length -= i;
    }

    //send all full pages
    while (length/na_asmi_page_size > 0)
    {
        if (status = nr_asmi_write_page (address, na_asmi_page_size, data))
            return (status); //write failed
        address += na_asmi_page_size;
        data += na_asmi_page_size;
        length -= na_asmi_page_size;
    }

    //send partial last page (if there is one)
    if (length > 0)
    {
        if (status = nr_asmi_write_page (address, length, data))
            return (status); //write failed
    }

    return (na_asmi_success);
}

#define NR_ASMI_BITREVERSE(x) \
  ((((x) & 0x80) ? 0x01 : 0) | \
   (((x) & 0x40) ? 0x02 : 0) | \
   (((x) & 0x20) ? 0x04 : 0) | \
   (((x) & 0x10) ? 0x08 : 0) | \
   (((x) & 0x08) ? 0x10 : 0) | \
   (((x) & 0x04) ? 0x20 : 0) | \
   (((x) & 0x02) ? 0x40 : 0) | \
   (((x) & 0x01) ? 0x80 : 0))

int nr_asmi_address_past_config (unsigned long *addr)
{
    unsigned long i;
    int j;
    unsigned char value;
    unsigned char buf[4];
    int err;

    if (err=nr_asmi_read_buffer(0, sizeof(buf) / sizeof(*buf), buf))
        return (err);

    for (i=0; i<na_asmi_bulk_size - 8; i++)
    {
        if (err=nr_asmi_read_byte (i, &value))
            return (err);
        if (value == NR_ASMI_BITREVERSE(0x6A)) break;
        if (value != 0xFF) return (na_asmi_invalid_config);
    }

    //if we haven't seen any data by the 64th byte,
    //then it doesn't look like there's any configuration data
    if (i >= na_asmi_bulk_size - 8) return (na_asmi_invalid_config);

    // If we made it this far, we found the 0x6A byte at address i.  Beyond that,
    // we expect an 8-byte "option register", of which the last 4 bytes are the
    // length, LS-byte first.
    i += 5; // Jump ahead to the length.

    // Read the 4 bytes of the length.
    if (err=nr_asmi_read_buffer(i,4,buf))
        return (err);

    // Compute the length.
    *addr = 0;
    for (j = 3; j != ~0; --j)
    {
        *addr <<= 8;
        *addr |= NR_ASMI_BITREVERSE(buf[j]);
    }
    // The last address, oddly enough, is in bits.
    // Convert to bytes, rounding up.
    *addr += 7;
    *addr /= 8;

    return (na_asmi_success);
}


