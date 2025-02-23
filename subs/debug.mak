CPP_PROJ=/nologo /MTd /W3 /Od /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_LIB" /Fp"../include/debug.pch"  /Yu"windows.h" /Fo"debug/" /c

LIB32_FLAGS=/nologo /out:"..\debuglib\v5subs.lib"

..\debuglib\v5subs.lib : debug\alarmsum.obj debug\array.obj debug\ascfloat.obj \
 debug\asensor.obj debug\bitclass.obj debug\callfile.obj debug\catclass.obj debug\chaxis.obj debug\choose_wire.obj \
 debug\client.obj debug\color.obj debug\colorcl.obj debug\computer.obj debug\copymach.obj \
 debug\copypart.obj debug\current.obj debug\dateclas.obj debug\db_to_vdb.obj debug\dbsubs.obj debug\dbupsize.obj \
 debug\dcurve.obj debug\dow.obj debug\downsubs.obj debug\downtime.obj debug\dstat.obj debug\dtotchar.obj \
 debug\dynamic_file_name.obj \
 debug\error_ms.obj debug\evclass.obj debug\execwait.obj debug\exists.obj debug\external_parameter.obj debug\extract.obj \
 debug\fifo.obj \
 debug\fileclas.obj debug\fillpart.obj debug\fillshot.obj debug\fix.obj debug\fontclas.obj debug\ftcalc.obj debug\ftanalog.obj \
 debug\ftii_file.obj debug\ftii_parameters.obj debug\ftii_shot_data.obj debug\ftiiclas.obj \
 debug\ftime.obj debug\ftoasc.obj debug\genlist.obj debug\get_ini.obj \
 debug\get_tc.obj debug\autoback.obj debug\gexedir.obj debug\gpvar.obj debug\hex.obj debug\hourglas.obj debug\iniclass.obj \
 debug\insalph.obj debug\is_empty.obj debug\itoasc.obj debug\killdir.obj debug\listbox.obj debug\limit_switch_class.obj debug\log_file_string.obj \
 debug\machcomp.obj debug\machine.obj debug\machine_to_hmi.obj debug\machname.obj debug\marks.obj debug\maximize.obj \
 debug\mem.obj debug\menu.obj debug\menuctrl.obj debug\mexists.obj debug\misc.obj debug\monrun.obj debug\msgclas.obj debug\multipart_runlist.obj debug\multistring.obj \
 debug\nameclas.obj debug\optocontrol.obj debug\param.obj debug\param_index_class.obj debug\part.obj debug\partlist.obj \
 debug\path.obj \
 debug\pexists.obj debug\plookup.obj debug\plotclas.obj debug\plotsubs.obj debug\pointcl.obj debug\popctrl.obj debug\postsec.obj \
 debug\profiles.obj debug\purge.obj debug\pw.obj \
 debug\readline.obj debug\rectclas.obj debug\reverse.obj debug\ringcl.obj debug\rjust.obj \
 debug\runlist.obj \
 debug\semaphor.obj debug\setpoint.obj debug\shift.obj debug\shot_name.obj debug\shot_name_reset.obj debug\shotclas.obj debug\sockcli.obj \
 debug\statics.obj debug\static_text_parameter.obj debug\startpar.obj debug\stddown.obj \
 debug\stpres.obj debug\strarray.obj \
 debug\stparam.obj debug\stringcl.obj debug\stringtable.obj debug\structs.obj debug\stsetup.obj debug\textlen.obj \
 debug\textlist.obj debug\temp_message.obj debug\time.obj \
 debug\timeclas.obj debug\unitadj.obj debug\units.obj debug\updown.obj debug\v5tov7.obj debug\vdbclass.obj debug\visigrid.obj debug\visiplot.obj \
 debug\warmupcl.obj debug\wclass.obj debug\winsubs.obj debug\wire_class.obj debug\workclas.obj debug\zero.obj
 link -lib $(LIB32_FLAGS) debug\alarmsum.obj debug\array.obj debug\ascfloat.obj \
 debug\asensor.obj debug\bitclass.obj debug\callfile.obj debug\catclass.obj debug\chaxis.obj debug\choose_wire.obj \
 debug\client.obj debug\color.obj debug\colorcl.obj debug\computer.obj debug\copymach.obj \
 debug\copypart.obj debug\current.obj debug\dateclas.obj debug\db_to_vdb.obj debug\dbsubs.obj debug\dbupsize.obj \
 debug\dcurve.obj debug\dow.obj debug\downsubs.obj debug\downtime.obj debug\dstat.obj debug\dtotchar.obj \
 debug\dynamic_file_name.obj \
 debug\error_ms.obj debug\evclass.obj debug\execwait.obj debug\exists.obj debug\external_parameter.obj debug\extract.obj \
 debug\fifo.obj \
 debug\fileclas.obj debug\fillpart.obj debug\fillshot.obj debug\fix.obj debug\fontclas.obj debug\ftcalc.obj debug\ftanalog.obj \
 debug\ftii_file.obj debug\ftii_parameters.obj debug\ftii_shot_data.obj debug\ftiiclas.obj \
 debug\ftime.obj debug\ftoasc.obj debug\genlist.obj debug\get_ini.obj \
 debug\get_tc.obj debug\autoback.obj debug\gexedir.obj debug\gpvar.obj debug\hex.obj debug\hourglas.obj debug\iniclass.obj \
 debug\insalph.obj debug\is_empty.obj debug\itoasc.obj debug\killdir.obj debug\listbox.obj debug\limit_switch_class.obj debug\log_file_string.obj \
 debug\machcomp.obj debug\machine.obj debug\machine_to_hmi.obj debug\machname.obj debug\marks.obj debug\maximize.obj \
 debug\mem.obj debug\menu.obj debug\menuctrl.obj debug\mexists.obj debug\misc.obj debug\monrun.obj debug\msgclas.obj debug\multipart_runlist.obj debug\multistring.obj \
 debug\nameclas.obj debug\optocontrol.obj debug\param.obj debug\param_index_class.obj debug\part.obj debug\partlist.obj \
 debug\path.obj \
 debug\pexists.obj debug\plookup.obj debug\plotclas.obj debug\plotsubs.obj debug\pointcl.obj debug\popctrl.obj debug\postsec.obj \
 debug\profiles.obj debug\purge.obj debug\pw.obj \
 debug\readline.obj debug\rectclas.obj debug\reverse.obj debug\ringcl.obj debug\rjust.obj \
 debug\runlist.obj \
 debug\semaphor.obj debug\setpoint.obj debug\shift.obj debug\shot_name.obj debug\shot_name_reset.obj debug\shotclas.obj debug\sockcli.obj \
 debug\statics.obj debug\static_text_parameter.obj debug\startpar.obj debug\stddown.obj \
 debug\stpres.obj debug\strarray.obj \
 debug\stparam.obj debug\stringcl.obj debug\stringtable.obj debug\structs.obj debug\stsetup.obj debug\textlen.obj \
 debug\textlist.obj debug\temp_message.obj debug\time.obj \
 debug\timeclas.obj debug\unitadj.obj debug\units.obj debug\updown.obj debug\v5tov7.obj debug\vdbclass.obj debug\visigrid.obj debug\visiplot.obj \
 debug\warmupcl.obj debug\wclass.obj debug\winsubs.obj debug\wire_class.obj debug\workclas.obj debug\zero.obj

