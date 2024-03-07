#ifndef _CATEGORY_CLASS_
#define _CATEGORY_CLASS_

struct CATEGORY_ENTRY
    {
    TCHAR number[DOWNCAT_NUMBER_LEN+1];
    TCHAR name[DOWNCAT_NAME_LEN+1];
    };

class CATEGORY_CLASS
{
private:

int x;
int n;
CATEGORY_ENTRY * cp;
void cleanup();

public:

CATEGORY_CLASS();
~CATEGORY_CLASS();
BOOLEAN find( TCHAR * cat_to_find );
BOOLEAN findname( const TCHAR * name_to_find );
BOOLEAN next();
BOOLEAN read();
void    rewind() { x = NO_INDEX; }
TCHAR * name();
TCHAR * cat();
};

struct SUBCAT_ENTRY
    {
    TCHAR catnumber[DOWNCAT_NUMBER_LEN+1];
    TCHAR subnumber[DOWNCAT_NUMBER_LEN+1];
    TCHAR name[DOWNCAT_NAME_LEN+1];
    };

class SUBCAT_CLASS
{
private:

int x;
int n;
SUBCAT_ENTRY * sp;
void cleanup();

public:

SUBCAT_CLASS() { n = 0; x = NO_INDEX;  sp = 0; }
~SUBCAT_CLASS();
BOOLEAN find( TCHAR * cat_to_find, TCHAR * subcat_to_find );
BOOLEAN findname( TCHAR * cat_to_find, TCHAR * name_to_find );
BOOLEAN read();
void    rewind() { x = NO_INDEX; }
BOOLEAN next();
TCHAR * name();
TCHAR * cat();
TCHAR * subcat(void );
};

#endif
