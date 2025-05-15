#ifndef _STRINGTABLE_CLASS_
#define _STRINGTABLE_CLASS_

class STRINGTABLE_ENTRY 
    {
    public:
    TCHAR * id;
    TCHAR * s;
    STRINGTABLE_ENTRY();
    ~STRINGTABLE_ENTRY();
    void acquire( STRINGTABLE_ENTRY & sorc );
    void cleanup();
    void operator=( const STRINGTABLE_ENTRY & sorc );
    };
    
class STRINGTABLE_CLASS
    {
    private:
    TCHAR               delimiter;
    STRING_CLASS        language;
    STRINGTABLE_ENTRY * se;
    int                 n;
    int                 x;  /* Current Index */
    HFONT               font_handle;
    FONT_CLASS          lf;
    
    public:
    STRINGTABLE_CLASS();
    ~STRINGTABLE_CLASS();
    void cleanup();
    void empty() { cleanup(); }
    void operator=(const STRINGTABLE_CLASS & sorc );
    int  count() { return n; }
    HFONT font() { return font_handle; }
    const FONT_CLASS & fontclass() { return lf; }
    bool append( STRING_CLASS & path );
    bool read( STRING_CLASS & path );
    TCHAR * find( TCHAR * id_to_find );
    TCHAR * find( STRING_CLASS & id_to_find );
    void rewind();
    bool next();
    TCHAR * id();
    TCHAR * text();
    void set_language( STRING_CLASS & new_language ) { language = new_language; }
    };

#endif