debug\alarmsum.obj : alarmsum.cpp
    cl $(CPP_PROJ) alarmsum.cpp

debug\array.obj : array.cpp ..\include\stringcl.h ..\include\fileclas.h
    cl $(CPP_PROJ) array.cpp

debug\ASCFLOAT.obj : ASCFLOAT.CPP
    cl $(CPP_PROJ) ASCFLOAT.CPP

debug\ASENSOR.obj : ASENSOR.CPP
    cl $(CPP_PROJ) ASENSOR.CPP

debug\autoback.obj : autoback.cpp ..\include\autoback.h
    cl $(CPP_PROJ) autoback.cpp

debug\bitclass.obj : bitclass.cpp ..\include\bitclass.h
    cl $(CPP_PROJ) bitclass.cpp

debug\CALLFILE.obj : CALLFILE.CPP
    cl $(CPP_PROJ) CALLFILE.CPP

debug\CATCLASS.obj : CATCLASS.CPP
    cl $(CPP_PROJ) CATCLASS.CPP

debug\chaxis.obj : chaxis.cpp  ..\include\chaxis.h ..\include\fileclas.h
    cl $(CPP_PROJ) chaxis.cpp

debug\choose_wire.obj : choose_wire.cpp
    cl $(CPP_PROJ) choose_wire.cpp

