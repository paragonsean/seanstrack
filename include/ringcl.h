#ifndef _RING_CLASS_
#define _RING_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

class RING_CLASS
    {
    private:

    int32 index;
    int32 outdex;
    int32 n;

    STRING_CLASS * s;

    public:

    RING_CLASS();
    ~RING_CLASS();
    void    clear();
    BOOLEAN init( int32 max_n );
    TCHAR * pop();
    BOOLEAN push( TCHAR * sorc );
    };
#endif

