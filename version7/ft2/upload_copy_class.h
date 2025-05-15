#ifndef _UPLOAD_COPY_CLASS_
#define _UPLOAD_COPY_CLASS_

#define NOF_FT2_VARIABLES 550

class UPLOAD_COPY_CLASS
    {
    private:
    STRING_CLASS a[NOF_FT2_VARIABLES];
    bool have_change;

    public:
    UPLOAD_COPY_CLASS();
    ~UPLOAD_COPY_CLASS();
    void init();
    bool is_changed();
    bool isnew( STRING_CLASS & sorc );
    STRING_CLASS & operator[]( int i );
    unsigned load( STRING_CLASS co, STRING_CLASS ma, STRING_CLASS pa );
    unsigned save( STRING_CLASS co, STRING_CLASS ma, STRING_CLASS pa );
    };

#endif