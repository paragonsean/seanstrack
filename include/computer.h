#define GET_FIRST_COMPUTER 0
#define GET_NEXT_COMPUTER  1

struct COMPUTER_ENTRY
{
COMPUTER_ENTRY * next;
NAME_CLASS name;
BOOLEAN    online;
};

class COMPUTER_CLASS
{
private:

COMPUTER_ENTRY * c;
short            cx;
short            search_mode;

public:

COMPUTER_CLASS();
BOOLEAN add( TCHAR * new_name );
BOOLEAN add( STRING_CLASS & new_name ) { return add( new_name.text()); }
int     count();
STRING_CLASS & whoami();
STRING_CLASS & name();
void    empty();
BOOLEAN find( TCHAR * name );
inline BOOLEAN find( STRING_CLASS & name ) { return find(name.text()); }
BOOLEAN is_present();
BOOLEAN is_present( STRING_CLASS & name);
BOOLEAN is_this_computer();
BOOLEAN load();
BOOLEAN next();
BOOLEAN online_status();
BOOLEAN online_status( STRING_CLASS & name );
void    rewind();
BOOLEAN save();
BOOLEAN set_online_status( BOOLEAN new_status );
BOOLEAN set_online_status( STRING_CLASS & name, BOOLEAN new_status );
BOOLEAN startup();
BOOLEAN startup( STRING_CLASS & my_v5_directory );
void    shutdown();
};
