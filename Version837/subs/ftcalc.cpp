#include <windows.h>
#include <regex>

/***********************************************************************
*  Note: The ith entry is the one for the ith parameter.               *
*  There is room for all the parameters but, obviously, only the       *
*  ones that are ftanalog parameters are used for anything.            *
***********************************************************************/
#include "..\include\visiparm.h"
#include "..\include\chars.h"
#include "..\include\nameclas.h"
#include "..\include\fileclas.h"
#include "..\include\part.h"
#include "..\include\ftcalc.h"
#include "..\include\subs.h"

using namespace std;

static const double pi = 3.14159265359;

/******************************************************************************
*                                 max_value                                   *
*  STRING_CLASS args CSV format of arguments from parameter calculator        *
*  FTII_PROFILE profile The current ft2 profile                               *
*  PART_CLASS part The current part                                           *
*  retval double maximum value with in the specified range of data set        *
******************************************************************************/
static double max_value( STRING_CLASS & args, FTII_PROFILE & profile, PART_CLASS & part )
{
    double       start_pos;
    double       end_pos;
    int          start_index;
    int          end_index;
    int          i;
    double       curr_vel;
    double       max_value;
    STRING_CLASS source;
    STRING_CLASS s;

    max_value = 0.0;
    start_pos = 0.0;
    end_pos = 0.0;

    if ( args.next_field() )
    {
        /*
        --------------------
        Copy the data source
        -------------------- */
        source = args;
        source.strip();

        /*
        -------------------------------
        Get the start and end positions
        ------------------------------- */
        if ( args.next_field() )
        {
            s = args;
            s.strip();
            start_pos = s.real_value();
            if ( args.next_field() )
            {
                s = args;
                s.strip();
                end_pos = s.real_value();
            }
        }
    }

    if ( end_pos == 0.0 )
        return max_value;

    start_index = profile.index_from_position( part.x4_from_dist(start_pos) );
    end_index = profile.index_from_position( part.x4_from_dist(end_pos) );

    if ( source == TEXT("VEL") )
    {
        if (profile.velocity == NULL)
            return max_value;

        max_value = part.velocity_from_x4( profile.velocity[start_index] );

        for ( i = start_index + 1; i <= end_index; ++i )
        {
            curr_vel = part.velocity_from_x4( profile.velocity[i] );
            if ( curr_vel > max_value )
            {
                max_value = curr_vel;
            }
        }
    }

    return max_value;
}

/******************************************************************************
*                                 min_value                                   *
*  STRING_CLASS args CSV format of arguments from parameter calculator        *
*  FTII_PROFILE profile The current ft2 profile                               *
*  PART_CLASS part The current part                                           *
*  retval double minimum value with in the specified range of data set        *
******************************************************************************/
static double min_value( STRING_CLASS & args, FTII_PROFILE & profile, PART_CLASS & part )
{
    double       start_pos;
    double       end_pos;
    int          start_index;
    int          end_index;
    int          i;
    double       curr_vel;
    double       min_value;
    STRING_CLASS source;
    STRING_CLASS s;

    min_value = 0.0;
    start_pos = 0.0;
    end_pos = 0.0;

    if ( args.next_field() )
    {
        /*
        --------------------
        Copy the data source
        -------------------- */
        source = args;
        source.strip();

        /*
        -------------------------------
        Get the start and end positions
        ------------------------------- */
        if ( args.next_field() )
        {
            s = args;
            start_pos = s.real_value();
            if (args.next_field())
            {
                s = args;
                s.strip();
                end_pos = s.real_value();
            }
        }
    }

    if ( end_pos == 0.0 )
        return min_value;


    start_index = profile.index_from_position(part.x4_from_dist(start_pos));
    end_index = profile.index_from_position(part.x4_from_dist(end_pos));

    if ( source == TEXT("VEL") )
    {
        if ( profile.velocity == NULL )
            return min_value;

        min_value = part.velocity_from_x4( profile.velocity[start_index] );

        for ( i = start_index + 1; i <= end_index; ++i )
        {
            curr_vel = part.velocity_from_x4( profile.velocity[i] );
            if ( curr_vel < min_value )
            {
                min_value = curr_vel;
            }
        }
    }

    return min_value;
}

