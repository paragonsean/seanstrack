#ifndef _LIMIT_SWITCH_CLASS_
#define _LIMIT_SWITCH_CLASS_

#ifndef _FTII_H_
#define MAX_FTII_LIMIT_SWITCHES 6
#endif

class LIMIT_SWITCH_DESC_CLASS
{
private:
STRING_CLASS desc[MAX_FTII_LIMIT_SWITCHES];
public:
LIMIT_SWITCH_DESC_CLASS();
~LIMIT_SWITCH_DESC_CLASS();
STRING_CLASS & operator[]( int index );
void     empty();
BOOLEAN  get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
BOOLEAN  put( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
};

class LIMIT_SWITCH_POS_CLASS
{
private:
STRING_CLASS pos[MAX_FTII_LIMIT_SWITCHES];
public:
LIMIT_SWITCH_POS_CLASS();
~LIMIT_SWITCH_POS_CLASS();
STRING_CLASS & operator[]( int index );
double   real_pos( int index );
void     empty();
BOOLEAN  get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
BOOLEAN  get( STRING_CLASS & path );
BOOLEAN  put( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
BOOLEAN  put( STRING_CLASS & path );
};

class LIMIT_SWITCH_WIRE_CLASS
{
private:
STRING_CLASS wire[MAX_FTII_LIMIT_SWITCHES];
public:
LIMIT_SWITCH_WIRE_CLASS();
~LIMIT_SWITCH_WIRE_CLASS();
TCHAR *  ascii_mask( int index );
int      wire_number( int index );
unsigned mask( int index );
STRING_CLASS & operator[]( int index );
void     empty();
BOOLEAN  get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
BOOLEAN  put( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
};

class LIMIT_SWITCH_CLASS
{
private:
LIMIT_SWITCH_DESC_CLASS description;
LIMIT_SWITCH_POS_CLASS  position;
LIMIT_SWITCH_WIRE_CLASS mywire;
public:
LIMIT_SWITCH_CLASS();
~LIMIT_SWITCH_CLASS();
TCHAR        * ascii_mask( int index ) { return mywire.ascii_mask( index );  }
unsigned       mask( int index )       { return mywire.mask( index );        }
int            wire_number( int index) { return mywire.wire_number( index ); }
STRING_CLASS & wire( int index )       { return mywire[index];               }
STRING_CLASS & pos( int index )        { return position[index];             }
double         real_pos( int index )   { return position.real_pos(index);    }
STRING_CLASS & desc( int index )       { return description[index];          }
TCHAR        * text_desc( int index )  { return description[index].text();   }
void           empty();
BOOLEAN        get( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
BOOLEAN        put( STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part );
};

#endif
