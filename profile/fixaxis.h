#ifndef _FIXED_AXIS_CLASS_
#define _FIXED_AXIS_CLASS_

#define FIXAXIS_HEAD_PRES_INDEX 0
#define FIXAXIS_ROD_PRES_INDEX  1
#define FIXAXIS_DIFF_PRES_INDEX 8
#define FIXAXIS_VELOCITY_INDEX  9

class FIXED_AXIS_ENTRY
    {
    public:

    BOOLEAN enabled;
    float min;
    float max;

    FIXED_AXIS_ENTRY() { enabled = FALSE; min = 0.0; max = 0.0; }
    ~FIXED_AXIS_ENTRY() {}
    BOOLEAN read( DB_TABLE & t );
    BOOLEAN write( DB_TABLE & t );
    void operator=( const FIXED_AXIS_ENTRY & sorc );
    };

class FIXED_AXIS_CLASS
    {
    protected:

    int32               n;
    FIXED_AXIS_ENTRY  * f;

    public:
    void empty();
    FIXED_AXIS_CLASS() { f = 0; n = 0; }
    ~FIXED_AXIS_CLASS() { empty(); }
    int32   count() { return n; }
    BOOLEAN init( int32 nof_entries );
    BOOLEAN setsize( int32 nof_entries );
    BOOLEAN read( TCHAR * filename );
    BOOLEAN write(TCHAR * filename );
    FIXED_AXIS_ENTRY & operator[]( int32 i );
    };

#endif