/******************************************************************************
*                                   target_pos                                *
*  STRING_CLASS args CSV format of arguments from parameter calculator        *
*  FTII_PROFILE profile The current ft2 profile                               *
*  PART_CLASS part The current part                                           *
*  retval double The position where the data set first crosses the threhold   *
******************************************************************************/
static double target_pos( STRING_CLASS & args, FTII_PROFILE & profile, PART_CLASS & part)
{
    STRING_CLASS        s;
    STRING              source;
    double              threshold;
    BOOLEAN             increasing;
    double              start_pos;
    double              curr_data;
    int                 i;
    int                 start_index;
    REGMATCH            sm;
    static const REGEX  dio_ex( TEXT("DIO(\\d+)") );
    STRING_CLASS        dio_str;
    unsigned int        dio_wire;
    unsigned int        dio_mask;
    unsigned int        u;
    int                 i1;

    /*
    --------------------------
    Get first arg: data source
    -------------------------- */
    if ( args.next_field() )
    {
        s = args;
        s.strip();
        source = s.text();
    }
    else
        return 0.0;

    /*
    -------------------------
    Get second arg: threshold
    ------------------------- */
    if ( args.next_field() )
    {
        s = args;
        s.strip();
        threshold = s.real_value();
    }
    else
        return 0.0;

    /*
    -------------------------
    Get third arg: increasing
    ------------------------- */
    if ( args.next_field() )
    {
        s = args;
        s.strip();
        increasing = s.boolean_value();
    }
    else
        return 0.0;

    /*
    ------------------------------
    Get fourth arg: start position
    ------------------------------ */
    if ( args.next_field() )
    {
        s = args;
        s.strip();
        start_pos = s.real_value();
    }
    else
        return 0.0;

    if ( source.compare(TEXT("VEL")) == 0 )
    {
        if ( profile.velocity == NULL )
            return 0.0;

        start_index = profile.index_from_position( part.x4_from_dist(start_pos) );

        for ( i = start_index; i < profile.np; ++i )
        {
            curr_data = part.velocity_from_x4( profile.velocity[i] );

            if ( increasing && curr_data > threshold )
                return part.dist_from_x4( profile.position[i] );

            if ( !increasing && curr_data < threshold )
                return part.dist_from_x4( profile.position[i] );
        }

        return 0.0;
    }
    else if ( regex_match(source, sm, dio_ex) )
    {
        /*
        -----------------------------------------
        Extract channel number from DIO parameter
        ----------------------------------------- */
        dio_str = sm[1].str().c_str();
        dio_wire = ( unsigned )dio_str.int_value();
        dio_mask = mask_from_wire( dio_wire );

        /*
        -------------------------------------------
        I need to look for the opposite level first
        ------------------------------------------- */
        if (increasing)
            u = 0;
        else
            u = dio_mask;

        /*
        -------------------------------------
        Set the default transition point at 0
        ------------------------------------- */
        i1 = 0;

        for ( i = 0; i < profile.np; i++ )
        {
            if ( (profile.isw1[i] & dio_mask) == u )
                break;
        }

        u ^= dio_mask;
        i++;
        while ( i < profile.np )
        {
            if ( (profile.isw1[i] & dio_mask) == u )
            {
                i1 = i;
                break;
            }
            i++;
        }

        return part.dist_from_x4( profile.position[i1] );
    }
    else
    {
        return 0.0;
    }
}

/******************************************************************************
*                                   is_operator                               *
*  retval True if str is a supported operator.                                *
******************************************************************************/
static BOOLEAN is_operator( STRING str )
{
    if ( str.empty() )
        return FALSE;

    STRING operators = TEXT( "^*/+-" );
    size_t found = operators.find( str[0] );

    if ( found == STRING::npos )
        return FALSE;
    else
        return TRUE;
}

/******************************************************************************
*                               compare_operators                             *
*  TCHAR op1 The first operator to compare.                                   *
*  TCHAR op2 The second operator to compare.                                  *
*  Return 0 if equal, -1 if op2 is less than op1, and 1 if op2 is greater     *
*    than op1.                                                                *
******************************************************************************/
static int compare_operators( TCHAR op1, TCHAR op2 )
{
    if ( op1 == CaretChar && op2 != CaretChar )
    {
        return -1;
    }
    else if ( op1 != CaretChar && op2 == CaretChar )
    {
        return 1;
    }
    else if ( (op1 == AsterixChar || op1 == ForwardSlashChar) && (op2 == PlusChar || op2 == MinusChar) )
    {
        return -1;
    }
    else if ( (op1 == PlusChar || op1 == MinusChar) && (op2 == AsterixChar || op2 == ForwardSlashChar) )
    {
        return 1;
    }
    return 0;
}

/******************************************************************************
*                                  is_unary_operand                           *
*  Supports the unary '-' operator                                            *
*  STRING token The STRING in question.                                       *
*  retval BOOLEAN True if token is a unary operand.                           *
******************************************************************************/
static BOOLEAN is_unary_operand( STRING token )
{
    static const REGEX operand_ex( TEXT("-?\\d+|-?\\d*.\\d+") );

    if ( token.empty() )
        return false;

    if ( regex_match(token, operand_ex) )
        return true;
    else
        return false;
}

/******************************************************************************
*                               infix_to_postfix                              *
*  queue<STRING> *infix The infix expression to be converted.                 *
*  queue<STRING> *postfix The expression converted to postfix form.           *
******************************************************************************/
static void infix_to_postfix( queue<STRING> * infix, queue<STRING> * postfix )
{
    stack <TCHAR> opstack;
    STRING op_str;
    STRING token;

    while ( !infix->empty() )
    {
        token.clear();
        token = infix->front();
        infix->pop();

        if ( is_unary_operand(token) )
            postfix->push( token );
        else if ( token[0] == LeftParen )
            opstack.push( token[0] );
        else if ( token[0] == RightParen )
        {
            while ( !opstack.empty() )
            {
                if ( opstack.top() == LeftParen )
                {
                    opstack.pop();
                    break;
                }
                op_str.clear();
                op_str = opstack.top();
                postfix->push( op_str );
                opstack.pop();
            }
        }
        else if ( is_operator(token) )
        {
            while ( !opstack.empty() && opstack.top() != LeftParen && compare_operators(opstack.top(), token[0]) <= 0 )
            {
                op_str.clear();
                op_str = opstack.top();
                postfix->push( op_str );
                opstack.pop();
            }
            opstack.push( token[0] );
        }
    }

    while ( !opstack.empty() )
    {
        op_str.clear();
        op_str = opstack.top();
        opstack.pop();

        if ( op_str[0] != LeftParen )
            postfix->push( op_str );
    }

    return;
}

