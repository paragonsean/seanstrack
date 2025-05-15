#ifndef _GENLIST_H

#define _GENLIST_H

struct GENERIC_LIST_ENTRY
    {
    GENERIC_LIST_ENTRY * next;
    void               * item;
    };

class GENERIC_LIST_CLASS
    {
    private:

    GENERIC_LIST_ENTRY * first;
    GENERIC_LIST_ENTRY * prev;
    GENERIC_LIST_ENTRY * gp;

    public:
    GENERIC_LIST_CLASS() { first=0; prev=0; gp=0; }
    ~GENERIC_LIST_CLASS();
    BOOLEAN   append( void * new_item );
    short     count();
    BOOLEAN   find( void * item_to_find );
    BOOLEAN   insert( void * new_item );
    void    * next();
    void    * next_loop();
    BOOLEAN   remove();
    BOOLEAN   remove( void * item_to_remove );
    void      remove_all();
    void      rewind();
    BOOLEAN   sort( int (* compare)(const void * sorc1, const void * sorc2) );
    };

#endif
