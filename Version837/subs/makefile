CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /O1 /D "NDEBUG" /D "WIN32" /D "_LIB" /D "UNICODE" /D "_UNICODE" /Yuwindows.h /Fp..\include\mywin.pch /c

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\lib\v5subs.lib"

..\lib\v5subs.lib : alarmsum.obj array.obj ASCFLOAT.obj ASENSOR.obj bitclass.obj CALLFILE.obj \
    CATCLASS.obj CLIENT.obj COLOR.obj COMPUTER.obj CONVERTA.obj COPYMACH.obj COPYPART.obj \
    createco.obj CURRENT.obj dateclas.obj DBSUBS.obj DCURVE.obj dow.obj downsubs.obj \
    downtime.obj dstat.obj ERROR_MS.obj evclass.obj execwait.obj exists.obj \
    EXTRACT.obj fileclas.obj FILLPART.obj fillshot.obj fix.obj FTANALOG.obj ftime.obj \
    FTOASC.obj GENLIST.obj GETSHOT.obj GET_INI.obj get_tc.obj GEXEDIR.obj GPVAR.obj  \
    hex.obj hourglas.obj INSALPH.obj IS_EMPTY.obj ITOASC.obj KILLDIR.obj listbox.obj \
    MACHCOMP.obj MACHINE.obj MACHNAME.obj MARKS.obj MAXIMIZE.obj MEM.obj menu.obj \
    MEXISTS.obj misc.obj monrun.obj namecopy.obj NAMES.obj PARAM.obj PART.obj \
    path.obj PEXISTS.obj plookup.obj plotclas.obj plotsubs.obj POSTSEC.obj profiles.obj \
    READLINE.obj REVERSE.obj ringcl.obj RJUST.obj SEMAPHOR.obj SENSOR.obj SHIFT.obj \
    shotclas.obj startpar.obj stparam.obj stringcl.obj STRUCTS.obj stsetup.obj TEXTLIST.obj \
    TIME.obj timeclas.obj UNITADJ.obj UNITS.obj updown.obj wclass.obj WINSUBS.obj \
    writelin.obj ZERO.obj
    $(LIB32) $(LIB32_FLAGS) alarmsum.obj array.obj ascfloat.obj ASENSOR.obj \
    bitclass.obj CALLFILE.obj CATCLASS.obj \
    CLIENT.obj COLOR.obj COMPUTER.obj CONVERTA.obj COPYMACH.obj COPYPART.obj  \
    createco.obj CURRENT.obj dateclas.obj DBSUBS.obj DCURVE.obj dow.obj       \
    downsubs.obj downtime.obj dstat.obj ERROR_MS.obj evclass.obj execwait.obj \
    exists.obj EXTRACT.obj fileclas.obj FILLPART.obj fillshot.obj fix.obj     \
    FTANALOG.obj ftime.obj FTOASC.obj GENLIST.obj GETSHOT.obj GET_INI.obj     \
    get_tc.obj GEXEDIR.obj GPVAR.obj hex.obj hourglas.obj INSALPH.obj         \
    IS_EMPTY.obj ITOASC.obj KILLDIR.obj listbox.obj MACHCOMP.obj MACHINE.obj  \
    MACHNAME.obj MARKS.obj MAXIMIZE.obj MEM.obj menu.obj MEXISTS.obj          \
    misc.obj monrun.obj namecopy.obj NAMES.obj PARAM.obj PART.obj             \
    path.obj PEXISTS.obj plookup.obj plotclas.obj plotsubs.obj POSTSEC.obj    \
    profiles.obj READLINE.obj REVERSE.obj ringcl.obj RJUST.obj SEMAPHOR.obj   \
    SENSOR.obj SHIFT.obj shotclas.obj startpar.obj stparam.obj stringcl.obj   \
    STRUCTS.obj stsetup.obj TEXTLIST.obj TIME.obj timeclas.obj UNITADJ.obj    \
    UNITS.obj updown.obj wclass.obj WINSUBS.obj writelin.obj ZERO.obj

alarmsum.obj : alarmsum.cpp
    $(CPP) $(CPP_PROJ) alarmsum.cpp

array.obj : array.cpp
    $(CPP) $(CPP_PROJ) array.cpp

ASCFLOAT.obj : ASCFLOAT.CPP
    $(CPP) $(CPP_PROJ) ASCFLOAT.CPP

ASENSOR.obj : ASENSOR.CPP
    $(CPP) $(CPP_PROJ) ASENSOR.CPP

bitclass.obj : bitclass.cpp
    $(CPP) $(CPP_PROJ) bitclass.cpp

CALLFILE.obj : CALLFILE.CPP
    $(CPP) $(CPP_PROJ) CALLFILE.CPP

CATCLASS.obj : CATCLASS.CPP
    $(CPP) $(CPP_PROJ) CATCLASS.CPP