/******************************************************************************
*                                  do_binary_op                               *
*  STRING op The operation to perform on the operands.                        *
*  STRING x The left operand.                                                 *
*  STRING y The right operand.                                                *
*  retval STRING The result of the operation.                                 *
******************************************************************************/
static STRING do_binary_op( STRING op, STRING x, STRING y )
{
    double xd = stod( x );
    double yd = stod( y );
    double result;
    STRING result_str;

    switch ( op[0] )
    {
    case TEXT( '^' ):
        result = pow(xd, yd);
        break;
    case TEXT( '*' ):
        result = xd * yd;
        break;
    case TEXT( '/' ):
        result = xd / yd;
        break;
    case TEXT( '+' ):
        result = xd + yd;
        break;
    case TEXT( '-' ):
        result = xd - yd;
        break;
    default:
        result = 0;
        break;
    }

    result_str = TO_STRING( result );

    return result_str;
}

/******************************************************************************
*                              evaluate_postfix                               *
*  queue<STRING> *postfix The postfix expression in queue form.               *
*  retval STRING The result of the evaluated postfix expression.              *
******************************************************************************/
static STRING evaluate_postfix( queue<STRING> * postfix )
{
    stack<STRING>   stack;
    STRING          token;
    STRING          x;
    STRING          y;

    while ( !postfix->empty() )
    {
        token = postfix->front();
        postfix->pop();

        if ( is_unary_operand(token) )
            stack.push( token );
        else if ( is_operator(token) )
        {
            y.clear();
            y = stack.top();
            stack.pop();
            x.clear();
            x = stack.top();
            stack.pop();
            stack.push( do_binary_op(token, x, y) );
        }
    }

    x.clear();
    x = stack.top();
    stack.pop();

    return x;
}

/***********************************************************************
*                       SOURCE_DATA::SOURCE_DATA                       *
***********************************************************************/
SOURCE_DATA::SOURCE_DATA()
{
position = 0;
velocity = 0;
time     = 0;
is_set   = false;
}


/***********************************************************************
*                       ~SOURCE_DATA::SOURCE_DATA                      *
***********************************************************************/
SOURCE_DATA::~SOURCE_DATA()
{
}

/***********************************************************************
*                          SOURCE_DATA::CLEAR                          *
***********************************************************************/
void SOURCE_DATA::clear()
{
position = 0;
velocity = 0;
time     = 0;
is_set   = false;
}

/***********************************************************************
*                        SOURCE_DATA::OPERATOR=                        *
***********************************************************************/
void SOURCE_DATA::operator=( const SOURCE_DATA & sorc )
{
position = sorc.position;
velocity = sorc.velocity;
time     = sorc.time;
is_set   = sorc.is_set;
}

/***********************************************************************
*                           SOURCE_DATA::SET                           *
***********************************************************************/
void SOURCE_DATA::set( double new_position, double new_time, double new_velocity )
{
position = new_position;
time     = new_time;
velocity = new_velocity;
is_set   = true;
}

/***********************************************************************
*                         FTCALC_CLASS::FTCALC_CLASS                   *
***********************************************************************/
FTCALC_CLASS::FTCALC_CLASS()
{
}

/***********************************************************************
*                    FTCALC_CLASS::CLEAR_SOURCE_DATA                   *
***********************************************************************/
void FTCALC_CLASS::clear_source_data()
{
int i;

for ( i=0; i<MAX_PARMS; i++ )
    {
    sourcedata[i].position = 0.0;
    sourcedata[i].velocity = 0.0;
    sourcedata[i].time     = 0.0;
    sourcedata[i].is_set   = false;
    }
}

/***********************************************************************
*                         FTCALC_CLASS::CLEANUP                        *
***********************************************************************/
void FTCALC_CLASS::cleanup()
{
int i;
for ( i=0; i<MAX_PARMS; i++ )
    cmd[i].empty();
}

/***********************************************************************
*                         FTCALC_CLASS::VALUE                          *
***********************************************************************/
double FTCALC_CLASS::value( int dest_parameter_number, double * parameters, PART_CLASS & part, LIMIT_SWITCH_CLASS & limits, FTII_PROFILE & profile )
{
int i;

if ( dest_parameter_number <= 0 || dest_parameter_number > MAX_PARMS )
    return 0.0;

i = dest_parameter_number - 1;
if ( cmd[i].isempty() )
    return 0.0;

return value( cmd[i], parameters, part, limits, profile );
}

