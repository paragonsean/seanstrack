CPP_PROJ=/nologo /MTd /W3 /Od /Zp1 /Z7 /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"../include/udebug.pch" /Yu"windows.h" /Fo"udebug/" /c

LIB32_FLAGS=/nologo /out:"..\udebuglib\v5subs.lib"

..\udebuglib\v5subs.lib : udebug\alarmsum.obj udebug\array.obj udebug\ascfloat.obj \
 udebug\asensor.obj udebug\bitclass.obj udebug\callfile.obj udebug\catclass.obj udebug\chaxis.obj udebug\choose_wire.obj \
 udebug\client.obj udebug\color.obj udebug\colorcl.obj udebug\computer.obj udebug\copymach.obj \
 udebug\copypart.obj udebug\current.obj udebug\dateclas.obj udebug\db_to_vdb.obj udebug\dbsubs.obj udebug\dbupsize.obj \
 udebug\dcurve.obj udebug\dow.obj udebug\downsubs.obj udebug\downtime.obj udebug\dstat.obj udebug\dtotchar.obj \
 udebug\dynamic_file_name.obj \
 udebug\error_ms.obj udebug\evclass.obj udebug\execwait.obj udebug\exists.obj udebug\external_parameter.obj udebug\extract.obj \
 udebug\fifo.obj udebug\fileclas.obj udebug\fillpart.obj \
 udebug\fillshot.obj udebug\fix.obj udebug\fontclas.obj udebug\ftcalc.obj udebug\ftanalog.obj \
 udebug\ftii_file.obj udebug\ftii_parameters.obj udebug\ftii_shot_data.obj udebug\ftiiclas.obj \
 udebug\ftime.obj udebug\ftoasc.obj udebug\genlist.obj udebug\get_ini.obj \
 udebug\get_tc.obj udebug\autoback.obj udebug\gexedir.obj udebug\gpvar.obj udebug\hex.obj udebug\hourglas.obj udebug\iniclass.obj \
 udebug\insalph.obj udebug\is_empty.obj udebug\itoasc.obj udebug\killdir.obj udebug\listbox.obj udebug\limit_switch_class.obj udebug\log_file_string.obj \
 udebug\machcomp.obj udebug\machine.obj udebug\machine_to_hmi.obj udebug\machname.obj udebug\marks.obj udebug\maximize.obj \
 udebug\mem.obj udebug\menu.obj udebug\menuctrl.obj udebug\mexists.obj udebug\misc.obj udebug\monrun.obj udebug\msgclas.obj udebug\multipart_runlist.obj udebug\multistring.obj \
 udebug\nameclas.obj udebug\optocontrol.obj udebug\param.obj udebug\param_index_class.obj udebug\part.obj udebug\partlist.obj udebug\path.obj \
 udebug\pexists.obj udebug\plookup.obj udebug\plotclas.obj udebug\plotsubs.obj udebug\pointcl.obj udebug\popctrl.obj udebug\postsec.obj \
 udebug\profiles.obj udebug\purge.obj udebug\pw.obj \
 udebug\readline.obj udebug\rectclas.obj udebug\reverse.obj udebug\ringcl.obj udebug\rjust.obj \
 udebug\runlist.obj \
 udebug\semaphor.obj udebug\setpoint.obj udebug\shift.obj udebug\shot_name.obj udebug\shot_name_reset.obj udebug\shotclas.obj udebug\sockcli.obj \
 udebug\statics.obj udebug\static_text_parameter.obj  udebug\startpar.obj udebug\stddown.obj \
 udebug\stpres.obj udebug\strarray.obj \
 udebug\stparam.obj udebug\stringcl.obj udebug\stringtable.obj udebug\structs.obj udebug\stsetup.obj udebug\textlen.obj \
 udebug\textlist.obj udebug\temp_message.obj udebug\time.obj \
 udebug\timeclas.obj udebug\unitadj.obj udebug\units.obj udebug\updown.obj udebug\v5tov7.obj udebug\vdbclass.obj udebug\visigrid.obj udebug\visiplot.obj \
 udebug\warmupcl.obj udebug\wclass.obj udebug\winsubs.obj udebug\wire_class.obj udebug\workclas.obj udebug\zero.obj
 link -lib $(LIB32_FLAGS) udebug\alarmsum.obj udebug\array.obj udebug\ascfloat.obj \
 udebug\asensor.obj udebug\bitclass.obj udebug\callfile.obj udebug\catclass.obj udebug\chaxis.obj udebug\choose_wire.obj \
 udebug\client.obj udebug\color.obj udebug\colorcl.obj udebug\computer.obj udebug\copymach.obj \
 udebug\copypart.obj udebug\current.obj udebug\dateclas.obj udebug\db_to_vdb.obj udebug\dbsubs.obj udebug\dbupsize.obj \
 udebug\dcurve.obj udebug\dow.obj udebug\downsubs.obj udebug\downtime.obj udebug\dstat.obj udebug\dtotchar.obj \
 udebug\dynamic_file_name.obj \
 udebug\error_ms.obj udebug\evclass.obj udebug\execwait.obj udebug\exists.obj udebug\external_parameter.obj udebug\extract.obj \
 udebug\fifo.obj udebug\fileclas.obj udebug\fillpart.obj \
 udebug\fillshot.obj udebug\fix.obj udebug\fontclas.obj udebug\ftcalc.obj udebug\ftanalog.obj \
 udebug\ftii_file.obj udebug\ftii_parameters.obj udebug\ftii_shot_data.obj udebug\ftiiclas.obj \
 udebug\ftime.obj udebug\ftoasc.obj udebug\genlist.obj udebug\get_ini.obj \
 udebug\get_tc.obj udebug\autoback.obj udebug\gexedir.obj udebug\gpvar.obj udebug\hex.obj udebug\hourglas.obj udebug\iniclass.obj \
 udebug\insalph.obj udebug\is_empty.obj udebug\itoasc.obj udebug\killdir.obj udebug\listbox.obj udebug\limit_switch_class.obj udebug\log_file_string.obj \
 udebug\machcomp.obj udebug\machine.obj udebug\machine_to_hmi.obj udebug\machname.obj udebug\marks.obj udebug\maximize.obj \
 udebug\mem.obj udebug\menu.obj udebug\menuctrl.obj udebug\mexists.obj udebug\misc.obj udebug\monrun.obj udebug\msgclas.obj udebug\multipart_runlist.obj udebug\multistring.obj \
 udebug\nameclas.obj udebug\optocontrol.obj udebug\param.obj udebug\param_index_class.obj udebug\part.obj udebug\partlist.obj \
 udebug\path.obj \
 udebug\pexists.obj udebug\plookup.obj udebug\plotclas.obj udebug\plotsubs.obj udebug\pointcl.obj udebug\popctrl.obj udebug\postsec.obj \
 udebug\profiles.obj udebug\purge.obj udebug\pw.obj \
 udebug\readline.obj udebug\rectclas.obj udebug\reverse.obj udebug\ringcl.obj udebug\rjust.obj \
 udebug\runlist.obj \
 udebug\semaphor.obj udebug\setpoint.obj udebug\shift.obj udebug\shot_name.obj udebug\shot_name_reset.obj udebug\shotclas.obj udebug\sockcli.obj \
 udebug\statics.obj udebug\static_text_parameter.obj \
 udebug\startpar.obj udebug\stddown.obj udebug\stpres.obj udebug\strarray.obj \
 udebug\stparam.obj udebug\stringcl.obj udebug\stringtable.obj udebug\structs.obj udebug\stsetup.obj udebug\textlen.obj \
 udebug\textlist.obj udebug\temp_message.obj udebug\time.obj \
 udebug\timeclas.obj udebug\unitadj.obj udebug\units.obj udebug\updown.obj udebug\v5tov7.obj udebug\vdbclass.obj udebug\visigrid.obj udebug\visiplot.obj \
 udebug\warmupcl.obj udebug\wclass.obj udebug\winsubs.obj udebug\wire_class.obj udebug\workclas.obj udebug\zero.obj

