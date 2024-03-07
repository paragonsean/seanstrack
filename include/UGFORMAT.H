/***********************************************
    Ultimate Grid 97
    Copyright 1994 - 1997 Dundas Software Ltd.

    Class
        CUGCellFormat
    Purpose
        This class is used for the formating the
        cells data for display and/or for editing.
    Details
************************************************/

class CUGCellFormat {

public:
    CUGCellFormat();
    ~CUGCellFormat();

    void ApplyDisplayFormat(CUGCell *cell);

    int  ValidateCellInfo(CUGCell *cell);
};