/***********************************************************************
*                         FTCALC_CLASS::VALUE                          *
***********************************************************************/
double FTCALC_CLASS::value( STRING_CLASS & command, double * parameters, PART_CLASS & part, LIMIT_SWITCH_CLASS & limits, FTII_PROFILE & profile )
{
    int i;
    NAME_CLASS  name;
    FILE_CLASS  file;
    TCHAR     * cp;
    double      result;
    STRING      result_str;
    std::map <STRING, STRING>::iterator vars_it;

    limit_switch = limits;

    /*
    -----------------------------------------------
    Read in the parameters from the calculator file
    ----------------------------------------------- */
    name.get_calc_file_name( part.computer, part.machine, part.name );

    if (file.open_for_read(name))
    {
        for (i = 0; i <= CALC_CYLINDER_DIAMETER_INDEX; ++i)
        {
            cp = file.readline();
            /*
            --------------------------------------
            Gate Area is the 12th line of calc.dat
            -------------------------------------- */
            if (i == CALC_GATE_AREA_INDEX)
            {
                gate_area = extdouble(cp);
            }

            /*
            ----------------------------------------------
            Hydraulic Cyl Dia is the 15th line of calc.dat
            ---------------------------------------------- */
            else if (i == CALC_GATE_AREA_INDEX)
            {
                hydraulic_cyl_dia = extdouble(cp);
            }
        }

        file.close();
        hydraulic_cyl_area = pi*pow( hydraulic_cyl_dia / 2.0, 2.0 );
    }

    parameter_list = parameters;
    current_part = part;

    vars.clear();

    /*
    ---------------------------------
    Stray '.' at end of string is bad
    --------------------------------- */
    cp = command.last_char();
    if( *cp == PeriodChar )
        *cp = NullChar;

    result_str = parse_lines( command.text(), profile, part );
    vars_it = vars.find( TEXT("result") );

    /*
    -----------------------------------------
    Use value of result variable if it exists
    ----------------------------------------- */
    if ( vars_it != vars.end() )
    {
        result_str = vars_it->second;
        result = stod( result_str );
        return result;
    }

    /*
    ----------------------------------------------
    Otherwise use value of last variable statement
    ---------------------------------------------- */
    else if ( !result_str.empty() )
    {
        result = stod( result_str );
        return result;
    }

    /*
    ---------------------------
    Could not evaluate a result
    --------------------------- */
    else
    {
        //MessageBox( NULL, TEXT( "No result calculated." ), TEXT( "Warning" ), MB_OK );
        return 0.0;
    }
}

/***********************************************************************
*                         FTCALC_CLASS::REMOVE                         *
***********************************************************************/
BOOLEAN FTCALC_CLASS::remove( int index_to_remove )
{
int i;
int n;

n = MAX_PARMS;
n--;
if ( index_to_remove < 0 || index_to_remove > n )
    return FALSE;

i = index_to_remove;
while ( i < n )
    {
    cmd[i]        = cmd[i+1];
    sourcedata[i] = sourcedata[i+1];
    i++;
    }
cmd[n].empty();
sourcedata[n].clear();

return TRUE;
}

/***********************************************************************
*                           FTCALC_CLASS::LOAD                         *
***********************************************************************/
BOOLEAN FTCALC_CLASS::load( PARAMETER_CLASS & params, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
int          i;
FILE_CLASS   f;
NAME_CLASS   s;
STRING_CLASS cmd_replaced;
STRING       cmd_str;
NAME_CLASS   p;
DWORD        file_size;
DWORD        bytes_read;

cleanup();
s.get_ftcalc_csvname( computer, machine, part );

if ( s.file_exists() )
    {
    /*
    ---------------
    Old load method
    --------------- */
    if ( f.open_for_read(s) )
        {
        while ( true )
            {
            s = f.readline();
            cmd_replaced.null();

            if ( s.isempty() )
                break;
            if ( s.countchars(CommaChar) >= 1 )
                {
                s.next_field();
                i = s.int_value();

                if ( i > 0 && i<= MAX_PARMS )
                    {
                    while ( s.next_field() )
                        {
                        /*
                        ---------------------
                        Replace \r\n with ','
                        --------------------- */
                        cmd_str = s.text();
                        cmd_replaced += cmd_str.c_str();
                        cmd_replaced += CrLfString;
                        }

                    cmd[i-1] = cmd_replaced;
                    }
                }
            }
        f.close();
        return TRUE;
        }
    else
        {
        return FALSE;
        }
    }
else
    {
    /*
    ---------------
    New load method
    --------------- */
    for ( i = 0; i < MAX_PARMS; i++ )
        {
        if ( params.parameter[i].input.type == FT_CALC_INPUT )
            {
            p.get_ftcalc_param_file_name( computer, machine, part, params.parameter[i].name );

            if ( p.file_exists() && f.open_for_read(p) )
                {
                f.get_file_size( file_size );
                cmd[i].upsize( file_size + 1 );
                bytes_read = f.readbinary( cmd[i].text(), file_size );
                cmd[i].text()[bytes_read] = NullChar;
                f.close();
                }
            }
        }
    }

return TRUE;
}

/***********************************************************************
*                          FTCALC_CLASS::SAVE                          *
***********************************************************************/
BOOLEAN FTCALC_CLASS::save( PARAMETER_CLASS & params, STRING_CLASS & computer, STRING_CLASS & machine, STRING_CLASS & part )
{
int          i;
FILE_CLASS   f;
STRING_CLASS s;
NAME_CLASS   p;

for ( i = 0; i < MAX_PARMS; i++ )
    {
    if ( cmd[i].isempty() )
        continue;

    p.get_ftcalc_param_file_name( computer, machine, part, params.parameter[i].name );

    if ( f.open_for_write(p) )
        {
        if ( !f.writeline(cmd[i]) )
            {
            f.close();
            return FALSE;
            }
        f.close();
        }
    else
        {
        return FALSE;
        }

    }

/*
--------------------------------
Delete old style FTCALC.CSV file
-------------------------------- */
p.get_ftcalc_csvname( computer, machine, part );

if ( p.file_exists() )
    {
    p.delete_file();
    }

return TRUE;
}

