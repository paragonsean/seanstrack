#ifndef _UNITS_CLASS_
#define _UNITS_CLASS_

#ifndef _VISIPARM_H
typedef unsigned short BITSETYPE;
#endif

class UNITS_CLASS
    {
    private:

    int  x;
    bool rewound;

    public:

    UNITS_CLASS();
    ~UNITS_CLASS();
    int       count();
    bool      find( TCHAR * sorc );
    bool      find( short id_to_find );
    short     id();
    BITSETYPE type();
    TCHAR *   name();
    bool      next();
    void      rewind() { x = 0; rewound = true; }
    float     seconds_per_velocity_units();
    };

#endif