debug\CLIENT.obj : CLIENT.CPP
    cl $(CPP_PROJ) CLIENT.CPP

debug\COLOR.obj : COLOR.CPP
    cl $(CPP_PROJ) COLOR.CPP

debug\colorcl.obj : colorcl.cpp ..\include\colorcl.h
    cl $(CPP_PROJ) colorcl.cpp

debug\COMPUTER.obj : COMPUTER.CPP
    cl $(CPP_PROJ) COMPUTER.CPP

debug\COPYMACH.obj : COPYMACH.CPP
    cl $(CPP_PROJ) COPYMACH.CPP

debug\copypart.obj : copypart.cpp ..\include\part.h
    cl $(CPP_PROJ) copypart.cpp

debug\CURRENT.obj : CURRENT.CPP
    cl $(CPP_PROJ) CURRENT.CPP

debug\dateclas.obj : dateclas.cpp
    cl $(CPP_PROJ) dateclas.cpp

debug\db_to_vdb.obj : db_to_vdb.CPP
    cl $(CPP_PROJ) db_to_vdb.CPP

debug\DBSUBS.obj : DBSUBS.CPP
    cl $(CPP_PROJ) DBSUBS.CPP

debug\dbupsize.obj : dbupsize.CPP
    cl $(CPP_PROJ) dbupsize.CPP

debug\DCURVE.obj : DCURVE.CPP
    cl $(CPP_PROJ) DCURVE.CPP

debug\dow.obj : dow.cpp
    cl $(CPP_PROJ) dow.cpp

debug\downsubs.obj : downsubs.cpp
    cl $(CPP_PROJ) downsubs.cpp

debug\downtime.obj : downtime.cpp
    cl $(CPP_PROJ) downtime.cpp

debug\dstat.obj : dstat.cpp
    cl $(CPP_PROJ) dstat.cpp

debug\dtotchar.obj : dtotchar.cpp
    cl $(CPP_PROJ) dtotchar.cpp

debug\dynamic_file_name.obj : dynamic_file_name.cpp
    cl $(CPP_PROJ) dynamic_file_name.cpp

debug\ERROR_MS.obj : ERROR_MS.CPP
    cl $(CPP_PROJ) ERROR_MS.CPP

debug\evclass.obj : evclass.cpp ..\include\evclass.h
    cl $(CPP_PROJ) evclass.cpp

debug\execwait.obj : execwait.cpp
    cl $(CPP_PROJ) execwait.cpp

debug\exists.obj : exists.cpp
    cl $(CPP_PROJ) exists.cpp

debug\external_parameter.obj : external_parameter.cpp ..\include\external_parameter.h
    cl $(CPP_PROJ) external_parameter.cpp

debug\extract.obj : extract.cpp
    cl $(CPP_PROJ) extract.cpp

debug\fifo.obj : fifo.cpp ..\include\fifo.h
    cl $(CPP_PROJ) fifo.cpp

debug\fileclas.obj : fileclas.cpp ..\include\fileclas.h
    cl $(CPP_PROJ) fileclas.cpp

debug\fillpart.obj : fillpart.CPP
    cl $(CPP_PROJ) fillpart.CPP

debug\fillshot.obj : fillshot.cpp
    cl $(CPP_PROJ) fillshot.cpp