/******************************************************************************
*                           FTCALC_CLASS::parse_operand                       *
*  STRING input The entire command STRING being parsed.                       *
*  int i The index to start parsing for an operand.                           *
*  STRING &operand The parsed operand will be copied here.                    *
*  retval unsingned int The number of chars the calling functions needs       *
*  to advance the parsing of input STRING.                                    *
******************************************************************************/
unsigned int FTCALC_CLASS::parse_operand( const STRING input, int i, STRING & operand )
{
    unsigned int si = i;
    unsigned int ni = i;
    int param_index;
    double x;
    STRING input_sub = input.substr( i );
    REGMATCH sm;
    static const REGEX numeric_ex( TEXT("C?(\\d*\\.?\\d*).*"), std::regex_constants::icase );
    std::map<STRING, STRING>::iterator vars_it;

    /*
    --------
    Original
    -------- */
    static const REGEX variable_name_ex( TEXT("([[:alpha:]]+[_[:alnum:]]*).*") );
    static const REGEX impact_time_ex( TEXT("IST.*"), std::regex_constants::icase );
    static const REGEX min_stroke_length_ex( TEXT("MSL.*"), std::regex_constants::icase );
    static const REGEX parameter_ex( TEXT("P([0-9]{1,3}).*"), std::regex_constants::icase );
    static const REGEX plunger_area_ex( TEXT("PA.*"), std::regex_constants::icase );
    static const REGEX plunger_diameter_ex( TEXT("PD.*"), std::regex_constants::icase );
    static const REGEX position_ex( TEXT("POS([0-9]{1,3}).*"), std::regex_constants::icase );
    static const REGEX time_ex( TEXT("TIM([0-9]{1,3}).*"), std::regex_constants::icase );
    static const REGEX total_stroke_length_ex( TEXT("TSL.*"), std::regex_constants::icase );
    static const REGEX velocity_ex( TEXT("VEL([0-9]{1,3}).*"), std::regex_constants::icase );

    /*
    -----
    Basic
    ----- */
    static const REGEX eos_vel_ex( TEXT("EOS_VEL.*"), std::regex_constants::icase );

    /*
    --------
    Advanced
    -------- */
    static const REGEX p1top3_ex( TEXT("p1top3.*"), std::regex_constants::icase );
    static const REGEX p2top3_ex( TEXT("p2top3.*"), std::regex_constants::icase );
    static const REGEX min_csfs_pos_ex( TEXT("Min_CSFS_Pos.*|Min CSFS Pos.*"), std::regex_constants::icase );
    static const REGEX min_csfs_vel_ex( TEXT("Min_CSFS_Vel.*|Min CSFS Vel.*"), std::regex_constants::icase );
    static const REGEX vel_for_rise_ex( TEXT("Vel_for_Rise.*|Vel for Rise.*"), std::regex_constants::icase );
    static const REGEX time_for_intens_pres_ex( TEXT("Time_for_Intens_Pres.*|Time for Intens Pres.*"), std::regex_constants::icase );
    static const REGEX pres_for_response_time_ex( TEXT("Pres_for_Response_Time.*|Pres for Response Time.*"), std::regex_constants::icase );

    /*
    -----------------------------
    Positions from Limit Switches
    ----------------------------- */
    static const REGEX limit_switches_ex( TEXT("LIM([123456]).*"), std::regex_constants::icase );

    /*
    ---------------------
    Calculator Parameters
    --------------------- */
    static const REGEX gate_area_ex( TEXT("Gate_Area.*|Gate Area.*"), std::regex_constants::icase );
    static const REGEX hydraulic_cyl_area_ex( TEXT("Hydraulic_Cyl_Area.*|Hydraulic Cyl Area.*"), std::regex_constants::icase );
    static const REGEX hydraulic_cyl_dia_ex( TEXT("Hydraulic_Cyl_Dia.*|Hydraulic Cyl Dia.*"), std::regex_constants::icase );

    /*
    ---------------------------------------
    Check if the operand matches a variable
    --------------------------------------- */
    if ( regex_match(input_sub, sm, variable_name_ex) )
    {
        vars_it = vars.find( sm[1].str() );
        if ( vars_it != vars.end() )
        {
            operand = vars_it->second;
            return ( sm[1].str().length() );
        }
    }

    /*
    ------------------------------------
    Check for impact time parameter: IST
    ------------------------------------ */
    if ( regex_match(input_sub, sm, impact_time_ex) )
    {
        if ( sp[0].value == EmptyString )
        {
            //MessageBox( NULL, sp[0].value.text(), TEXT("Invalid IST"), MB_OK );
            return 0;
        }
        else
        {
            operand = sp[0].value.text();
            return (3);
        }
    }

    /*
    ----------------------------------------------
    Check for minumum stroke length parameter: MSL
    ---------------------------------------------- */
    if ( regex_match(input_sub, sm, min_stroke_length_ex) )
    {
        operand = TO_STRING( current_part.min_stroke_length );
        return 3;
    }

    /*
    -------------------------------
    Check for parameter value: Pxxx
    ------------------------------- */
    if ( regex_match(input_sub, sm, parameter_ex) )
    {
        param_index = stoi( sm[1].str() );
        if ( param_index < 1 || param_index >= MAX_PARMS )
            return 0;
        operand = TO_STRING( parameter_list[param_index-1] );
        return ( sm[1].str().length() + 1 );
    }

    /*
    --------------------------
    Check for plunger area: PA
    -------------------------- */
    if ( regex_match(input_sub, sm, plunger_area_ex) )
    {
        x = current_part.plunger_diameter;
        x *= x;
        x *= pi;
        x /= 4.0;
        operand = TO_STRING( x );
        return 2;
    }

    /*
    ------------------------------
    Check for plunger diameter: PA
    ------------------------------ */
    if ( regex_match(input_sub, sm, plunger_diameter_ex) )
    {
        operand = TO_STRING( current_part.plunger_diameter );
        return 2;
    }

    /*
    ------------------------------------
    Check for position parameter: POSxxx
    ------------------------------------ */
    if ( regex_match(input_sub, sm, position_ex) )
    {
        param_index = stoi( sm[1].str() );

        if ( param_index >= 0 && param_index < MAX_PARMS )
        {
            operand = TO_STRING( sourcedata[param_index].position );
            return ( sm[1].str().length() + 3 );
        }
        else
        {
            return 0;
        }
    }

    /*
    ------------------------------------
    Check for position parameter: TIMxxx
    ------------------------------------ */
    if ( regex_match(input_sub, sm, time_ex) )
    {
        param_index = stoi( sm[1].str() );

        if ( param_index >= 0 && param_index < MAX_PARMS )
        {
            operand = TO_STRING( sourcedata[param_index].position );
            return ( sm[1].str().length() + 3 );
        }
        else
        {
            return 0;
        }
    }

    /*
    ---------------------------------
    Check for position parameter: TSL
    --------------------------------- */
    if ( regex_match(input_sub, sm, total_stroke_length_ex) )
    {
        operand = TO_STRING( current_part.total_stroke_length );
        return ( sm[1].str().length() + 3 );
    }

    /*
    ------------------------------------
    Check for position parameter: VELxxx
    ------------------------------------ */
    if ( regex_match(input_sub, sm, velocity_ex) )
    {
        param_index = stoi( sm[1].str() );

        if ( param_index >= 0 && param_index < MAX_PARMS )
        {
            operand = TO_STRING( sourcedata[param_index].velocity );
            return ( sm[1].str().length() + 3 );
        }
        else
        {
            return 0;
        }
    }

    /*
    ----------------------------
    Check for parameter: EOS_VEL
    ---------------------------- */
    if ( regex_match(input_sub, sm, eos_vel_ex) )
    {
        operand = TO_STRING( current_part.eos_velocity );
        return 7;
    }

    /*
    ---------------------------
    Check for parameter: p1top3
    --------------------------- */
    if ( regex_match(input_sub, sm, p1top3_ex) )
    {
        operand = TO_STRING( current_part.sleeve_fill_distance );
        return 6;
    }

    /*
    ---------------------------
    Check for parameter: p2top3
    --------------------------- */
    if ( regex_match(input_sub, sm, p2top3_ex) )
    {
        operand = TO_STRING( current_part.runner_fill_distance );
        return 6;
    }

    /*
    ---------------------------------
    Check for parameter: Min CSFS Pos
    --------------------------------- */
    if ( regex_match(input_sub, sm, min_csfs_pos_ex) )
    {
        operand = TO_STRING( current_part.csfs_min_position );
        return 12;
    }

    /*
    ---------------------------------
    Check for parameter: Min CSFS Vel
    --------------------------------- */
    if ( regex_match(input_sub, sm, min_csfs_vel_ex) )
    {
        operand = TO_STRING( current_part.csfs_min_velocity );
        return 12;
    }

    /*
    ---------------------------------
    Check for parameter: Vel for Rise
    --------------------------------- */
    if ( regex_match(input_sub, sm, vel_for_rise_ex) )
    {
        operand = TO_STRING( current_part.csfs_rise_velocity );
        return 12;
    }

    /*
    -----------------------------------------
    Check for parameter: Time for Intens Pres
    ----------------------------------------- */
    if ( regex_match(input_sub, sm, time_for_intens_pres_ex) )
    {
        operand = TO_STRING( current_part.time_for_intens_pres );
        return 20;
    }

    /*
    -------------------------------------------
    Check for parameter: Pres for Response Time
    ------------------------------------------- */
    if ( regex_match(input_sub, sm, pres_for_response_time_ex) )
    {
        operand = TO_STRING( current_part.pres_for_response_time );
        return 22;
    }

    /*
    -------------------------
    Check for parameter: LIMx
    ------------------------- */
    if ( regex_match(input_sub, sm, limit_switches_ex) )
    {
        param_index = stoi( sm[1].str() );
        operand = limit_switch.pos( param_index - 1 ).text();

        if ( operand.empty() )
        {
            return 0;
        }
        else
        {
            return 4;
        }
    }

    /*
    ------------------------------
    Check for parameter: Gate Area
    ------------------------------ */
    if ( regex_match(input_sub, sm, gate_area_ex) )
    {
        operand = TO_STRING( gate_area );
        return 9;
    }

    /*
    --------------------------------------
    Check for parameter: Hydraulic Cyl Dia
    -------------------------------------- */
    if ( regex_match(input_sub, sm, hydraulic_cyl_dia_ex) )
    {
        operand = TO_STRING( hydraulic_cyl_dia );
        return 17;
    }

    /*
    ---------------------------------------
    Check for parameter: Hydraulic Cyl Area
    --------------------------------------- */
    if ( regex_match(input_sub, sm, hydraulic_cyl_area_ex) )
    {
        operand = TO_STRING( hydraulic_cyl_area );
        return 18;
    }

    /*
    -----------------------------------------------
    Check for numeric constants, may start with 'C'
    TODO: Why does this REGEX have to be last?
    ----------------------------------------------- */
    if ( regex_match(input_sub, sm, numeric_ex) )
    {
        operand = sm[1].str();
        if ( input_sub[0] == CChar || input_sub[0] == CCharLow )
        {
            return ( operand.length() + 1 );
        }
        else
        {
            return ( operand.length() );
        }
    }

    /*
    -----------------------
    No valid operand found!
    ----------------------- */
    return 0;
}

