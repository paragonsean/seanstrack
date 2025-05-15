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
#include <regex>
#include <shlobj.h>

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
#include "..\..\include\ftii_parameters.h"

#include "resource.h"
#include "resrc1.h"

#define _MAIN_
#include "..\..\include\chars.h"

extern TCHAR MonallIniFile[] = TEXT( "Monall.ini" );
extern TCHAR ConfigSection[] = TEXT( "Config" );

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS OkButton;
WINDOW_CLASS MyTbox;
WINDOW_CLASS ResultBox;
LISTBOX_CLASS MainListbox;
PART_CLASS    CurrentPart;
MACHINE_CLASS CurrentMachine;
PARAMETER_CLASS CurrentParam;
ANALOG_SENSOR CurrentAnalogSensor;
LIMIT_SWITCH_CLASS CurrentLimitSwitch;
FTANALOG_CLASS CurrentFTAnalog;
NAME_CLASS TestDataDir;
FTII_PROFILE CurrentProfile;

std::string computer_name;
std::string machine_name;
std::string part_name;

static TCHAR MyClassName[] = TEXT( "Test");

/***********************************************************************
*                               TEST FUNCTIONS                         *
***********************************************************************/
double *init_calculator()
{
    NAME_CLASS part_file;
    NAME_CLASS profile_file;
    DB_TABLE part_db;
    STRING_CLASS shot_name;    
    FTCALC_CLASS ftcalc;
    STRING_CLASS s;
    int n;
    double *parameters;

    /*
    ----------------------
    Get list of shot files
    ---------------------- */
    if (computer_name.empty())
    {
        CurrentPart.computer = TEXT("C:\\V8");
    }
    else
    {
        CurrentPart.computer = computer_name.c_str();
    }

    if (machine_name.empty())
    {
        CurrentPart.machine = TEXT("7-001");
    }
    else
    {
        CurrentPart.machine = machine_name.c_str();
    }

    if (part_name.empty())
    {
        CurrentPart.name = TEXT("75082 1720");
    }
    else
    {
        CurrentPart.name = part_name.c_str();
    }

    CurrentParam.find(CurrentPart.computer, CurrentPart.machine, CurrentPart.name);
    CurrentPart.find(CurrentPart.computer, CurrentPart.machine, CurrentPart.name);
    part_file.get_graphlst_dbname(CurrentPart.computer, CurrentPart.machine, CurrentPart.name);

    if (part_file.file_exists())
    {
        if (part_db.open(part_file, 0, PFL))
        {
            n = part_db.nof_recs() - 1;
            part_db.goto_record_number(n);
            part_db.get_next_record(NO_LOCK);
            part_db.get_alpha(shot_name, GRAPHLST_SHOT_NAME_INDEX);
        }
    }

    /*
    -------------------------------------
    Init parameters from latest shot file
    ------------------------------------- */
    profile_file.get_profile_name(CurrentPart.computer, CurrentPart.machine, CurrentPart.name, shot_name.text(), TEXT(".pr2"));

    if (!CurrentProfile.get(profile_file))
    {
        s = TEXT("NO_SHOT_FILE");
        MessageBox( NULL, shot_name.text(), s.text(), MB_OK );
        return FALSE;
    }

    parameters = calculate_ftii_parameters(
        CurrentProfile,
        CurrentMachine,
        CurrentPart,
        CurrentParam,       
        CurrentFTAnalog, //need valid data for some vars
        ftcalc,
        CurrentLimitSwitch);

    return parameters;
}