CLIENT.obj : CLIENT.CPP
    $(CPP) $(CPP_PROJ) CLIENT.CPP

COLOR.obj : COLOR.CPP
    $(CPP) $(CPP_PROJ) COLOR.CPP

COMPUTER.obj : COMPUTER.CPP
    $(CPP) $(CPP_PROJ) COMPUTER.CPP

CONVERTA.obj : CONVERTA.CPP
    $(CPP) $(CPP_PROJ) CONVERTA.CPP

COPYMACH.obj : COPYMACH.CPP
    $(CPP) $(CPP_PROJ) COPYMACH.CPP

COPYPART.obj : COPYPART.CPP
    $(CPP) $(CPP_PROJ) COPYPART.CPP

createco.obj : createco.cpp
    $(CPP) $(CPP_PROJ) createco.cpp

CURRENT.obj : CURRENT.CPP
    $(CPP) $(CPP_PROJ) CURRENT.CPP

dateclas.obj : dateclas.cpp
    $(CPP) $(CPP_PROJ) dateclas.cpp

DBSUBS.obj : DBSUBS.CPP
    $(CPP) $(CPP_PROJ) DBSUBS.CPP

DCURVE.obj : DCURVE.CPP
    $(CPP) $(CPP_PROJ) DCURVE.CPP

dow.obj : dow.cpp
    $(CPP) $(CPP_PROJ) dow.cpp

downsubs.obj : downsubs.cpp
    $(CPP) $(CPP_PROJ) downsubs.cpp

downtime.obj : downtime.cpp
    $(CPP) $(CPP_PROJ) downtime.cpp

dstat.obj : dstat.cpp
    $(CPP) $(CPP_PROJ) dstat.cpp

ERROR_MS.obj : ERROR_MS.CPP
    $(CPP) $(CPP_PROJ) ERROR_MS.CPP

evclass.obj : evclass.cpp
    $(CPP) $(CPP_PROJ) evclass.cpp

execwait.obj : execwait.cpp
    $(CPP) $(CPP_PROJ) execwait.cpp

exists.obj : exists.cpp
    $(CPP) $(CPP_PROJ) exists.cpp

EXTRACT.obj : EXTRACT.CPP
    $(CPP) $(CPP_PROJ) EXTRACT.CPP

fileclas.obj : fileclas.cpp
    $(CPP) $(CPP_PROJ) fileclas.cpp

FILLPART.obj : FILLPART.CPP
    $(CPP) $(CPP_PROJ) FILLPART.CPP

fillshot.obj : fillshot.cpp
    $(CPP) $(CPP_PROJ) fillshot.cpp

fix.obj : fix.cpp
    $(CPP) $(CPP_PROJ) fix.cpp

FTANALOG.obj : FTANALOG.CPP
    $(CPP) $(CPP_PROJ) FTANALOG.CPP

ftime.obj : ftime.cpp
    $(CPP) $(CPP_PROJ) ftime.cpp

FTOASC.obj : FTOASC.CPP
    $(CPP) $(CPP_PROJ) FTOASC.CPP

GENLIST.obj : GENLIST.CPP
    $(CPP) $(CPP_PROJ) GENLIST.CPP

GETSHOT.obj : GETSHOT.CPP
    $(CPP) $(CPP_PROJ) GETSHOT.CPP

GET_INI.obj : GET_INI.CPP
    $(CPP) $(CPP_PROJ) GET_INI.CPP

get_tc.obj : get_tc.cpp
    $(CPP) $(CPP_PROJ) get_tc.cpp

GEXEDIR.obj : GEXEDIR.CPP
    $(CPP) $(CPP_PROJ) GEXEDIR.CPP

GPVAR.obj : GPVAR.CPP
    $(CPP) $(CPP_PROJ) GPVAR.CPP

hex.obj : hex.cpp
    $(CPP) $(CPP_PROJ) hex.cpp

hourglas.obj : hourglas.cpp
    $(CPP) $(CPP_PROJ) hourglas.cpp

INSALPH.obj : INSALPH.CPP
    $(CPP) $(CPP_PROJ) INSALPH.CPP

IS_EMPTY.obj : IS_EMPTY.CPP
    $(CPP) $(CPP_PROJ) IS_EMPTY.CPP

ITOASC.obj : ITOASC.CPP
    $(CPP) $(CPP_PROJ) ITOASC.CPP

KILLDIR.obj : KILLDIR.CPP
    $(CPP) $(CPP_PROJ) KILLDIR.CPP

listbox.obj : listbox.cpp
    $(CPP) $(CPP_PROJ) listbox.cpp

MACHCOMP.obj : MACHCOMP.CPP
    $(CPP) $(CPP_PROJ) MACHCOMP.CPP

