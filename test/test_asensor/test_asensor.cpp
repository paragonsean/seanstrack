#include <windows.h>
#include <richedit.h>
#include <conio.h>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "..\..\include\visiparm.h"
#include "..\..\include\nameclas.h"
#include "..\..\include\asensor.h"
#include "..\..\include\dbclass.h"
#include "..\..\include\listbox.h"
#include "..\..\include\multistring.h"
#include "..\..\include\setpoint.h"
#include "..\..\include\vdbclass.h"
#include "..\..\include\wclass.h"
#include "..\..\include\subs.h"
#include "..\..\include\fileclas.h"
#include "..\..\include\part.h"
#include "..\..\include\ftcalc.h"

#include "resource.h"
#include "resrc1.h"

#define _MAIN_
#include "..\..\include\chars.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS OkButton;
WINDOW_CLASS MyTbox;
LISTBOX_CLASS MainListbox;

static TCHAR MyClassName[] = TEXT( "Test");

/***********************************************************************
*                              TEST CALCULATOR                         *
***********************************************************************/
void test_calculator()
{
    std::string test_exp_001 = "(10+1)*2-(2*3+2)";
    std::string test_ans_001 = "14.000000";
    std::string test_exp_002 = "(10+1)*2-2*(3+2)-1";
    std::string test_ans_002 = "11.000000";
    std::string test_exp_003 = "100.00*0.5";
    std::string test_ans_003 = "50.000000";
    std::string test_exp_004 = "100^.5";
    std::string test_ans_004 = "10.000000";
    std::string test_exp_005 = "(C1+C2)*(C3.5+.75)";
    std::string test_ans_005 = "12.750000";
    std::string test_exp_006 = "C10 / C2";
    std::string test_ans_006 = "5.000000";

    std::queue<std::string> infixQ;
    std::queue<std::string> postfixQ;
    std::string             result;
    std::string             pass_fail;
    std::stringstream       result_ss;

    FTCALC_CLASS calc;

    /*
    -----------------------------------------
    Test Expression 001
    ----------------------------------------- */
    calc.infixStringToQueue( test_exp_001, infixQ );
    calc.infixToPostfix( &infixQ, &postfixQ );
    result = calc.evaluatePostfix( &postfixQ );

    if (result == test_ans_001)
    {
        pass_fail.clear();
        pass_fail = "PASS: " + result;
        MessageBox( NULL, pass_fail.c_str(), test_exp_001.c_str(), MB_OK );
    }
    else
    {
        pass_fail.clear();
        pass_fail = "FAIL: " + result;
        MessageBox( NULL, pass_fail.c_str(), test_exp_001.c_str(), MB_OK );
    }

    /*
    -------------------
    Test Expression 002
    ------------------- */
    calc.infixStringToQueue( test_exp_002, infixQ );
    calc.infixToPostfix( &infixQ, &postfixQ );
    result = calc.evaluatePostfix( &postfixQ );

    if ( result == test_ans_002 )
    {
        pass_fail.clear();
        pass_fail = "PASS: " + result;
        MessageBox( NULL, pass_fail.c_str(), test_exp_002.c_str(), MB_OK );
    }
    else
    {
        pass_fail.clear();
        pass_fail = "FAIL: " + result;
        MessageBox( NULL, pass_fail.c_str(), test_exp_002.c_str(), MB_OK );
    }

    /*
    -------------------
    Test Expression 003
    ------------------- */
    calc.infixStringToQueue( test_exp_003, infixQ );
    calc.infixToPostfix( &infixQ, &postfixQ );
    result = calc.evaluatePostfix( &postfixQ );

    if ( result == test_ans_003 )
    {
        pass_fail.clear();
        pass_fail = "PASS: " + result;
        MessageBox( NULL, pass_fail.c_str(), test_exp_003.c_str(), MB_OK );
    }
    else
    {
        pass_fail.clear();
        pass_fail = "FAIL: " + result;
        MessageBox( NULL, pass_fail.c_str(), test_exp_003.c_str(), MB_OK );
    }

    /*
    -------------------
    Test Expression 004
    -------------------*/
    calc.infixStringToQueue( test_exp_004, infixQ );
    calc.infixToPostfix( &infixQ, &postfixQ );
    result = calc.evaluatePostfix( &postfixQ );

    if (result == test_ans_004)
    {
        pass_fail.clear();
        pass_fail = "PASS: " + result;
        MessageBox( NULL, pass_fail.c_str(), test_exp_004.c_str(), MB_OK );
    }
    else
    {
        pass_fail.clear();
        pass_fail = "FAIL: " + result;
        MessageBox( NULL, pass_fail.c_str(), test_exp_004.c_str(), MB_OK );
    }

    /*
    -------------------
    Test Expression 005
    -------------------*/
    calc.infixStringToQueue( test_exp_005, infixQ );
    calc.infixToPostfix( &infixQ, &postfixQ );
    result = calc.evaluatePostfix( &postfixQ );

    if (result == test_ans_005)
    {
        pass_fail.clear();
        pass_fail = "PASS: " + result;
        MessageBox(NULL, pass_fail.c_str(), test_exp_005.c_str(), MB_OK);
    }
    else
    {
        pass_fail.clear();
        pass_fail = "FAIL: " + result;
        MessageBox(NULL, pass_fail.c_str(), test_exp_005.c_str(), MB_OK);
    }

    /*
    -------------------
    Test Expression 006
    ------------------- */
    calc.infixStringToQueue( test_exp_006, infixQ );
    calc.infixToPostfix( &infixQ, &postfixQ );
    result = calc.evaluatePostfix( &postfixQ );

    if (result == test_ans_006)
    {
        pass_fail.clear();
        pass_fail = "PASS: " + result;
        MessageBox( NULL, pass_fail.c_str(), test_exp_006.c_str(), MB_OK );
    }
    else
    {
        pass_fail.clear();
        pass_fail = "FAIL: " + result;
        MessageBox(NULL, pass_fail.c_str(), test_exp_006.c_str(), MB_OK);
    }
}