udebug\alarmsum.obj : alarmsum.cpp
    cl $(CPP_PROJ) alarmsum.cpp

udebug\array.obj : array.cpp
    cl $(CPP_PROJ) array.cpp

udebug\ascfloat.obj : ascfloat.cpp
    cl $(CPP_PROJ) ascfloat.cpp

udebug\asensor.obj : asensor.cpp
    cl $(CPP_PROJ) asensor.cpp

udebug\autoback.obj : autoback.cpp ..\include\autoback.h
    cl $(CPP_PROJ) autoback.cpp

udebug\bitclass.obj : bitclass.cpp ..\include\bitclass.h
    cl $(CPP_PROJ) bitclass.cpp

udebug\callfile.obj : callfile.cpp
    cl $(CPP_PROJ) callfile.cpp

udebug\catclass.obj : catclass.cpp
    cl $(CPP_PROJ) catclass.cpp

udebug\chaxis.obj : chaxis.cpp ..\include\chaxis.h
    cl $(CPP_PROJ) chaxis.cpp

udebug\choose_wire.obj : choose_wire.cpp
    cl $(CPP_PROJ) choose_wire.cpp

udebug\client.obj : client.cpp
    cl $(CPP_PROJ) client.cpp

udebug\color.obj : color.cpp ..\include\color.h
    cl $(CPP_PROJ) color.cpp