/******************************************************************************
*                   FTCALC_CLASS::infix_string_to_queue                       *
*  STRING infix Contains the infix expression in STRING form.                 *
*  queue<STRING> infix_q The STRING queue form of the infix expression.       *
******************************************************************************/
int FTCALC_CLASS::infix_string_to_queue( STRING infix_str, queue<STRING> & infix_q )
{
    unsigned int n = infix_str.length();
    unsigned int ni;
    unsigned int i;
    STRING num_str;
    STRING char_str;
    STRING invalid_operand;

    i = 0;
    while ( i < n )
    {
        /*
        ------------------
        Ignore white space
        ------------------ */
        if ( infix_str[i] == SpaceChar )
        {
            i++;
            continue;
        }

        /*
        -------------------
        Ignore \r\n for now
        ------------------- */
        if ( infix_str[i] == CrChar || infix_str[i] == LfChar )
        {
            i++;
            continue;
        }

        /*
        ----------------
        Handle operators
        ---------------- */
        if ( is_operator( &infix_str[i] ) || infix_str[i] == LeftParen || infix_str[i] == RightParen )
        {
            char_str.clear( );
            char_str = infix_str[i];
            infix_q.push( char_str );
            i++;
            continue;
        }

        /*
        ------------------
        Must be an operand
        ------------------ */
        num_str.clear( );
        ni = parse_operand( infix_str, i, num_str );

        if (ni == 0)
        {
            invalid_operand = TEXT( "Invalid operand: " ) + infix_str;
            //MessageBox( NULL, invalid_operand.c_str(), TEXT("Error"), MB_OK );
            return 1;
        }

        infix_q.push( num_str );
        i += ni;
    }

    return 0;
}

