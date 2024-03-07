#ifndef _VISIPARM_H
#include "..\include\visiparm.h"
#endif

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

#ifndef _BITMAPCL_H_
#include "bitmapcl.h"
#endif

#undef max
#undef min

class DATA_POINT_ENTRY
    {
    public:

    int32 n;
    float sum;
    float sum2;
    float min;
    float max;

    DATA_POINT_ENTRY() { n=0; sum=0.0; sum2 = 0.0; min=0.0; max=0.0; }
    void add( float value );
    float avg();
    float sd();
    };

class DATA_ARRAY_CLASS
    {
    protected:

    int32 x;
    int32 n;
    int32 nset;
    BOOLEAN rewound;
    DATA_POINT_ENTRY * p;

    public:

    void    cleanup();
    DATA_ARRAY_CLASS() { p = 0; cleanup(); }
    ~DATA_ARRAY_CLASS() { cleanup(); }
    virtual BOOLEAN init( int32 nof_points );
    int32   left_shift( DATA_POINT_ENTRY & sorc );
    int32   left_shift( float value );
    BOOLEAN is_set() { if ( x < nset ) return TRUE; return FALSE; }
    BOOLEAN next();
    BOOLEAN next_set();
    void    rewind() { x = 0; rewound = TRUE; }
    BOOLEAN add( float value );
    BOOLEAN add( int32 i, float value );
    int32   nof_points() { return n; }
    int32   set_count() { return nset; }

    int32   n_in_avg() { if ( p ) return p[x].n;     else return 0;   }
    float   average()  { if ( p ) return p[x].avg(); else return 0.0; }
    float   stddev()   { if ( p ) return p[x].sd();  else return 0.0; }
    float   min()      { if ( p ) return p[x].min;   else return 0.0; }
    float   max()      { if ( p ) return p[x].max;   else return 0.0; }
    };

class SINGLE_PLOT_CLASS : public DATA_ARRAY_CLASS
    {
    private:

    RECT r;
    STRING_CLASS title;
    STRING_CLASS units;
    STRING_CLASS target;
    float        alarm_max;
    float        alarm_min;
    float        warning_max;
    float        warning_min;
    float        fixed_max;
    float        fixed_min;

    int32 info_box_x_position( int32 x );
    void  show_units( HDC dc );

    public:

    BITMAP_CLASS bitmap;

    SINGLE_PLOT_CLASS();
    ~SINGLE_PLOT_CLASS();
    BOOLEAN init( int32 nof_points );
    void    paint( HDC dc );
    RECT  & rect() { return r; }
    BOOLEAN restore_info_box( HDC dc );
    void    set_alarm_limits( float new_min, float new_max ) { alarm_min = new_min; alarm_max = new_max; }
    void    set_fixed_limits( float new_min, float new_max ) { fixed_min = new_min; fixed_max = new_max; }
    void    set_warning_limits( float new_min, float new_max ) { warning_min = new_min; warning_max = new_max; }
    void    set_rect( RECT newr ) { r = newr; }
    void    set_target( TCHAR * new_target );
    void    set_target( STRING_CLASS & new_target );
    void    set_title( TCHAR * s ){ title = s; }
    void    set_units( TCHAR * s ){ units = s; }
    BOOLEAN show_info_box( HDC dc, int32 x, int32 i, BOOLEAN need_bitmap );
    };

class MULTI_PLOT_CLASS
    {
    private:

    int32               n;
    SINGLE_PLOT_CLASS * sp;

    public:

    void cleanup();
    int32 count() { return n; }
    MULTI_PLOT_CLASS() { sp=0; cleanup(); }
    ~MULTI_PLOT_CLASS() { cleanup(); }
    BOOLEAN init( int32 nof_plots );
    void set_nof_points( int32 nof_points );
    SINGLE_PLOT_CLASS & operator[]( int32 i );
    };