void test_calculator()
{
    STRING result_str;
    STRING pass_fail;
    FTCALC_CLASS calc;
    STRING_CLASS command;
    double *parameters;  
    double result;
    PART_CLASS test_part;
    size_t i;
    std::vector<STRING> exp;
    std::vector<STRING> ans;
    BOOL all_passed = true;  
    TCHAR buf[100];

    /*
    -------------------------
    Single line numeric tests
    ------------------------- */
    static const STRING test_exp_001 = TEXT( "(10+1)*2-(2*3+2)" );
    static const STRING test_ans_001 = TEXT("14.0");
    static const STRING test_exp_002 = TEXT("(10+1)*2-2*(3+2)-1");
    static const STRING test_ans_002 = TEXT("11.0");
    static const STRING test_exp_003 = TEXT("100.00*0.5");
    static const STRING test_ans_003 = TEXT("50.0");
    static const STRING test_exp_004 = TEXT("100^.5");
    static const STRING test_ans_004 = TEXT("10.0");
    static const STRING test_exp_005 = TEXT("(C1+C2)*(C3.5+.75)");
    static const STRING test_ans_005 = TEXT("12.75");
    static const STRING test_exp_006 = TEXT("C10 / C2");
    static const STRING test_ans_006 = TEXT("5.0");

    /*
    ------------------------
    Multiline variable tests
    ------------------------ */
    static const STRING test_exp_007 = TEXT("x001 = MSL * 2.0\r\nx001\r\n");
    static const STRING test_ans_007 = TEXT("558.8");
    static const STRING test_exp_008 = TEXT("x=PD * 2\r\ny=x^.5*3\r\ny");
    static const STRING test_ans_008 = TEXT("32.073");
    static const STRING test_exp_009 = TEXT("abc = PA / 2\r\nxyz = (abc + 2)\r\nxyz");
    static const STRING test_ans_009 = TEXT("1284.557");
    static const STRING test_exp_010 = TEXT("var01=P100 + P24\r\nvar01");
    static const STRING test_ans_010 = TEXT("0.0");
    static const STRING test_exp_011 = TEXT("v_50 = P50 / P2\r\nv_50");
    static const STRING test_ans_011 = TEXT("0.0");
    static const STRING test_exp_012 = TEXT("time01 = TIM1 + 25\r\ntime01");
    static const STRING test_ans_012 = TEXT("25.0");
    static const STRING test_exp_013 = TEXT("pos_001 = POS100 - 50.5\r\npos_001");
    static const STRING test_ans_013 = TEXT("-50.5");
    static const STRING test_exp_014 = TEXT("vel_001 = VEL100 - 50.5\r\nvel_001");
    static const STRING test_ans_014 = TEXT("-50.5");
    static const STRING test_exp_015 = TEXT("bigvar = EOS_VEL*2+p1top3+p2top3\r\nbigvar");
    static const STRING test_ans_015 = TEXT("305.816");
    static const STRING test_exp_016 = TEXT("BiGvAr_001 = EOS_VEL*2+p1top3+p2top3\r\nBiGvAr_001");
    static const STRING test_ans_016 = TEXT("305.816");

    /* 
    -------------------------
    Tests with result keyword
    ------------------------- */
    static const STRING test_exp_017 = TEXT("BiGvAr_001 = EOS_VEL*2+p1top3+p2top3\r\nresult = BiGvAr_001");
    static const STRING test_ans_017 = TEXT("305.816");
    static const STRING test_exp_018 = TEXT("x = 1+2\r\nresult = x");
    static const STRING test_ans_018 = TEXT("3.0");

    /*
    --------------
    Function tests
    -------------- */
    static const STRING test_exp_019 = TEXT( "x= TARGET_POS(VEL,1.56,Y,0)\r\nresult = x" );
    static const STRING test_ans_019 = TEXT( "206.68" );
    static const STRING test_exp_020 = TEXT( "x = MAX_VALUE(VEL,44,50)\r\nresult = x" );
    static const STRING test_ans_020 = TEXT( "0.235" );
    static const STRING test_exp_021 = TEXT( "x = MIN_VALUE(VEL,44,50)\r\nresult = x" );
    static const STRING test_ans_021 = TEXT( "0.226" );
    static const STRING test_exp_022 = TEXT( "start = 22\r\nend = 50\r\nMAX_VALUE(VEL, start * 2 , end )" );
    static const STRING test_ans_022 = TEXT( "0.235" );
    static const STRING test_exp_023 = TEXT( "start = 22.0\r\nend = 50\r\nMAX_VALUE(VEL, start*2.0 , end/1.0 )" );
    static const STRING test_ans_023 = TEXT( "0.235" );
    static const STRING test_exp_024 = TEXT( "s=44.0\r\ne=49\r\nMIN_VALUE(VEL,s, e + 1.0 )" );
    static const STRING test_ans_024 = TEXT( "0.226" );
    static const STRING test_exp_025 = TEXT( "s=44.0\r\ne=49\r\nMIN_VALUE(VEL,s, e + 1.0 )" );
    static const STRING test_ans_025 = TEXT( "0.226" );
    static const STRING test_exp_026 = TEXT( "x= TARGET_POS(VEL,1.56,Y,0)\r\nresult = x" );
    static const STRING test_ans_026 = TEXT( "206.68" );
    static const STRING test_exp_027 = TEXT( "t = 1.56\r\nx = TARGET_POS(VEL, t, Y, 0)\r\nresult = x" );
    static const STRING test_ans_027 = TEXT( "206.68" );
    static const STRING test_exp_028 = TEXT( "t = 1.56\r\ne=0.0\r\nx = TARGET_POS(VEL, t, Y, e* 1.0)\r\nresult = x" );
    static const STRING test_ans_028 = TEXT( "206.68" );


    /*
    ---------------------------------------------
    Push the exp and ans strings into the vectors
    --------------------------------------------- */
    exp.push_back( test_exp_001 );
    ans.push_back( test_ans_001 );
    exp.push_back( test_exp_002 );
    ans.push_back( test_ans_002 );
    exp.push_back( test_exp_003 );
    ans.push_back( test_ans_003 );
    exp.push_back( test_exp_004 );
    ans.push_back( test_ans_004 );
    exp.push_back( test_exp_005 );
    ans.push_back( test_ans_005 );
    exp.push_back( test_exp_006 );
    ans.push_back( test_ans_006 );
    exp.push_back( test_exp_007 );
    ans.push_back( test_ans_007 );
    exp.push_back( test_exp_008 );
    ans.push_back( test_ans_008 );
    exp.push_back( test_exp_009 );
    ans.push_back( test_ans_009 );    
    exp.push_back( test_exp_010 );
    ans.push_back( test_ans_010 );
    exp.push_back( test_exp_011 );
    ans.push_back( test_ans_011 );
    exp.push_back( test_exp_012 );
    ans.push_back( test_ans_012 );
    exp.push_back( test_exp_013 );
    ans.push_back( test_ans_013 );
    exp.push_back( test_exp_014 );
    ans.push_back( test_ans_014 );
    exp.push_back( test_exp_015 );
    ans.push_back( test_ans_015 );
    exp.push_back( test_exp_016 );
    ans.push_back( test_ans_016 );
    exp.push_back( test_exp_017 );
    ans.push_back( test_ans_017 );
    exp.push_back( test_exp_018 );
    ans.push_back( test_ans_018 );
    exp.push_back( test_exp_019 );
    ans.push_back( test_ans_019 );
    exp.push_back( test_exp_020 );
    ans.push_back( test_ans_020 );
    exp.push_back( test_exp_021 );
    ans.push_back( test_ans_021 );
    exp.push_back( test_exp_022 );
    ans.push_back( test_ans_022 );
    exp.push_back( test_exp_023 );
    ans.push_back( test_ans_023 );
    exp.push_back( test_exp_024 );
    ans.push_back( test_ans_024 );
    exp.push_back( test_exp_025 );
    ans.push_back( test_ans_025 );
    exp.push_back( test_exp_026 );
    ans.push_back( test_ans_026 );
    exp.push_back( test_exp_027 );
    ans.push_back( test_ans_027 );
    exp.push_back( test_exp_028 );
    ans.push_back( test_ans_028 );
 
    /*
    ---------------------
    Initialize calculator
    --------------------- */
    parameters = init_calculator();

    /*
    -----------------------------------------
    Execute test cases and verify the results
    ----------------------------------------- */   
    for ( i = 0; i < exp.size(); ++i )
    {
        command = exp[i].c_str( );
        result = calc.value( command, parameters, CurrentPart, CurrentLimitSwitch, CurrentProfile );
        double_to_tchar( buf, result, 3 );
        result_str = buf;
        
        if ( result_str == ans[i].c_str( ) )
        {
            pass_fail.clear( );
            pass_fail = TEXT( "PASS: " ) + result_str;
            //MessageBox( NULL, exp[i].c_str( ), pass_fail.c_str( ), MB_OK );
        }
        else
        {
            pass_fail.clear( );
            pass_fail = TEXT( "FAIL: " ) + result_str;
            all_passed = false;
            MessageBox( NULL, exp[i].c_str( ), pass_fail.c_str( ), MB_OK );            
        }      
    }  

    if (all_passed)
    {
        MessageBox(NULL, TEXT("Hooray!"), TEXT("All Tests Passed"), MB_OK);
    }
}

