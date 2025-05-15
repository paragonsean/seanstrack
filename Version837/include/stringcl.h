#ifndef _STRING_CLASS_
#define _STRING_CLASS_

#ifndef _VISIPARM_H
typedef long int int32;
#endif

#ifndef _SUBS_H_
TCHAR * ascii_double( double sorc );
int32   countchars( TCHAR c, const TCHAR * sorc );
void    matchcase( TCHAR * dest, const TCHAR * sorc );
float   extfloat( const TCHAR * sorc );
double  extdouble( const TCHAR * sorc );
unsigned long asctoul( const TCHAR * sorc );
int32   asctoint32( const TCHAR * sorc );
TCHAR * int32toasc( int32 n );
TCHAR * ultoascii( unsigned long n );
#endif

class STRING_CLASS
    {
    private:

    int32   maxlen;
    TCHAR * s;
    static TCHAR emptystring[];

    public:

    STRING_CLASS();
    STRING_CLASS( const TCHAR * sorc );
    STRING_CLASS( const STRING_CLASS & sorc );
    STRING_CLASS( int sorc );
    ~STRING_CLASS();
    BOOLEAN acquire( TCHAR * sorc );
    BOOLEAN acquire( TCHAR * sorc, int slen );
    BOOLEAN acquire( STRING_CLASS & sorc );
    void    empty();
    TCHAR * find( TCHAR c );
    TCHAR * find( const TCHAR * s_to_find );
    TCHAR * find( const STRING_CLASS & sorc ) { return find(sorc.s); }
    TCHAR * findlast( TCHAR c );
    TCHAR * findlast( const TCHAR * s_to_find );
    TCHAR * findlast( const STRING_CLASS & sorc ) { return findlast(sorc.s); }
    BOOLEAN init( int32 new_len );
    BOOLEAN isempty();
    BOOLEAN isnumeric();
    bool    iszero();
    int     compare( TCHAR * sorc );
    int     compare( STRING_CLASS & sorc ) { return compare(sorc.s); }
    bool    contains( TCHAR c );
    bool    contains( const TCHAR * s_to_find );
    bool    contains( const STRING_CLASS & sorc ) { return contains(sorc.s); }
    int32   countchars( TCHAR c ) { if (s) return ::countchars(c,s); else return 0; }
    int32   len();
    void    lowercase();
    void    matchcase( const TCHAR * sorc ) { if ( s && sorc ) ::matchcase( s, sorc ); }
    void    operator=( TCHAR sorc );
    void    operator=( const TCHAR * sorc );
    void    operator=( const STRING_CLASS & sorc );
    void    operator=( int n );
    void    operator=( unsigned int sorc );
    void    operator=( BOOLEAN sorc );
    void    operator=( double sorc );

    void    operator+=( const TCHAR   sorc );
    void    operator+=( const TCHAR * sorc );
    void    operator+=( const STRING_CLASS & sorc );
    void    operator+=( int n );
    void    operator+=( unsigned int sorc );
    void    operator+=( BOOLEAN sorc );
    void    operator+=( double sorc );

    BOOLEAN operator<( const STRING_CLASS & sorc );
    BOOLEAN operator<=( const STRING_CLASS & sorc );
    BOOLEAN operator<( const TCHAR * sorc );
    BOOLEAN operator>( const STRING_CLASS & sorc );
    BOOLEAN operator>=( const STRING_CLASS & sorc );
    BOOLEAN operator>( const TCHAR * sorc );
    BOOLEAN operator==(const STRING_CLASS & sorc );
    BOOLEAN operator==(const TCHAR * sorc );

    BOOLEAN operator!=(const STRING_CLASS & sorc ) { if (operator==(sorc)) return FALSE; return TRUE; }
    BOOLEAN operator!=(const TCHAR * sorc ) { if (operator==(sorc)) return FALSE; return TRUE; }

    BOOLEAN boolean_value();
    void    cat_w_char( const TCHAR * sorc, TCHAR c );
    void    cat_w_char( const STRING_CLASS & sorc, TCHAR c ) { cat_w_char( sorc.s, c ); }

    void    cat_path( const TCHAR * sorc );
    void    cat_path( const STRING_CLASS & sorc ) { cat_path( sorc.s ); }

    BOOLEAN get_ascii( char * sorc );
    BOOLEAN get_substring( const STRING_CLASS & sorc, TCHAR begin_char, TCHAR end_char );
    bool    get_text( HWND w );
    bool    get_text( HWND w, int id ) { return STRING_CLASS::get_text( GetDlgItem(w, id) ); }
    TCHAR * last_char();
    bool    next_field( TCHAR separator );
    bool    next_field();
    void    null();
    void    null_first_control_char();
    TCHAR * release();
    int     remove_all_occurances( TCHAR offending_char );
    bool    remove_leading_chars( int new_len );
    int     remove_leading_chars( TCHAR bad_char );
    BOOLEAN replace( const TCHAR * oldtext, const TCHAR * newtext );
    BOOLEAN replace( TCHAR oldc, TCHAR newc );
    int     replace_all( TCHAR oldc, TCHAR newc );
    int     replace_leading_chars( TCHAR bad_char, TCHAR replacement_char );
    BOOLEAN rjust( int32 new_len, TCHAR c );
    BOOLEAN rjust( int32 new_len );
    bool    set_text( HWND w );
    BOOLEAN set_text( HWND w, int id ) { return STRING_CLASS::set_text( GetDlgItem(w, id) ); }
    bool    start_equals( TCHAR * sorc );
    bool    start_equals( STRING_CLASS & sorc ) { return STRING_CLASS::start_equals( sorc.s ); }
    void    strip();
    void    strip_crlf();
    BOOLEAN strip_double_quotes();
    TCHAR * text() { if (s) return s; else return emptystring; }
    unsigned int uint_value() { if (s) return (unsigned int) asctoul(s); else return 0; }
    void    uppercase();
    BOOLEAN upsize( int32 new_len );
    int     int_value() { if (s) return (int) asctoint32(s); else return 0; }
    float   float_value() { if (s) return extfloat(s); else return 0.0; }
    double  real_value();
    };

#endif
