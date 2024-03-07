#ifndef _MULTISTRING_CLASS_
#define _MULTISTRING_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

class MULTISTRING_CLASS : public STRING_CLASS {
private:
STRING_CLASS * a;
int            na;
int            current_index;

public:
MULTISTRING_CLASS();
MULTISTRING_CLASS( int new_n );
~MULTISTRING_CLASS();
bool append( const TCHAR * sorc );
bool append( const STRING_CLASS & sorc );
bool copy( MULTISTRING_CLASS & sorc );
int  count() { return na; }
bool insert( const TCHAR * sorc );
bool insert( const STRING_CLASS & sorc );
bool set_array_size( int new_n );
bool empty_array();
int  find( STRING_CLASS & s_to_find );
bool goto_index( int dest_index );
bool rewind();
bool next();
bool split( const TCHAR separator );
bool join( const TCHAR separator );
bool join();
bool remove( int n_to_remove );
bool sort();
MULTISTRING_CLASS & operator[]( int i );
void    operator=( MULTISTRING_CLASS & sorc ) { STRING_CLASS::operator=(sorc); }
void    operator=( TCHAR sorc ) { STRING_CLASS::operator=(sorc); }
void    operator=( const TCHAR * sorc ) { STRING_CLASS::operator=(sorc); }
void    operator=( const STRING_CLASS & sorc ) { STRING_CLASS::operator=(sorc); }
void    operator=( int n ) { STRING_CLASS::operator=(n); }
void    operator=( unsigned int sorc ) { STRING_CLASS::operator=(sorc); }
void    operator=( BOOLEAN sorc ) { STRING_CLASS::operator=(sorc); }
void    operator=( double sorc )  { STRING_CLASS::operator=(sorc); }
void    operator+=( const TCHAR   sorc ) { STRING_CLASS::operator+=(sorc); }
void    operator+=( const TCHAR * sorc ) { STRING_CLASS::operator+=(sorc); }
void    operator+=( const STRING_CLASS & sorc ) { STRING_CLASS::operator+=(sorc); }
void    operator+=( int n ) { STRING_CLASS::operator+=(n); }
void    operator+=( unsigned int sorc ) { STRING_CLASS::operator+=(sorc); }
void    operator+=( BOOLEAN sorc ){ STRING_CLASS::operator+=(sorc); }
void    operator+=( double sorc ){ STRING_CLASS::operator+=(sorc); }
BOOLEAN operator<( const TCHAR * sorc ){ return STRING_CLASS::operator<(sorc); }
BOOLEAN operator<( const STRING_CLASS & sorc ){ return STRING_CLASS::operator<(sorc); }
BOOLEAN operator<( const MULTISTRING_CLASS & sorc ){ return STRING_CLASS::operator<(sorc); }
BOOLEAN operator<=( const TCHAR * sorc ){ return STRING_CLASS::operator<=(sorc); }
BOOLEAN operator<=( const STRING_CLASS & sorc ){ return STRING_CLASS::operator<=(sorc); }
BOOLEAN operator<=( const MULTISTRING_CLASS & sorc ){ return STRING_CLASS::operator<=(sorc); }
BOOLEAN operator>( const TCHAR * sorc ){ return STRING_CLASS::operator>(sorc); }
BOOLEAN operator>( const STRING_CLASS & sorc ){ return STRING_CLASS::operator>(sorc); }
BOOLEAN operator>( const MULTISTRING_CLASS & sorc ){ return STRING_CLASS::operator>(sorc); }
BOOLEAN operator>=( const TCHAR * sorc ){ return STRING_CLASS::operator>=(sorc); }
BOOLEAN operator>=( const STRING_CLASS & sorc ){ return STRING_CLASS::operator>=(sorc); }
BOOLEAN operator>=( const MULTISTRING_CLASS & sorc ){ return STRING_CLASS::operator>=(sorc); }
BOOLEAN operator==(const TCHAR * sorc ){ return STRING_CLASS::operator==(sorc); }
BOOLEAN operator==(const STRING_CLASS & sorc ){ return STRING_CLASS::operator==(sorc); }
BOOLEAN operator==(const MULTISTRING_CLASS & sorc ){ return STRING_CLASS::operator==(sorc); }
BOOLEAN operator!=(const TCHAR * sorc ) { if (STRING_CLASS::operator==(sorc)) return FALSE; return TRUE; }
BOOLEAN operator!=(const STRING_CLASS & sorc ) { if (STRING_CLASS::operator==(sorc)) return FALSE; return TRUE; }
BOOLEAN operator!=(const MULTISTRING_CLASS & sorc ) { if (STRING_CLASS::operator==(sorc)) return FALSE; return TRUE; }
};

#endif