static void test_sensor_response()
{
	NAME_CLASS sensor_file;	
    NAME_CLASS increasing_file;
    NAME_CLASS decreasing_file;
    FILE_CLASS fin;
	ANALOG_SENSOR asensor;

    sensor_file = TestDataDir;
    sensor_file += TEXT("\\asensor_jay.csv");
    if (!sensor_file.file_exists())
    {
        MessageBox(NULL, sensor_file.text(), TEXT("Error opening file"), MB_OK);
        return;
    }
	asensor.get(sensor_file);

	/* read in test input file */
	/* test input file is a csv with the following columns: */
	/* voltage,answer,value,answer... */  
    increasing_file = TestDataDir;
    increasing_file += TEXT("\\test_asensor_input_increasing.csv");   
	if (!(fin.open_for_read(increasing_file)))
	{
		MessageBox(NULL, increasing_file.text(), TEXT("Error opening file"), MB_OK);
		return;
	}
	
	TCHAR *line = fin.readline();
	STRING_CLASS str = line;
	double test_voltage;
	double test_value;
	double response;
	double answer;
	
	/* test linear interpolation */
	bool pass = true;

	/* test sensor with increasing response */
	asensor.find(TEXT("Test Sensor Increasing"));
	while (str.next_field())
	{
		/* test converted_value increasing */
		test_voltage = str.real_value();
		str.next_field();
		answer = str.real_value();
		response = asensor.converted_value(test_voltage);
		
		if (response != answer)
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
        MessageBox(NULL, TEXT("PASSED"), TEXT("Sensor Response Increasing Test"), MB_OK);
	}
	else
	{
        MessageBox(NULL, TEXT("FAILED"), TEXT("Sensor Response Increasing Test"), MB_OK);
	}

	fin.close();
    decreasing_file = TestDataDir;
    decreasing_file += TEXT("\\test_asensor_input_decreasing.csv");  
    if (!(fin.open_for_read(decreasing_file)))
	{
        MessageBox(NULL, decreasing_file.text(), TEXT("Error opening file"), MB_OK);
		return;
	}

	/* test sensor with decreasing response */
	pass = true;
    asensor.find(TEXT("Test Sensor Decreasing"));
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
        MessageBox(NULL, TEXT("PASSED"), TEXT("Sensor Response Decreasing Test"), MB_OK);
	}
	else
	{
        MessageBox(NULL, TEXT("FAILED"), TEXT("Sensor Response Decreasing Test"), MB_OK);
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
*                              DIR_BROWSER                             *
***********************************************************************/
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED)
    {
        std::string tmp = (const char *)lpData;
        std::cout << "path: " << tmp << std::endl;
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }

    return 0;
}