debug\fix.obj : fix.cpp
    cl $(CPP_PROJ) fix.cpp

debug\fontclas.obj : fontclas.cpp ..\include\fontclas.h
    cl $(CPP_PROJ) fontclas.cpp

debug\FTANALOG.obj : FTANALOG.CPP
    cl $(CPP_PROJ) FTANALOG.CPP

debug\ftcalc.obj : ftcalc.cpp ..\include\ftcalc.h
    cl $(CPP_PROJ) ftcalc.cpp

debug\ftii_file.obj : ftii_file.cpp ..\include\ftii_file.h
    cl $(CPP_PROJ) ftii_file.cpp

debug\ftii_parameters.obj : ftii_parameters.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h
    cl $(CPP_PROJ) ftii_parameters.cpp

debug\ftii_shot_data.obj : ftii_shot_data.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h
    cl $(CPP_PROJ) ftii_shot_data.cpp

debug\ftiiclas.obj : ftiiclas.cpp ..\include\ftii.h ..\include\ftii_file.h
    cl $(CPP_PROJ) ftiiclas.cpp

debug\ftime.obj : ftime.cpp
    cl $(CPP_PROJ) ftime.cpp

debug\FTOASC.obj : FTOASC.CPP
    cl $(CPP_PROJ) FTOASC.CPP

debug\GENLIST.obj : GENLIST.CPP
    cl $(CPP_PROJ) GENLIST.CPP

debug\GET_INI.obj : GET_INI.CPP
    cl $(CPP_PROJ) GET_INI.CPP

debug\get_tc.obj : get_tc.cpp
    cl $(CPP_PROJ) get_tc.cpp

debug\GEXEDIR.obj : GEXEDIR.CPP
    cl $(CPP_PROJ) GEXEDIR.CPP

debug\GPVAR.obj : GPVAR.CPP ..\include\fileclas.h
    cl $(CPP_PROJ) GPVAR.CPP

debug\hex.obj : hex.cpp
    cl $(CPP_PROJ) hex.cpp

debug\hourglas.obj : hourglas.cpp
    cl $(CPP_PROJ) hourglas.cpp

debug\iniclass.obj : iniclass.cpp ..\include\iniclass.h
    cl $(CPP_PROJ) iniclass.cpp

debug\INSALPH.obj : INSALPH.CPP
    cl $(CPP_PROJ) INSALPH.CPP

debug\IS_EMPTY.obj : IS_EMPTY.CPP
    cl $(CPP_PROJ) IS_EMPTY.CPP

debug\ITOASC.obj : ITOASC.CPP
    cl $(CPP_PROJ) ITOASC.CPP

debug\KILLDIR.obj : KILLDIR.CPP
    cl $(CPP_PROJ) KILLDIR.CPP

debug\listbox.obj : listbox.cpp
    cl $(CPP_PROJ) listbox.cpp

debug\limit_switch_class.obj : limit_switch_class.cpp ..\include\limit_switch_class.h
    cl $(CPP_PROJ) limit_switch_class.cpp

debug\log_file_string.obj : log_file_string.cpp
    cl $(CPP_PROJ) log_file_string.cpp

debug\MACHCOMP.obj : MACHCOMP.CPP
    cl $(CPP_PROJ) MACHCOMP.CPP

debug\MACHINE.obj : MACHINE.CPP
    cl $(CPP_PROJ) MACHINE.CPP

debug\machine_to_hmi.obj : machine_to_hmi.cpp
    cl $(CPP_PROJ) machine_to_hmi.cpp

debug\MACHNAME.obj : MACHNAME.CPP ..\include\machname.h
    cl $(CPP_PROJ) MACHNAME.CPP

debug\MARKS.obj : MARKS.CPP
    cl $(CPP_PROJ) MARKS.CPP

debug\MAXIMIZE.obj : MAXIMIZE.CPP
    cl $(CPP_PROJ) MAXIMIZE.CPP

debug\MEM.obj : MEM.CPP
    cl $(CPP_PROJ) MEM.CPP