udebug\colorcl.obj : colorcl.cpp ..\include\colorcl.h
    cl $(CPP_PROJ) colorcl.cpp

udebug\computer.obj : computer.cpp ..\include\computer.h
    cl $(CPP_PROJ) computer.cpp

udebug\copymach.obj : copymach.cpp
    cl $(CPP_PROJ) copymach.cpp

udebug\copypart.obj : copypart.cpp
    cl $(CPP_PROJ) copypart.cpp

udebug\current.obj : current.cpp
    cl $(CPP_PROJ) current.cpp

udebug\dateclas.obj : dateclas.cpp
    cl $(CPP_PROJ) dateclas.cpp

udebug\db_to_vdb.obj : db_to_vdb.CPP
    cl $(CPP_PROJ) db_to_vdb.CPP

udebug\dbsubs.obj : dbsubs.cpp
    cl $(CPP_PROJ) dbsubs.cpp

udebug\dbupsize.obj : dbupsize.CPP
    cl $(CPP_PROJ) dbupsize.CPP

udebug\dcurve.obj : dcurve.cpp
    cl $(CPP_PROJ) dcurve.cpp

udebug\dow.obj : dow.cpp
    cl $(CPP_PROJ) dow.cpp

udebug\downsubs.obj : downsubs.cpp
    cl $(CPP_PROJ) downsubs.cpp

udebug\downtime.obj : downtime.cpp
    cl $(CPP_PROJ) downtime.cpp

udebug\dstat.obj : dstat.cpp
    cl $(CPP_PROJ) dstat.cpp

udebug\dtotchar.obj : dtotchar.cpp
    cl $(CPP_PROJ) dtotchar.cpp

udebug\dynamic_file_name.obj : dynamic_file_name.cpp
    cl $(CPP_PROJ) dynamic_file_name.cpp

udebug\error_ms.obj : error_ms.cpp
    cl $(CPP_PROJ) error_ms.cpp

udebug\evclass.obj : evclass.cpp ..\include\evclass.h
    cl $(CPP_PROJ) evclass.cpp

udebug\execwait.obj : execwait.cpp
    cl $(CPP_PROJ) execwait.cpp

udebug\exists.obj : exists.cpp
    cl $(CPP_PROJ) exists.cpp

udebug\external_parameter.obj : external_parameter.cpp
    cl $(CPP_PROJ) external_parameter.cpp

udebug\extract.obj : extract.cpp
    cl $(CPP_PROJ) extract.cpp

udebug\fifo.obj : fifo.cpp
    cl $(CPP_PROJ) fifo.cpp