/************************************************************************************
*  FTCALC_CLASS::parse_lines                                                        *
*  TCHAR *text The multiline command text for a parameter.                          *
*  retval STRING The result of the evaluated command.                               *
************************************************************************************/
STRING FTCALC_CLASS::parse_lines( TCHAR * text, FTII_PROFILE & profile, PART_CLASS & part )
{
    STRING textStr = text;
    STRING line;
    size_t found;
    size_t pos = 0;
    queue<STRING> infix_q;
    queue<STRING> postfix_q;
    STRING postfix_str;
    STRING result;
    static const REGEX neg_op1_ex( TEXT("(.*)\\s*=\\s*-(\\d*.?\\d*)") );
    static const REGEX neg_op2_ex( TEXT("^-(\\d*.?\\d*)") );

    /*
    --------------------------------
    Replace negative number operator
    -------------------------------- */
    textStr = regex_replace(textStr, neg_op1_ex, TEXT( "$1=0-$2" ) );
    textStr = regex_replace(textStr, neg_op2_ex, TEXT( "0-$1" ) );

    found = textStr.find( CrLfString, pos );
    while ( found != STRING::npos )
    {
        line.clear();
        line = textStr.substr( pos, found - pos );
        found += 2;
        pos = found;
        if ( !line.empty() )
        {
            parse_functions( line, profile, part );
            if ( !parse_assignment( line ) )
            {
                if ( infix_string_to_queue(line.c_str(), infix_q) > 0 )
                    return EmptyString;
                infix_to_postfix( &infix_q, &postfix_q );
                result = evaluate_postfix( &postfix_q );
            }
        }
        found = textStr.find( CrLfString, pos );
    }

    found = textStr.find( NullChar, pos );
    line.clear();
    line = textStr.substr( pos, found );
    if ( !line.empty() )
    {
        parse_functions( line, profile, part );
        if ( !parse_assignment( line ) )
        {
            if ( infix_string_to_queue(line.c_str(), infix_q ) > 0 )
                return EmptyString;

            infix_to_postfix( &infix_q, &postfix_q );
            result = evaluate_postfix( &postfix_q );
        }
    }

    return result;
}