/***********************************************************************
*                         TEST_SENSOR_RESPONSE                         *
***********************************************************************/
static void test_sensor_response()
{
    static TCHAR f_asensor_increasing[] = TEXT( "c:\\src\\version8\\test\\test_asensor\\test_asensor_input_increasing.csv" );

	ANALOG_SENSOR asensor;
    TCHAR       * f_sensor;
    FILE_CLASS    fin;
    TCHAR       * line;
	NAME_CLASS    sensor_file;
	STRING_CLASS  str;
	double        answer;
	double        response;
	double        test_value;
	double        test_voltage;

	asensor.get( &sensor_file );

    /*
    ----------------------------------------------------
    Read in test input file
    test input file is a csv with the following columns:
    voltage,answer,value,answer
    ---------------------------------------------------- */
	if ( !(fin.open_for_read(f_asensor_increasing)) )
	{
		MessageBox(NULL, f_asensor_increasing, "Error opening file", MB_OK);
		return;
	}

    str = fin.readline();


    /*
    -------------------------
    Test linear interpolation
    ------------------------- */
	bool pass = true;

    /*
    ------------------------------------
    Test sensor with increasing response
    ------------------------------------ */
	asensor.find("Test Sensor Increasing");
	while ( str.next_field() )
	{
        /*
        -------------------------------
        Test converted_value increasing
        ------------------------------- */
		test_voltage = str.real_value();
		str.next_field();
		answer = str.real_value();
		response = asensor.converted_value(test_voltage);

		if ( response != answer )
		{
			pass = false;
		}

		/* test raw_count increasing */
		str.next_field();
		test_value = str.real_value();
		str.next_field();
		answer = str.real_value();
		response = asensor.raw_count(test_value);

		if (response != answer)
		{
			pass = false;
		}
	}

	if (pass)
	{
		MessageBox(NULL, "PASSED", "Sensor Response Increasing Test", MB_OK);
	}
	else
	{
		MessageBox(NULL, "FAILED", "Sensor Response Increasing Test", MB_OK);
	}

	fin.close();
	TCHAR * f_asensor_decreasing = "c:\\src\\version8\\test\\test_asensor\\test_asensor_input_decreasing.csv";
	if (!(fin.open_for_read(f_asensor_decreasing)))
	{
		MessageBox(NULL, f_asensor_decreasing, "Error opening file", MB_OK);
		return;
	}

	/* test sensor with decreasing response */
	pass = true;
	asensor.find("Test Sensor Decreasing");
	while (str.next_field())
	{
		/* test converted_value decreasing */
		test_voltage = str.real_value();
		str.next_field();
		answer = str.next_field();
		response = asensor.converted_value(test_voltage);

		if (response != answer)
		{
			pass = false;
		}

		/* test raw_count decreasing */
		str.next_field();
		test_value = str.real_value();
		str.next_field();
		answer = str.real_value();
		response = asensor.raw_count(test_value);

		if (response != answer)
		{
			pass = false;
		}
	}

	if (pass)
	{
		MessageBox(NULL, "PASSED", "Sensor Response Decreasing Test", MB_OK);
	}
	else
	{
		MessageBox(NULL, "FAILED", "Sensor Response Decreasing Test", MB_OK);
	}
}