udebug\fileclas.obj : fileclas.cpp ..\include\fileclas.h
    cl $(CPP_PROJ) fileclas.cpp

udebug\fillpart.obj : fillpart.cpp
    cl $(CPP_PROJ) fillpart.cpp

udebug\fillshot.obj : fillshot.cpp
    cl $(CPP_PROJ) fillshot.cpp

udebug\fix.obj : fix.cpp
    cl $(CPP_PROJ) fix.cpp

udebug\fontclas.obj : fontclas.cpp
    cl $(CPP_PROJ) fontclas.cpp

udebug\ftanalog.obj : ftanalog.cpp
    cl $(CPP_PROJ) ftanalog.cpp

udebug\ftcalc.obj : ftcalc.cpp ..\include\ftcalc.h
    cl $(CPP_PROJ) ftcalc.cpp

udebug\ftii_file.obj : ftii_file.cpp ..\include\ftii_file.h
    cl $(CPP_PROJ) ftii_file.cpp

udebug\ftii_parameters.obj : ftii_parameters.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h ..\include\ftanalog.h
    cl $(CPP_PROJ) ftii_parameters.cpp

udebug\ftii_shot_data.obj : ftii_shot_data.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h
    cl $(CPP_PROJ) ftii_shot_data.cpp

udebug\ftiiclas.obj : ftiiclas.cpp ..\include\ftii.h ..\include\ftii_file.h
    cl $(CPP_PROJ) ftiiclas.cpp

udebug\ftime.obj : ftime.cpp
    cl $(CPP_PROJ) ftime.cpp

udebug\ftoasc.obj : ftoasc.cpp
    cl $(CPP_PROJ) ftoasc.cpp

udebug\genlist.obj : genlist.cpp
    cl $(CPP_PROJ) genlist.cpp

udebug\get_ini.obj : get_ini.cpp
    cl $(CPP_PROJ) get_ini.cpp

udebug\get_tc.obj : get_tc.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) get_tc.cpp

udebug\gexedir.obj : gexedir.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) gexedir.cpp

udebug\gpvar.obj : gpvar.cpp
    cl $(CPP_PROJ) gpvar.cpp

udebug\hex.obj : hex.cpp
    cl $(CPP_PROJ) hex.cpp

udebug\hourglas.obj : hourglas.cpp
    cl $(CPP_PROJ) hourglas.cpp

udebug\iniclass.obj : iniclass.cpp ..\include\iniclass.h
    cl $(CPP_PROJ) iniclass.cpp

udebug\insalph.obj : insalph.cpp
    cl $(CPP_PROJ) insalph.cpp

udebug\is_empty.obj : is_empty.cpp
    cl $(CPP_PROJ) is_empty.cpp

udebug\itoasc.obj : itoasc.cpp
    cl $(CPP_PROJ) itoasc.cpp

udebug\killdir.obj : killdir.cpp
    cl $(CPP_PROJ) killdir.cpp

udebug\limit_switch_class.obj : limit_switch_class.cpp  ..\include\limit_switch_class.h
    cl $(CPP_PROJ) limit_switch_class.cpp

udebug\listbox.obj : listbox.cpp
    cl $(CPP_PROJ) listbox.cpp

udebug\log_file_string.obj : log_file_string.cpp
    cl $(CPP_PROJ) log_file_string.cpp

udebug\machcomp.obj : machcomp.cpp
    cl $(CPP_PROJ) machcomp.cpp

udebug\machine.obj : machine.cpp ..\include\setpoint.h
    cl $(CPP_PROJ) machine.cpp

udebug\machine_to_hmi.obj : machine_to_hmi.cpp
    cl $(CPP_PROJ) machine_to_hmi.cpp

udebug\machname.obj : machname.cpp ..\include\machname.h
    cl $(CPP_PROJ) machname.cpp

udebug\marks.obj : marks.cpp
    cl $(CPP_PROJ) marks.cpp

udebug\maximize.obj : maximize.cpp
    cl $(CPP_PROJ) maximize.cpp

