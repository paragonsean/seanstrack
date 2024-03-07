#ifndef _MULTIPART_RUNLIST_CLASS_
#define _MULTIPART_RUNLIST_CLASS_

struct PART_SELECT_WIRES
{
STRING_CLASS pa;
unsigned     wires;
};

class MULTIPART_RUNLIST_CLASS
{
public:

BOOLEAN             is_enabled;
unsigned            mask;
MACHINE_NAME_ENTRY  ma;
int                 n;
PART_SELECT_WIRES * psw;

MULTIPART_RUNLIST_CLASS();
~MULTIPART_RUNLIST_CLASS();
BOOLEAN get( STRING_CLASS & cn, STRING_CLASS & mn );
STRING_CLASS & part_to_run( unsigned wire_bits );
};

#endif
