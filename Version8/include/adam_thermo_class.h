#ifndef _ADAM_THERMO_CLASS_
#define _ADAM_THERMO_CLASS_

/*
-----------------------
Values are deg C x 1000
----------------------- */

class ADAM_THERMO_CLASS
    {
    public:
    unsigned value;

    ADAM_THERMO_CLASS();
    ~ADAM_THERMO_CLASS();
    void   clear();
    double deg_c();
    double deg_f();
    double deg( short units );
    bool is_set();
    bool is_not_set();
    void operator=( unsigned new_value );
    void operator=( ADAM_THERMO_CLASS & sorc );
    void operator=( TCHAR * sorc );
    void operator=( STRING_CLASS & sorc );
    bool operator==( ADAM_THERMO_CLASS & sorc );
    bool operator!=( ADAM_THERMO_CLASS & sorc );
    };

#endif
