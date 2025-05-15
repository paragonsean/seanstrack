#ifndef _STRINGTABLE_CLASS_
#define _STRINGTABLE_CLASS_

class STRINGTABLE_CLASS
    {
    private:
    STRING_CLASS        language;
    HFONT               font_handle;
    FONT_CLASS          lf;

    public:
    STRINGTABLE_CLASS();
    ~STRINGTABLE_CLASS();
    void    operator=( STRINGTABLE_CLASS & sorc );
    HFONT   font() { return font_handle; }
    const   FONT_CLASS & fontclass() { return lf; }
    bool    find( STRING_CLASS & dest, TCHAR * id_to_find );
    bool    find( STRING_CLASS & dest, STRING_CLASS & id_to_find );
    TCHAR * find( TCHAR * id_to_find );
    void    set_language( STRING_CLASS & new_language );
    };

#endif
