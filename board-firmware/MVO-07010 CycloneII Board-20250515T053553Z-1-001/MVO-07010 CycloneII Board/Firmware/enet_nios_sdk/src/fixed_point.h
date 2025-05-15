//////////////////////// fixed_point.h ///////////////////////////
//
// Copyright 2008 Visi-Trak Worldwide

// Fixed point macros.  These macros work on fixed point integers.  Ten (1 in 1024) and 14 bit (1 in 16384) 
// binary places are supported.

typedef long fp10; // fixed point decimal type, 3 decimal places
typedef long fp14; // Fixed point, 4 decimal places
typedef long long fp10ll; // Double long, 10 binary places
typedef long long fp14ll; // Double long, 14 binary places

// These macros will work with long and long long types. 

// Conversions to / from fp10
//
// Long to fp10
#define LONG_TO_FP10(fp) fp<<10
// fp10 to long
#define FP10_TO_LONG(fp) fp>>10

// Ten binary place conversions
#define FP10_ADD(fp1, fp2) fpd1+fpd2 
#define FP10_SUB(fp1, fp2) fpd1-fpd2
#define FP10_MUL(fp1, fp2) ((long long)fp1*(long long)fp2)<<10
#define FP10_DIV(fp1, fp2) ((long long)fp1<<10)/(long long)fp2

						
// Conversions to / from fixed4
//
// Long to fp14
#define LONG_TO_FP14(fp) fp<<14
// fp14 to long
#define FP14_TO_LONG(fp) fp>>14

// Fourteen binary place conversions
#define FP14_ADD(fp1, fp2) fpd1+fpd2 
#define FP14_SUB(fp1, fp2) fpd1-fpd2
#define FP14_MUL(fp1, fp2) ((long long)fp1*(long long)fp2)<<14
#define FP14_DIV(fp1, fp2) ((long long)fp1<<14)/(long long)fp2