debug\menu.obj : menu.cpp
    cl $(CPP_PROJ) menu.cpp

debug\menuctrl.obj : menuctrl.cpp ..\include\menuctrl.h
    cl $(CPP_PROJ) menuctrl.cpp

debug\MEXISTS.obj : MEXISTS.CPP
    cl $(CPP_PROJ) MEXISTS.CPP

debug\misc.obj : misc.cpp
    cl $(CPP_PROJ) misc.cpp

debug\monrun.obj : monrun.cpp
    cl $(CPP_PROJ) monrun.cpp

debug\msgclas.obj : msgclas.cpp
    cl $(CPP_PROJ) msgclas.cpp

debug\multipart_runlist.obj : multipart_runlist.cpp ..\include\multipart_runlist.h
    cl $(CPP_PROJ) multipart_runlist.cpp

debug\multistring.obj : multistring.cpp ..\include\multistring.h
    cl $(CPP_PROJ) multistring.cpp

debug\nameclas.obj : nameclas.cpp
    cl $(CPP_PROJ) nameclas.cpp

debug\optocontrol.obj : optocontrol.cpp
    cl $(CPP_PROJ) optocontrol.cpp

debug\param.obj : param.cpp
    cl $(CPP_PROJ) param.cpp

debug\param_index_class.obj  : param_index_class.cpp ..\include\param_index_class.h
    cl $(CPP_PROJ) param_index_class.cpp

debug\part.obj : part.cpp ..\include\part.h
    cl $(CPP_PROJ) part.cpp

debug\partlist.obj : partlist.cpp
    cl $(CPP_PROJ) partlist.cpp

debug\path.obj : path.cpp
    cl $(CPP_PROJ) path.cpp

debug\PEXISTS.obj : PEXISTS.CPP
    cl $(CPP_PROJ) PEXISTS.CPP

debug\plookup.obj : plookup.cpp
    cl $(CPP_PROJ) plookup.cpp

debug\plotclas.obj : plotclas.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) plotclas.cpp

debug\plotsubs.obj : plotsubs.cpp
    cl $(CPP_PROJ) plotsubs.cpp

debug\pointcl.obj : pointcl.cpp ..\include\pointcl.h
    cl $(CPP_PROJ) pointcl.cpp

debug\popctrl.obj : popctrl.cpp ..\include\popctrl.h
    cl $(CPP_PROJ) popctrl.cpp

debug\POSTSEC.obj : POSTSEC.CPP
    cl $(CPP_PROJ) POSTSEC.CPP

debug\profiles.obj : profiles.cpp
    cl $(CPP_PROJ) profiles.cpp

debug\purge.obj : purge.cpp
    cl $(CPP_PROJ) purge.cpp

debug\pw.obj : pw.cpp
    cl $(CPP_PROJ) pw.cpp

debug\READLINE.obj : READLINE.CPP
    cl $(CPP_PROJ) READLINE.CPP

debug\rectclas.obj : rectclas.CPP  ..\include\rectclas.h
    cl $(CPP_PROJ) rectclas.CPP

debug\REVERSE.obj : REVERSE.CPP
    cl $(CPP_PROJ) REVERSE.CPP

debug\ringcl.obj : ringcl.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) ringcl.cpp

debug\RJUST.obj : RJUST.CPP
    cl $(CPP_PROJ) RJUST.CPP

debug\runlist.obj : runlist.cpp  ..\include\runlist.h
    cl $(CPP_PROJ) runlist.cpp

debug\semaphor.obj : semaphor.cpp
    cl $(CPP_PROJ) semaphor.cpp

debug\setpoint.obj : setpoint.cpp ..\include\setpoint.h ..\include\strarray.h
    cl $(CPP_PROJ) setpoint.cpp

debug\shift.obj : shift.cpp
    cl $(CPP_PROJ) shift.cpp

debug\shot_name.obj : shot_name.cpp ..\include\shotname.h ..\include\setpoint.h
    cl $(CPP_PROJ) shot_name.cpp