udebug\mem.obj : mem.cpp
    cl $(CPP_PROJ) mem.cpp

udebug\menu.obj : menu.cpp
    cl $(CPP_PROJ) menu.cpp

udebug\menuctrl.obj : menuctrl.cpp ..\include\menuctrl.h
    cl $(CPP_PROJ) menuctrl.cpp

udebug\mexists.obj : mexists.cpp
    cl $(CPP_PROJ) mexists.cpp

udebug\misc.obj : misc.cpp
    cl $(CPP_PROJ) misc.cpp

udebug\monrun.obj : monrun.cpp
    cl $(CPP_PROJ) monrun.cpp

udebug\msgclas.obj : msgclas.cpp ..\include\msgclas.h
    cl $(CPP_PROJ) msgclas.cpp

udebug\multipart_runlist.obj : multipart_runlist.cpp ..\include\multipart_runlist.h
    cl $(CPP_PROJ) multipart_runlist.cpp

udebug\multistring.obj : multistring.cpp ..\include\multistring.h
    cl $(CPP_PROJ) multistring.cpp

udebug\nameclas.obj : nameclas.cpp ..\include\computer.h ..\include\nameclas.h ..\include\stringcl.h
    cl $(CPP_PROJ) nameclas.cpp

udebug\optocontrol.obj : optocontrol.cpp
    cl $(CPP_PROJ) optocontrol.cpp

udebug\param.obj : param.cpp
    cl $(CPP_PROJ) param.cpp

udebug\param_index_class.obj : param_index_class.cpp ..\include\param_index_class.h
    cl $(CPP_PROJ) param_index_class.cpp

udebug\part.obj : part.cpp
    cl $(CPP_PROJ) part.cpp

udebug\partlist.obj : partlist.cpp
    cl $(CPP_PROJ) partlist.cpp

udebug\path.obj : path.cpp
    cl $(CPP_PROJ) path.cpp

udebug\pexists.obj : pexists.cpp
    cl $(CPP_PROJ) pexists.cpp

udebug\plookup.obj : plookup.cpp ..\include\setpoint.h ..\include\plookup.h
    cl $(CPP_PROJ) plookup.cpp

udebug\plotclas.obj : plotclas.cpp
    cl $(CPP_PROJ) plotclas.cpp

udebug\plotsubs.obj : plotsubs.cpp
    cl $(CPP_PROJ) plotsubs.cpp

udebug\pointcl.obj : pointcl.cpp ..\include\pointcl.h
    cl $(CPP_PROJ) pointcl.cpp

udebug\popctrl.obj : popctrl.cpp ..\include\popctrl.h
    cl $(CPP_PROJ) popctrl.cpp

udebug\postsec.obj : postsec.cpp
    cl $(CPP_PROJ) postsec.cpp

udebug\profiles.obj : profiles.cpp
    cl $(CPP_PROJ) profiles.cpp

udebug\purge.obj : purge.cpp
    cl $(CPP_PROJ) purge.cpp

udebug\pw.obj : pw.cpp
    cl $(CPP_PROJ) pw.cpp

udebug\readline.obj : readline.cpp
    cl $(CPP_PROJ) readline.cpp

udebug\rectclas.obj : rectclas.cpp  ..\include\rectclas.h
    cl $(CPP_PROJ) rectclas.cpp

udebug\reverse.obj : reverse.cpp
    cl $(CPP_PROJ) reverse.cpp

udebug\ringcl.obj : ringcl.cpp
    cl $(CPP_PROJ) ringcl.cpp

udebug\rjust.obj : rjust.cpp
    cl $(CPP_PROJ) rjust.cpp

udebug\runlist.obj : runlist.cpp  ..\include\runlist.h
    cl $(CPP_PROJ) runlist.cpp

udebug\semaphor.obj : semaphor.cpp
    cl $(CPP_PROJ) semaphor.cpp

udebug\setpoint.obj : setpoint.CPP ..\include\setpoint.h ..\include\stringcl.h
    cl $(CPP_PROJ) setpoint.CPP

