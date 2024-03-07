class CUGArrowType: public CUGCellType {

public:

    CUGArrowType();
    ~CUGArrowType();
    virtual void OnDraw(HDC dc,RECT *rect,int col,long row,CUGCell *cell,
                        int selected,int current);

};