debug\shot_name_reset.obj : shot_name_reset.cpp ..\include\shot_name_reset.h ..\include\setpoint.h
    cl $(CPP_PROJ) shot_name_reset.cpp

debug\shotclas.obj : shotclas.cpp
    cl $(CPP_PROJ) shotclas.cpp

debug\sockcli.obj : sockcli.cpp ..\include\sockcli.h
    cl $(CPP_PROJ) sockcli.cpp

debug\statics.obj : statics.cpp ..\include\statics.h
    cl $(CPP_PROJ) statics.cpp

debug\static_text_parameter.obj : static_text_parameter.cpp ..\include\static_text_parameter.h
    cl $(CPP_PROJ) static_text_parameter.cpp

debug\startpar.obj : startpar.cpp
    cl $(CPP_PROJ) startpar.cpp

debug\stddown.obj : stddown.cpp ..\include\stddown.h
    cl $(CPP_PROJ) stddown.cpp

debug\stparam.obj : stparam.cpp ..\include\fileclas.h
    cl $(CPP_PROJ) stparam.cpp

debug\stpres.obj : stpres.cpp ..\include\stringcl.h ..\include\fileclas.h
    cl $(CPP_PROJ) stpres.cpp

debug\strarray.obj : strarray.cpp ..\include\strarray.h
    cl $(CPP_PROJ) strarray.cpp

debug\stringcl.obj : stringcl.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) stringcl.cpp

debug\stringtable.obj : stringtable.cpp ..\include\stringtable.h
    cl $(CPP_PROJ) stringtable.cpp

debug\STRUCTS.obj : STRUCTS.CPP
    cl $(CPP_PROJ) STRUCTS.CPP

debug\stsetup.obj : stsetup.cpp ..\include\stringcl.h ..\include\fileclas.h
    cl $(CPP_PROJ) stsetup.cpp

debug\textlen.obj : textlen.CPP
    cl $(CPP_PROJ) textlen.CPP

debug\TEXTLIST.obj : TEXTLIST.CPP
    cl $(CPP_PROJ) TEXTLIST.CPP

debug\temp_message.obj : temp_message.cpp ..\include\wclass.h
    cl $(CPP_PROJ) temp_message.cpp

debug\TIME.obj : TIME.CPP
    cl $(CPP_PROJ) TIME.CPP

debug\timeclas.obj : timeclas.cpp
    cl $(CPP_PROJ) timeclas.cpp

debug\UNITADJ.obj : UNITADJ.CPP
    cl $(CPP_PROJ) UNITADJ.CPP

debug\UNITS.obj : UNITS.CPP ..\include\units.h
    cl $(CPP_PROJ) UNITS.CPP

debug\updown.obj : updown.cpp
    cl $(CPP_PROJ) updown.cpp

debug\v5tov7.obj : v5tov7.cpp
    cl $(CPP_PROJ) v5tov7.cpp

debug\vdbclass.obj : vdbclass.cpp ..\include\vdbclass.h ..\include\multistring.h
    cl $(CPP_PROJ) vdbclass.cpp

debug\visigrid.obj : visigrid.cpp ..\include\visigrid.h
    cl $(CPP_PROJ) visigrid.cpp

debug\visiplot.obj : visiplot.cpp ..\include\visiplot.h ..\include\stringcl.h ..\include\wclass.h
    cl $(CPP_PROJ) visiplot.cpp

debug\warmupcl.obj : warmupcl.cpp
    cl $(CPP_PROJ) warmupcl.cpp

debug\wclass.obj : wclass.cpp ..\include\wclass.h
    cl $(CPP_PROJ) wclass.cpp

debug\WINSUBS.obj : WINSUBS.CPP
    cl $(CPP_PROJ) WINSUBS.CPP

debug\wire_class.obj : wire_class.cpp ..\include\wire_class.h
    cl $(CPP_PROJ) wire_class.cpp

debug\workclas.obj : workclas.cpp
    cl $(CPP_PROJ) workclas.cpp

debug\ZERO.obj : ZERO.CPP
    cl $(CPP_PROJ) ZERO.CPP