udebug\shift.obj : shift.cpp
    cl $(CPP_PROJ) shift.cpp

udebug\shot_name.obj : shot_name.cpp ..\include\shotname.h ..\include\setpoint.h
    cl $(CPP_PROJ) shot_name.cpp

udebug\shot_name_reset.obj : shot_name_reset.cpp ..\include\shot_name_reset.h ..\include\setpoint.h
    cl $(CPP_PROJ) shot_name_reset.cpp

udebug\shotclas.obj : shotclas.cpp
    cl $(CPP_PROJ) shotclas.cpp

udebug\sockcli.obj : sockcli.cpp
    cl $(CPP_PROJ) sockcli.cpp

udebug\statics.obj : statics.cpp ..\include\statics.h
    cl $(CPP_PROJ) statics.cpp

udebug\static_text_parameter.obj : static_text_parameter.cpp ..\include\static_text_parameter.h
    cl $(CPP_PROJ) static_text_parameter.cpp

udebug\startpar.obj : startpar.cpp
    cl $(CPP_PROJ) startpar.cpp

udebug\stddown.obj : stddown.cpp ..\include\stddown.h
    cl $(CPP_PROJ) stddown.cpp

udebug\stparam.obj : stparam.cpp
    cl $(CPP_PROJ) stparam.cpp

udebug\stpres.obj : stpres.cpp
    cl $(CPP_PROJ) stpres.cpp

udebug\strarray.obj : strarray.cpp ..\include\strarray.h
    cl $(CPP_PROJ) strarray.cpp

udebug\stringcl.obj : stringcl.cpp
    cl $(CPP_PROJ) stringcl.cpp

udebug\stringtable.obj : stringtable.cpp ..\include\stringtable.h
    cl $(CPP_PROJ) stringtable.cpp

udebug\structs.obj : structs.cpp
    cl $(CPP_PROJ) structs.cpp

udebug\stsetup.obj : stsetup.cpp
    cl $(CPP_PROJ) stsetup.cpp

udebug\textlen.obj : textlen.cpp
    cl $(CPP_PROJ) textlen.cpp

udebug\textlist.obj : textlist.cpp
    cl $(CPP_PROJ) textlist.cpp

udebug\temp_message.obj : temp_message.cpp
    cl $(CPP_PROJ) temp_message.cpp

udebug\time.obj : time.cpp
    cl $(CPP_PROJ) time.cpp

udebug\timeclas.obj : timeclas.cpp
    cl $(CPP_PROJ) timeclas.cpp

udebug\unitadj.obj : unitadj.cpp
    cl $(CPP_PROJ) unitadj.cpp

udebug\units.obj : units.cpp
    cl $(CPP_PROJ) units.cpp

udebug\updown.obj : updown.cpp
    cl $(CPP_PROJ) updown.cpp

udebug\v5tov7.obj : v5tov7.cpp
    cl $(CPP_PROJ) v5tov7.cpp

udebug\vdbclass.obj : vdbclass.cpp ..\include\vdbclass.h ..\include\multistring.h
    cl $(CPP_PROJ) vdbclass.cpp

udebug\visigrid.obj : visigrid.cpp ..\include\visigrid.h
    cl $(CPP_PROJ) visigrid.cpp

udebug\visiplot.obj : visiplot.cpp
    cl $(CPP_PROJ) visiplot.cpp

udebug\warmupcl.obj : warmupcl.cpp
    cl $(CPP_PROJ) warmupcl.cpp

udebug\wclass.obj : wclass.cpp
    cl $(CPP_PROJ) wclass.cpp

udebug\winsubs.obj : winsubs.cpp
    cl $(CPP_PROJ) winsubs.cpp

udebug\wire_class.obj : wire_class.cpp ..\include\wire_class.h
    cl $(CPP_PROJ) wire_class.cpp

udebug\workclas.obj : workclas.cpp
    cl $(CPP_PROJ) workclas.cpp

udebug\zero.obj : zero.cpp
    cl $(CPP_PROJ) zero.cpp
