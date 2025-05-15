//////////////////////// fixed_point_decimal.h ///////////////////////////
//
// Copyright 2008 Visi-Trak Worldwide

// Fixed point decimal macros.  These macros work on fixed point decimal integers.  Three decimal
// places are assumed.  Numbers are basically the number * 1000.  Therefore 5.003 for example would be represented
// as 5003 and so on.

typedef long fixed3; // fixed point decimal type, 3 decimal places
typedef long fixed4; // Fixed point, 4 decimal places
typedef long long fixed3ll; // Double long, 3 decimal places
typedef long long fixed4ll; // Double long, 4 decimal places

// These macros will work with long and long long types except for the square root function.  That's limited
// to type fpd only (no type long long).

// Conversions to / from fixed3
//
// Long to fixed3
#define LONG_TO_FIXED3(fpd) fpd*1000
// fixed3 to long
#define FIXED3_TO_LONG(fpd) fpd/1000
// Floating point to fixed3 conversion, these will take a lot of processing time, but manageable.						
#define FLOAT_TO_FIXED3(floatval) (fixed3)(floatval*1000F)						
#define FLOAT_TO_FIXED3LL(floatval) (fixed3ll)(floatval*1000F)
// Fixed3 to floating point conversion, OK for both fixed3 and fixed3ll						
#define FIXED3_TO_FLOAT(fixed3val) (float)((float)fixed3val/1000F)	
// Double to fixed3 conversion, these will take a great deal of processing time, use sparingly if at all.						
#define DOUBLE_TO_FIXED3(doubleval) (fixed3)(doubleval*1000F)						
#define DOUBLE_TO_FIXED3LL(doubleval) (fixed3ll)(doubleval*1000F)
// Fixed3 to double conversion, OK for both fixed3 and fixed3ll						
#define FIXED3_TO_DOUBLE(fixed3val) (double)((double)fixed3val/1000F)	

// Three decimal place conversions
#define FIXED3_ADD(fpd1, fpd2) fpd1+fpd2 
#define FIXED3_SUB(fpd1, fpd2) fpd1-fpd2
#define FIXED3_MUL(fpd1, fpd2) ((long long)fpd1*(long long)fpd2)/1000LL
#define FIXED3_DIV(fpd1, fpd2) ((long long)fpd1*1000LL)/(long long)fpd2

// Multiplying the sqrt of a fixed3 number (the num * 1000) by the constant 31.623 will convert the result
// to fixed3 format.  fpd_result is type fixed3.  
// This macro is limited to type fixed3 (won't work with type fixed3ll).
#define FIXED3_SQRT(fpd_item_to_sqrt, fpd_result) \
						if (fpd_item_to_sqrt==0) fpd_result=0; \
						else if (fpd_item_to_sqrt<0) fpd_result=(fpd)root((long)(0-fpd_item_to_sqrt)); \
						else fpd_result=(fpd)root((long)fpd_item_to_sqrt); \
						fpd_result=FIXED3_MUL(fpd_result, 31623)	

						
// Conversions to / from fixed4
//
// Long to fixed4
#define LONG_TO_FIXED4(fpd) fpd*10000
#define LONG_TO_FIXED4LL(fpd) fpd*10000 // For readability
#define CONVERT_TO_FIXED4(fpd) fpd*10000
#define CONVERT_TO_FIXED4LL(fpd) fpd*10000
// fixed4 to long
#define FIXED4_TO_LONG(fpd) fpd/10000
#define FIXED4LL_TO_LONG(fpd) fpd/10000 // For readability
// Floating point to fixed4 conversion, these will take a lot of processing time, but manageable.						
#define FLOAT_TO_FIXED4(floatval) (fixed4)(floatval*10000F)						
#define FLOAT_TO_FIXED4LL(floatval) (fixed4ll)(floatval*10000F)	
// Fixed4 to floating point conversion, OK for both fixed4 and fixed4ll						
#define FIXED4_TO_FLOAT(fixed4val) (float)((float)fixed4val/10000F)	
// Double to fixed4 conversion, these will take a great deal of processing time, use sparingly if at all.						
#define DOUBLE_TO_FIXED4(doubleval) (fixed4)(doubleval*10000F)						
#define DOUBLE_TO_FIXED4LL(doubleval) (fixed4ll)(doubleval*10000F)
// Fixed4 to double conversion, OK for both fixed4 and fixed4ll						
#define FIXED4_TO_DOUBLE(fixed4val) (double)((double)fixed4val/10000F)	


// Four decimal place calculations
#define FIXED4_ADD(fpd1, fpd2) fpd1+fpd2 
#define FIXED4_SUB(fpd1, fpd2) fpd1-fpd2
#define FIXED4_MUL(fpd1, fpd2) ((long long)fpd1*(long long)fpd2)/10000LL
#define FIXED4_DIV(fpd1, fpd2) ((long long)fpd1*10000LL)/(long long)fpd2

// Multiplying the sqrt of a fixed4 number (the num * 10000) by the constant 100.0000 will convert the result
// to 4 decimal place fpd format.  fpd_result is type fixed4.  
// This macro is limited to type fixed4 (won't work with fixed4ll).
#define FIXED4_SQRT(fpd_item_to_sqrt, fpd_result) \
						if (fpd_item_to_sqrt==0) fpd_result=0; \
						else if (fpd_item_to_sqrt<0) fpd_result=(fpd)root((long)(0-fpd_item_to_sqrt)); \
						else fpd_result=(fpd)root((long)fpd_item_to_sqrt); \
						fpd_result=FIXED4_MUL(fpd_result, 1000000)		