std::string BrowseFolder(std::string saved_path)
{
    TCHAR path[MAX_PATH];

    const char * path_param = saved_path.c_str();

    BROWSEINFO bi = { 0 };
    bi.lpszTitle = ("Browse for folder...");
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = (LPARAM)path_param;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != 0)
    {
        //get the name of the folder and put it in path
        SHGetPathFromIDList(pidl, path);

        //free memory used
        IMalloc * imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc)))
        {
            imalloc->Free(pidl);
            imalloc->Release();
        }

        return path;
    }

    return "";
}

/***********************************************************************
*                              evaluate_command                        *
***********************************************************************/
void evaluate_command( STRING_CLASS & cmd, STRING_CLASS & result )
{      
    double     * parameters;  
    double       n;
    FTCALC_CLASS ftcalc;  
    TCHAR buf[100];

    /*
    ---------------------
    Initialize calculator
    --------------------- */
    parameters = init_calculator();  
   
    n = ftcalc.value( cmd, parameters, CurrentPart, 
        CurrentLimitSwitch, CurrentProfile );
    double_to_tchar(buf, n, 3);

    result = buf;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;
int id;
STRING_CLASS command;
STRING_CLASS result;
std::string path;
NAME_CLASS path_str;
std::string box_msg;
REGEX cmp_split_ex( TEXT( "(.*)\\\\data\\\\(.*)\\\\(.*)" ) );
REGMATCH sm;

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
                    command.get_text( hWnd, MY_TBOX );
                    evaluate_command( command, result );
                    result.set_text( hWnd, RESULTS_BOX );
                }
                return 0;
            
            case ID_OPEN_PARTDIR:                
                path = BrowseFolder(TEXT(""));
                
                if (regex_match(path, sm, cmp_split_ex))
                {
                    computer_name = sm[1].str();
                    machine_name = sm[2].str();
                    part_name = sm[3].str();
                    box_msg.append(TEXT("Computer: "));
                    box_msg.append(computer_name);
                    box_msg.append(TEXT("\nMachine: "));
                    box_msg.append(machine_name);
                    box_msg.append(TEXT("\nPart: "));
                    box_msg.append(TEXT(part_name));
                }

                MessageBox(NULL, box_msg.c_str(), TEXT( "BrowseFolder Result" ), MB_OK);
                break;

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
    605, 400,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MyTbox = CreateWindow(
    TEXT("edit"),
    TEXT("0"),
    WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT | ES_MULTILINE,
    //WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
    0, 0,
    500, 300,
    w,
    (HMENU) MY_TBOX,
    MainInstance,
    NULL
    );

ResultBox = CreateWindow(
    TEXT("edit"),
    TEXT("0"),
    WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
    //WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
    0, 300,
    500, 20,
    w,
    (HMENU) RESULTS_BOX,
    MainInstance,
    NULL
    );

OkButton = CreateWindow (
    TEXT("button"),
    TEXT("OK"),
    WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
    501, 0,
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

TCHAR exe_path[500];
GetModuleFileName(NULL, exe_path, 500);
TestDataDir = exe_path;
TestDataDir.remove_ending_filename();
TestDataDir.remove_ending_filename();

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