/***********************************************************************
*                           CREATE_FILES                               *
************************************************************************/
static void create_files()
{
TCHAR sorc[] = TEXT( "c:\\sources\\version8\\test\\results\\000001.pr2" );
TCHAR myroot[] = TEXT( "c:\\sources\\version8\\test\\results" );
int i;

NAME_CLASS s;
NAME_CLASS sn;
NAME_CLASS d;
s = sorc;

for ( i=2; i<10001; i++)
    {
    sn = i;
    sn.rjust( 6, ZeroChar );
    sn += TEXT( ".pr2" );
    d = myroot;
    d.cat_path( sn );
    s.copyto( d );
    }
}

/***********************************************************************
*                              READ_FILES                              *
***********************************************************************/
static void read_files()
{
TCHAR sorc[] = TEXT( "c:\\sources\\version7\\test\\results\\000001.csv" );
TCHAR myroot[] = TEXT( "c:\\sources\\version7\\test\\results" );
int i;
DWORD t1;
DWORD t2;
SETPOINT_CLASS s;
NAME_CLASS sn;
NAME_CLASS d;

t1 = GetTickCount();
for ( i=1; i<1001; i++)
    {
    sn = i;
    sn.rjust( 6, ZeroChar );
    sn += TEXT( ".csv" );
    d = myroot;
    d.cat_path( sn );
    s.get( d );
    }
t2 = GetTickCount();

sn = (unsigned int) t1;
d  = (unsigned int) t2;
MessageBox( 0, d.text(), sn.text(), MB_OK );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;
int id;
STRING_CLASS  s;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DBINIT:
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case IDOK:
                if ( cmd == BN_CLICKED )
                {
                    TCHAR testStr[500];
                    GetDlgItemText(hWnd, MY_TBOX, testStr, 500);

                    FTCALC_CLASS calc;
                    calc.parseLines(testStr);

                    /*
                    //infix to postfix
                    queue<string> infixQ;
                    queue<string> postfixQ;
                    string postfixString;
                    FTCALC_CLASS calc;
                    calc.infixStringToQueue(testStr, infixQ);
                    calc.infixToPostfix(&infixQ, &postfixQ);
                    calc.stringQueueToString(postfixQ, postfixString);
                    MessageBox(NULL, postfixString.c_str(), "Postfix", MB_OK);
                    string result = calc.evaluatePostfix(&postfixQ);
                    MessageBox(NULL, result.c_str(), "Evaluated", MB_OK);
                    */
                }
                return 0;

            case CREATE_CHOICE:
				test_sensor_response();
                return 0;

            case TEST_CALCULATOR:
                test_calculator();
                return 0;

            case DELETE_CHOICE:
                return 0;

            case EXIT_CHOICE:
                MainWindow.close();
                return 0;
            }

        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
WNDCLASS wc;
HWND     w;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = TEXT( "MAINMENU" );
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

w = CreateWindow(
    MyClassName,
    TEXT("Main Window Title"),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 300,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MyTbox = CreateWindow(
    TEXT("edit"),
    //TEXT("RichEdit"),
    "0",
    WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT | ES_MULTILINE,
    //WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
    0, 0,
    150, 100,
    w,
    (HMENU) MY_TBOX,
    MainInstance,
    NULL
    );

OkButton = CreateWindow (
    TEXT("button"),
    "OK",
    WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
    151, 0,
    30, 20,
    w,
    (HMENU) IDOK,
    MainInstance,
    NULL
    );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