MACHINE.obj : MACHINE.CPP
    $(CPP) $(CPP_PROJ) MACHINE.CPP

MACHNAME.obj : MACHNAME.CPP
    $(CPP) $(CPP_PROJ) MACHNAME.CPP

MARKS.obj : MARKS.CPP
    $(CPP) $(CPP_PROJ) MARKS.CPP

MAXIMIZE.obj : MAXIMIZE.CPP
    $(CPP) $(CPP_PROJ) MAXIMIZE.CPP

MEM.obj : MEM.CPP
    $(CPP) $(CPP_PROJ) MEM.CPP

menu.obj : menu.cpp
    $(CPP) $(CPP_PROJ) menu.cpp

MEXISTS.obj : MEXISTS.CPP
    $(CPP) $(CPP_PROJ) MEXISTS.CPP

misc.obj : misc.cpp
    $(CPP) $(CPP_PROJ) misc.cpp

monrun.obj : monrun.cpp
    $(CPP) $(CPP_PROJ) monrun.cpp

namecopy.obj : namecopy.cpp
    $(CPP) $(CPP_PROJ) namecopy.cpp

NAMES.obj : NAMES.CPP
    $(CPP) $(CPP_PROJ) NAMES.CPP

PARAM.obj : PARAM.CPP
    $(CPP) $(CPP_PROJ) PARAM.CPP

PART.obj : PART.CPP
    $(CPP) $(CPP_PROJ) PART.CPP

path.obj : path.cpp
    $(CPP) $(CPP_PROJ) path.cpp

PEXISTS.obj : PEXISTS.CPP
    $(CPP) $(CPP_PROJ) PEXISTS.CPP

plookup.obj : plookup.cpp
    $(CPP) $(CPP_PROJ) plookup.cpp

plotclas.obj : plotclas.cpp
    $(CPP) $(CPP_PROJ) plotclas.cpp

plotsubs.obj : plotsubs.cpp
    $(CPP) $(CPP_PROJ) plotsubs.cpp

POSTSEC.obj : POSTSEC.CPP
    $(CPP) $(CPP_PROJ) POSTSEC.CPP

profiles.obj : profiles.cpp
    $(CPP) $(CPP_PROJ) profiles.cpp

READLINE.obj : READLINE.CPP
    $(CPP) $(CPP_PROJ) READLINE.CPP

REVERSE.obj : REVERSE.CPP
    $(CPP) $(CPP_PROJ) REVERSE.CPP

ringcl.obj : ringcl.cpp
    $(CPP) $(CPP_PROJ) ringcl.cpp

RJUST.obj : RJUST.CPP
    $(CPP) $(CPP_PROJ) RJUST.CPP

SEMAPHOR.obj : SEMAPHOR.CPP
    $(CPP) $(CPP_PROJ) SEMAPHOR.CPP

SENSOR.obj : SENSOR.CPP
    $(CPP) $(CPP_PROJ) SENSOR.CPP

SHIFT.obj : SHIFT.CPP
    $(CPP) $(CPP_PROJ) SHIFT.CPP

shotclas.obj : shotclas.cpp
    $(CPP) $(CPP_PROJ) shotclas.cpp

startpar.obj : startpar.cpp
    $(CPP) $(CPP_PROJ) startpar.cpp

stparam.obj : stparam.cpp
    $(CPP) $(CPP_PROJ) stparam.cpp

stringcl.obj : stringcl.cpp
    $(CPP) $(CPP_PROJ) stringcl.cpp

STRUCTS.obj : STRUCTS.CPP
    $(CPP) $(CPP_PROJ) STRUCTS.CPP

stsetup.obj : stsetup.cpp
    $(CPP) $(CPP_PROJ) stsetup.cpp

TEXTLIST.obj : TEXTLIST.CPP
    $(CPP) $(CPP_PROJ) TEXTLIST.CPP

TIME.obj : TIME.CPP
    $(CPP) $(CPP_PROJ) TIME.CPP

timeclas.obj : timeclas.cpp
    $(CPP) $(CPP_PROJ) timeclas.cpp

UNITADJ.obj : UNITADJ.CPP
    $(CPP) $(CPP_PROJ) UNITADJ.CPP

UNITS.obj : UNITS.CPP
    $(CPP) $(CPP_PROJ) UNITS.CPP

updown.obj : updown.cpp
    $(CPP) $(CPP_PROJ) updown.cpp

wclass.obj : wclass.cpp
    $(CPP) $(CPP_PROJ) wclass.cpp

WINSUBS.obj : WINSUBS.CPP
    $(CPP) $(CPP_PROJ) WINSUBS.CPP

writelin.obj : writelin.cpp
    $(CPP) $(CPP_PROJ) writelin.cpp

ZERO.obj : ZERO.CPP
    $(CPP) $(CPP_PROJ) ZERO.CPP