/******************************************************************************
*                      FTCALC_CLASS::parse_assignment                         *
*  Description: Parses assignement operations and creates member variables.   *
*  STRING line The assignment operation to parse.                             *
*  retval BOOLEAN Returns true if line contained an assignment operation,     *
*       otherwise false.                                                      *
******************************************************************************/
BOOLEAN FTCALC_CLASS::parse_assignment( STRING line )
{
    STRING lhs;
    STRING rhs;
    STRING result;
    queue <STRING> infix_q;
    queue <STRING> postfix_q;
    static const REGEX assignment_ex( TEXT("([[:alpha:]]+[_[:alnum:]]*)\\s*=\\s*(.*)") );
    REGMATCH match;
    BOOLEAN found = false;
    std::map<STRING, STRING>::iterator vars_it;

    if ( !regex_match(line, match, assignment_ex) )
    {
        return false;
    }

    lhs = match[1];
    rhs = match[2];

    if ( infix_string_to_queue(rhs.c_str(), infix_q) > 0 )
        return false;

    infix_to_postfix( &infix_q, &postfix_q );
    result = evaluate_postfix( &postfix_q );
    vars[lhs] = result;

    return true;
}

/***********************************************************************
*                    FTCALC_CLASS::parse_functions                     *
*  Description: Parses function calls in line parameter and replaces   *
*       them with their evaluated values in line parameter.            *
*  STRING line The assignment operation to parse.                      *
*  retval BOOLEAN Returns true if line contained an assignment         *
*       operation, otherwise false.                                    *
***********************************************************************/
void FTCALC_CLASS::parse_functions( STRING & line, FTII_PROFILE & profile, PART_CLASS & part )
{
    static const REGEX max_val_ex( TEXT(".*MAX_VALUE\\s*\\((.*),(.*),(.*)\\).*") );
    static const REGEX max_val_replace( TEXT("(.*)MAX_VALUE\\s*\\(.*\\)(.*)") );
    static const REGEX min_val_ex(TEXT(".*MIN_VALUE\\s*\\((.*),(.*),(.*)\\).*"));
    static const REGEX min_val_replace( TEXT("(.*)MIN_VALUE\\s*\\(.*\\)(.*)") );
    static const REGEX target_pos_ex( TEXT(".*TARGET_POS\\s*\\((.*),(.*),(.*),(.*)\\).*") );
    static const REGEX target_pos_replace( TEXT("(.*)TARGET_POS\\s*\\(.*\\)(.*)") );
    REGMATCH sm;
    STRING_CLASS args;
    STRING_CLASS value;
    STRING_CLASS newline;
    STRING_CLASS var;
    STRING_CLASS eval;
    STRING_CLASS s;

    /*
    ------------------
    function MAX_VALUE
    ------------------ */
    if ( regex_match(line, sm, max_val_ex) )
    {
        args = sm[1].str().c_str();
        args += CommaChar;
        s = sm[2].str().c_str();
        evaluate_arg( eval, s );
        args += eval;
        args += CommaChar;
        s = sm[3].str().c_str();
        evaluate_arg( eval, s );
        args += eval;
        value = max_value( args, profile, part );
        newline = TEXT( "$1 " );
        newline += value;
        newline += TEXT( " $2" );
        line = regex_replace( line, max_val_replace, newline.text() );
    }

    /*
    ------------------
    function MIN_VALUE
    ------------------ */
    if ( regex_match(line, sm, min_val_ex) )
    {
        args.null();
        s.null();
        eval.null();
        args = sm[1].str().c_str();
        args += CommaChar;
        s = sm[2].str().c_str();
        evaluate_arg(eval, s);
        args += eval;
        args += CommaChar;
        s = sm[3].str().c_str();
        evaluate_arg(eval, s);
        args += eval;
        value = min_value( args, profile, part );
        newline = TEXT( "$1 " );
        newline += value;
        newline += TEXT( " $2" );
        line = regex_replace( line, min_val_replace, newline.text() );
    }

    /*
    -------------------
    function TARGET_POS
    ------------------- */
    if ( regex_match(line, sm, target_pos_ex) )
    {
        args.null();
        s.null();
        eval.null();
        args = sm[1].str().c_str();
        args += CommaChar;
        s = sm[2].str().c_str();
        evaluate_arg( eval, s );
        args += eval;
        args += CommaChar;
        s = sm[3].str().c_str();
        args += s;
        args += CommaChar;
        s = sm[4].str().c_str();
        evaluate_arg( eval, s );
        args += eval;
        value = target_pos( args, profile, part );
        newline = TEXT( "$1 " );
        newline += value;
        newline += TEXT( " $2" );
        line = regex_replace( line, target_pos_replace, newline.text() );
    }
}

/***********************************************************************
*                    FTCALC_CLASS::evaluate_arg                        *
*  Description: Evaluates arguments to calculator function calls       *
*      reducing infix notation expressions, including variables to a   *
*      single numeric value.                                           *
*  STRING_CLASS & value The evaluated value will be copied here.       *
*  STRING_CLASS & arg The argument to be evaluated.                    *
***********************************************************************/
void FTCALC_CLASS::evaluate_arg( STRING_CLASS & value, STRING_CLASS & arg )
{
    STRING arg_str = arg.text();
    queue<STRING> arg_q;
    queue<STRING> postfix_q;
    STRING value_str;

    infix_string_to_queue( arg_str, arg_q );
    infix_to_postfix( & arg_q,  & postfix_q );
    value_str = evaluate_postfix( & postfix_q );
    value = value_str.c_str();
}