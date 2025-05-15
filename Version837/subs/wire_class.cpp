#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"
#include "..\include\wire_class.h"

struct DEFAULT_WIRE_ENTRY
{
TCHAR  * wire_number;
TCHAR  * input_connector_name;
TCHAR  * output_connector_name;
unsigned wire_bit;
};

static TCHAR CommaChar = TEXT( ',' );

static DEFAULT_WIRE_ENTRY DefaultWireList[] = {
    { TEXT("1"),  TEXT(""),    TEXT(""),    0x00000001 },
    { TEXT("2"),  TEXT(""),    TEXT(""),    0x00000002 },
    { TEXT("3"),  TEXT(""),    TEXT(""),    0x00000004 },
    { TEXT("4"),  TEXT(""),    TEXT(""),    0x00000008 },
    { TEXT("5"),  TEXT(""),    TEXT(""),    0x00000010 },
    { TEXT("6"),  TEXT(""),    TEXT(""),    0x00000020 },
    { TEXT("7"),  TEXT(""),    TEXT(""),    0x00000040 },
    { TEXT("8"),  TEXT(""),    TEXT(""),    0x00000080 },
    { TEXT("9"),  TEXT(""),    TEXT(""),    0x00000100 },
    { TEXT("10"), TEXT(""),    TEXT(""),    0x00000200 },
    { TEXT("11"), TEXT(""),    TEXT(""),    0x00000400 },
    { TEXT("12"), TEXT(""),    TEXT(""),    0x00000800 },
    { TEXT("13"), TEXT(""),    TEXT(""),    0x00001000 },
    { TEXT("14"), TEXT(""),    TEXT(""),    0x00002000 },
    { TEXT("15"), TEXT(""),    TEXT(""),    0x00004000 },
    { TEXT("16"), TEXT(""),    TEXT(""),    0x00008000 },
    { TEXT("17"), TEXT("61B"), TEXT("69B"), 0x00010000 },
    { TEXT("18"), TEXT("61A"), TEXT("69A"), 0x00020000 },
    { TEXT("19"), TEXT("60B"), TEXT("68B"), 0x00040000 },
    { TEXT("20"), TEXT("60A"), TEXT("68A"), 0x00080000 },
    { TEXT("21"), TEXT("59B"), TEXT("67B"), 0x00100000 },
    { TEXT("22"), TEXT("59A"), TEXT("67A"), 0x00200000 },
    { TEXT("23"), TEXT("58B"), TEXT("66B"), 0x00400000 },
    { TEXT("24"), TEXT("58A"), TEXT("66A"), 0x00800000 },
    { TEXT("25"), TEXT("57B"), TEXT("65B"), 0x01000000 },
    { TEXT("26"), TEXT("57A"), TEXT("65A"), 0x02000000 },
    { TEXT("27"), TEXT("56B"), TEXT("64B"), 0x04000000 },
    { TEXT("28"), TEXT("56A"), TEXT("64A"), 0x08000000 },
    { TEXT("29"), TEXT("55B"), TEXT("63B"), 0x10000000 },
    { TEXT("30"), TEXT("55A"), TEXT("63A"), 0x20000000 },
    { TEXT("31"), TEXT("54B"), TEXT("62B"), 0x40000000 },
    { TEXT("32"), TEXT("54A"), TEXT("62A"), 0x80000000 }
    };
const static unsigned UZero = 0;

/***********************************************************************
*                            WIRE_NAME_ENTRY                           *
***********************************************************************/
WIRE_NAME_ENTRY::WIRE_NAME_ENTRY()
{
wire_bit = UZero;
}

/***********************************************************************
*                           ~WIRE_NAME_ENTRY                           *
***********************************************************************/
WIRE_NAME_ENTRY::~WIRE_NAME_ENTRY()
{
}

/***********************************************************************
*                              WIRE_CLASS                              *
***********************************************************************/
WIRE_CLASS::WIRE_CLASS()
{
int i;

for ( i=0; i<32; i++ )
{
    wirelist[i].wire_number = DefaultWireList[i].wire_number;
    if ( !is_empty(DefaultWireList[i].input_connector_name) )
        wirelist[i].input_connector_name = DefaultWireList[i].input_connector_name;
    if ( !is_empty( DefaultWireList[i].output_connector_name) )
        wirelist[i].output_connector_name = DefaultWireList[i].output_connector_name;
    wirelist[i].wire_bit = DefaultWireList[i].wire_bit;
}
}

/***********************************************************************
*                              ~WIRE_CLASS                             *
***********************************************************************/
WIRE_CLASS::~WIRE_CLASS()
{
}

/***********************************************************************
*                                 GET                                  *
* This looks to see if there is a wire_connector_list.csv file         *
* for this machine. If so, the file will have one or more lines with   *
* the connector names.                                                 *
* Wire,17,Input Connector,61B,Output Connector,69B                     *
* Wire,18,Input Connector,61A Output Connector,69A                     *
* These will be read and replace the default values in my wirelist.    *
*                                                                      *
* The return value mearly indicates whether I read anything or not.    *
* it does not indicate an error.                                       *
***********************************************************************/
BOOLEAN WIRE_CLASS::get( STRING_CLASS & co, STRING_CLASS & ma )
{
int        i;
FILE_CLASS f;
NAME_CLASS s;

s.get_wire_connector_list_csv( co, ma );
if ( s.file_exists() )
    {
    if ( f.open_for_read(s) )
    {
        while ( true )
    {
            s = f.readline();
            if ( s.isempty() )
                break;
            if ( s.countchars(CommaChar) == 3 )
{
                s.next_field();
                i = s.int_value();
                i--;
                if ( i>=0 && i<32 )
    {
                    s.next_field();
                    s.next_field();
                    wirelist[i].input_connector_name = s;

                    s.next_field();
                    s.next_field();
                    wirelist[i].output_connector_name = s;
                    }
                }
            }
        f.close();
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                             wire_number                              *
***********************************************************************/
int WIRE_CLASS::wire_number( STRING_CLASS & wire_name )
{
int i;

for ( i=0; i<32; i++ )
    {
    if ( wire_name == wirelist[i].wire_number || wire_name == wirelist[i].input_connector_name || wire_name == wirelist[i].output_connector_name )
        return i+1;
    }

return NO_WIRE;
}

/***********************************************************************
*                               wirebit                                *
***********************************************************************/
unsigned WIRE_CLASS::wirebit( STRING_CLASS & wire_name )
{
const unsigned UZERO = 0;

int i;

for ( i=0; i<32; i++ )
{
    if ( wire_name == wirelist[i].wire_number || wire_name == wirelist[i].input_connector_name || wire_name == wirelist[i].output_connector_name )
        return wirelist[i].wire_bit;
}

return UZERO;
}

/***********************************************************************
*                               wirebit                                *
***********************************************************************/
unsigned WIRE_CLASS::wirebit( int wire_number )
{
unsigned x;

x = 0;
if ( wire_number > NO_WIRE )
    {
    x = 1;
    wire_number--;
    if ( wire_number > 0 )
        x <<= wire_number;
}

return x;
}