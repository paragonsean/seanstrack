CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /D "UNICODE" /D "_UNICODE" /Fp"../include/ufree.pch" /Yu"windows.h" /Fo"ufree/" /c

LIB32_FLAGS=/nologo /out:"..\ufreelib\v5subs.lib"

..\ufreelib\v5subs.lib : ufree\alarmsum.obj ufree\array.obj ufree\ascfloat.obj \
 ufree\asensor.obj ufree\autoback.obj ufree\bitclass.obj ufree\callfile.obj ufree\catclass.obj ufree\chaxis.obj ufree\choose_wire.obj \
 ufree\client.obj ufree\color.obj ufree\colorcl.obj ufree\computer.obj ufree\copymach.obj \
 ufree\copypart.obj ufree\current.obj ufree\dateclas.obj ufree\db_to_vdb.obj ufree\dbsubs.obj ufree\dbupsize.obj \
 ufree\dcurve.obj ufree\dow.obj ufree\downsubs.obj ufree\downtime.obj ufree\dstat.obj ufree\dtotchar.obj \
 ufree\dynamic_file_name.obj \
 ufree\error_ms.obj ufree\evclass.obj ufree\execwait.obj ufree\exists.obj ufree\external_parameter.obj ufree\extract.obj \
 ufree\fifo.obj ufree\fileclas.obj ufree\fillpart.obj \
 ufree\fillshot.obj ufree\fix.obj ufree\fontclas.obj ufree\ftanalog.obj ufree\ftcalc.obj \
 ufree\ftii_file.obj  ufree\ftii_parameters.obj ufree\ftii_shot_data.obj ufree\ftiiclas.obj \
 ufree\ftime.obj ufree\ftoasc.obj ufree\genlist.obj ufree\get_ini.obj \
 ufree\get_tc.obj ufree\gexedir.obj ufree\gpvar.obj ufree\hex.obj ufree\hourglas.obj ufree\iniclass.obj \
 ufree\insalph.obj ufree\is_empty.obj ufree\itoasc.obj ufree\killdir.obj ufree\limit_switch_class.obj ufree\listbox.obj ufree\log_file_string.obj \
 ufree\machcomp.obj ufree\machine.obj ufree\machname.obj ufree\machine_to_hmi.obj ufree\marks.obj ufree\maximize.obj \
 ufree\mem.obj ufree\menu.obj ufree\menuctrl.obj ufree\mexists.obj ufree\misc.obj ufree\monrun.obj ufree\msgclas.obj ufree\multipart_runlist.obj ufree\multistring.obj \
 ufree\nameclas.obj ufree\optocontrol.obj ufree\param.obj ufree\param_index_class.obj ufree\part.obj ufree\partlist.obj ufree\path.obj \
 ufree\pexists.obj ufree\plookup.obj ufree\plotclas.obj ufree\plotsubs.obj ufree\pointcl.obj ufree\popctrl.obj ufree\postsec.obj \
 ufree\profiles.obj ufree\purge.obj ufree\pw.obj \
 ufree\readline.obj ufree\rectclas.obj ufree\reverse.obj ufree\ringcl.obj ufree\rjust.obj \
 ufree\runlist.obj \
 ufree\semaphor.obj ufree\setpoint.obj ufree\shift.obj ufree\shot_name.obj ufree\shot_name_reset.obj ufree\shotclas.obj ufree\sockcli.obj ufree\startpar.obj \
 ufree\statics.obj ufree\static_text_parameter.obj \
 ufree\stddown.obj ufree\stpres.obj ufree\strarray.obj \
 ufree\stparam.obj ufree\stringcl.obj ufree\stringtable.obj ufree\structs.obj ufree\stsetup.obj ufree\textlen.obj \
 ufree\textlist.obj ufree\temp_message.obj ufree\time.obj \
 ufree\timeclas.obj ufree\unitadj.obj ufree\units.obj ufree\updown.obj ufree\v5tov7.obj ufree\vdbclass.obj ufree\visigrid.obj ufree\visiplot.obj \
 ufree\warmupcl.obj ufree\wclass.obj ufree\winsubs.obj ufree\wire_class.obj ufree\workclas.obj ufree\write_shotparm_log.obj ufree\zero.obj
 link -lib $(LIB32_FLAGS) ufree\alarmsum.obj ufree\array.obj ufree\ascfloat.obj \
 ufree\asensor.obj ufree\bitclass.obj ufree\callfile.obj ufree\catclass.obj ufree\chaxis.obj ufree\choose_wire.obj \
 ufree\client.obj ufree\color.obj ufree\colorcl.obj ufree\computer.obj ufree\copymach.obj \
 ufree\copypart.obj ufree\current.obj ufree\dateclas.obj ufree\db_to_vdb.obj ufree\dbsubs.obj ufree\dbupsize.obj \
 ufree\dcurve.obj ufree\dow.obj ufree\downsubs.obj ufree\downtime.obj ufree\dstat.obj ufree\dtotchar.obj \
 ufree\dynamic_file_name.obj \
 ufree\error_ms.obj ufree\evclass.obj ufree\execwait.obj ufree\exists.obj ufree\external_parameter.obj ufree\extract.obj \
 ufree\fifo.obj ufree\fileclas.obj ufree\fillpart.obj \
 ufree\fillshot.obj ufree\fix.obj ufree\fontclas.obj ufree\ftcalc.obj ufree\ftanalog.obj \
 ufree\ftii_file.obj ufree\ftii_parameters.obj ufree\ftii_shot_data.obj ufree\ftiiclas.obj \
 ufree\ftime.obj ufree\ftoasc.obj ufree\genlist.obj ufree\get_ini.obj \
 ufree\get_tc.obj ufree\autoback.obj ufree\gexedir.obj ufree\gpvar.obj ufree\hex.obj ufree\hourglas.obj ufree\iniclass.obj \
 ufree\insalph.obj ufree\is_empty.obj ufree\itoasc.obj ufree\killdir.obj ufree\listbox.obj ufree\limit_switch_class.obj ufree\log_file_string.obj \
 ufree\machcomp.obj ufree\machine.obj ufree\machine_to_hmi.obj ufree\machname.obj ufree\marks.obj ufree\maximize.obj \
 ufree\mem.obj ufree\menu.obj ufree\menuctrl.obj ufree\mexists.obj ufree\misc.obj ufree\monrun.obj ufree\msgclas.obj ufree\multipart_runlist.obj ufree\multistring.obj \
 ufree\nameclas.obj ufree\optocontrol.obj ufree\param.obj ufree\param_index_class.obj ufree\part.obj ufree\partlist.obj \
 ufree\path.obj \
 ufree\pexists.obj ufree\plookup.obj ufree\plotclas.obj ufree\plotsubs.obj ufree\pointcl.obj ufree\popctrl.obj ufree\postsec.obj \
 ufree\profiles.obj ufree\purge.obj ufree\pw.obj \
 ufree\readline.obj ufree\rectclas.obj ufree\reverse.obj ufree\ringcl.obj ufree\rjust.obj \
 ufree\runlist.obj \
 ufree\semaphor.obj ufree\setpoint.obj ufree\shift.obj ufree\shot_name.obj ufree\shot_name_reset.obj  ufree\shotclas.obj ufree\sockcli.obj ufree\startpar.obj \
 ufree\statics.obj ufree\static_text_parameter.obj \
 ufree\stddown.obj ufree\stpres.obj ufree\strarray.obj \
 ufree\stparam.obj ufree\stringcl.obj ufree\stringtable.obj ufree\structs.obj ufree\stsetup.obj ufree\textlen.obj \
 ufree\textlist.obj ufree\temp_message.obj ufree\time.obj \
 ufree\timeclas.obj ufree\unitadj.obj ufree\units.obj ufree\updown.obj ufree\v5tov7.obj ufree\vdbclass.obj ufree\visigrid.obj ufree\visiplot.obj \
 ufree\warmupcl.obj ufree\wclass.obj ufree\winsubs.obj ufree\wire_class.obj  ufree\workclas.obj ufree\write_shotparm_log.obj ufree\zero.obj

ufree\alarmsum.obj : alarmsum.cpp
    cl $(CPP_PROJ) alarmsum.cpp

ufree\array.obj : array.cpp ..\include\stringcl.h ..\include\fileclas.h ..\include\array.h
    cl $(CPP_PROJ) array.cpp

ufree\ascfloat.obj : ascfloat.cpp
    cl $(CPP_PROJ) ascfloat.cpp

ufree\asensor.obj : asensor.cpp
    cl $(CPP_PROJ) asensor.cpp

ufree\autoback.obj : autoback.cpp ..\include\autoback.h
    cl $(CPP_PROJ) autoback.cpp

ufree\bitclass.obj : bitclass.cpp ..\include\bitclass.h
    cl $(CPP_PROJ) bitclass.cpp

ufree\callfile.obj : callfile.cpp
    cl $(CPP_PROJ) callfile.cpp

ufree\catclass.obj : catclass.cpp
    cl $(CPP_PROJ) catclass.cpp

ufree\chaxis.obj : chaxis.cpp ..\include\chaxis.h ..\include\fileclas.h
    cl $(CPP_PROJ) chaxis.cpp

ufree\choose_wire.obj : choose_wire.cpp
    cl $(CPP_PROJ) choose_wire.cpp

ufree\client.obj : client.cpp ..\include\events.h
    cl $(CPP_PROJ) client.cpp

ufree\color.obj : color.cpp ..\include\color.h
    cl $(CPP_PROJ) color.cpp

ufree\colorcl.obj : colorcl.cpp ..\include\colorcl.h
    cl $(CPP_PROJ) colorcl.cpp

ufree\computer.obj : computer.cpp ..\include\computer.h
    cl $(CPP_PROJ) computer.cpp

ufree\copymach.obj : copymach.cpp ..\include\computer.h ..\include\setpoint.h
    cl $(CPP_PROJ) copymach.cpp

ufree\copypart.obj : copypart.cpp ..\include\computer.h ..\include\ftii.h ..\include\part.h ..\include\setpoint.h
    cl $(CPP_PROJ) copypart.cpp

ufree\current.obj : current.cpp ..\include\setpoint.h
    cl $(CPP_PROJ) current.cpp

ufree\dateclas.obj : dateclas.cpp
    cl $(CPP_PROJ) dateclas.cpp

ufree\db_to_vdb.obj : db_to_vdb.cpp
    cl $(CPP_PROJ) db_to_vdb.cpp

ufree\dbsubs.obj : dbsubs.cpp
    cl $(CPP_PROJ) dbsubs.cpp

ufree\dbupsize.obj : dbupsize.cpp
    cl $(CPP_PROJ) dbupsize.cpp

ufree\dcurve.obj : dcurve.cpp
    cl $(CPP_PROJ) dcurve.cpp

ufree\dow.obj : dow.cpp
    cl $(CPP_PROJ) dow.cpp

ufree\downsubs.obj : downsubs.cpp
    cl $(CPP_PROJ) downsubs.cpp

ufree\downtime.obj : downtime.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) downtime.cpp

ufree\dstat.obj : dstat.cpp
    cl $(CPP_PROJ) dstat.cpp

ufree\dtotchar.obj : dtotchar.cpp
    cl $(CPP_PROJ) dtotchar.cpp

ufree\dynamic_file_name.obj : dynamic_file_name.cpp ..\include\stringcl.h ..\include\computer.h ..\include\timeclas.h
    cl $(CPP_PROJ) dynamic_file_name.cpp

ufree\error_ms.obj : error_ms.cpp
    cl $(CPP_PROJ) error_ms.cpp

ufree\evclass.obj : evclass.cpp ..\include\evclass.h
    cl $(CPP_PROJ) evclass.cpp

ufree\execwait.obj : execwait.cpp
    cl $(CPP_PROJ) execwait.cpp

ufree\exists.obj : exists.cpp
    cl $(CPP_PROJ) exists.cpp

ufree\external_parameter.obj : external_parameter.cpp
    cl $(CPP_PROJ) external_parameter.cpp

ufree\extract.obj : extract.cpp
    cl $(CPP_PROJ) extract.cpp

ufree\fifo.obj : fifo.cpp ..\include\fifo.h
    cl $(CPP_PROJ) fifo.cpp

ufree\fileclas.obj : fileclas.cpp ..\include\fileclas.h
    cl $(CPP_PROJ) fileclas.cpp

ufree\fillpart.obj : fillpart.cpp
    cl $(CPP_PROJ) fillpart.cpp

ufree\fillshot.obj : fillshot.cpp
    cl $(CPP_PROJ) fillshot.cpp

ufree\fix.obj : fix.cpp
    cl $(CPP_PROJ) fix.cpp

ufree\fontclas.obj : fontclas.cpp ..\include\fontclas.h
    cl $(CPP_PROJ) fontclas.cpp

ufree\ftcalc.obj : ftcalc.cpp ..\include\ftcalc.h
    cl $(CPP_PROJ) ftcalc.cpp

ufree\ftanalog.obj : ftanalog.cpp ..\include\ftanalog.h
    cl $(CPP_PROJ) ftanalog.cpp

ufree\ftii_file.obj : ftii_file.cpp ..\include\ftii_file.h
    cl $(CPP_PROJ) ftii_file.cpp

ufree\ftii_parameters.obj : ftii_parameters.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h ..\include\ftanalog.h
    cl $(CPP_PROJ) ftii_parameters.cpp

ufree\ftii_shot_data.obj : ftii_shot_data.cpp ..\include\ftii.h ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h
    cl $(CPP_PROJ) ftii_shot_data.cpp

ufree\ftiiclas.obj : ftiiclas.cpp ..\include\ftii.h ..\include\ftii_file.h
    cl $(CPP_PROJ) ftiiclas.cpp

ufree\ftime.obj : ftime.cpp
    cl $(CPP_PROJ) ftime.cpp

ufree\ftoasc.obj : ftoasc.cpp
    cl $(CPP_PROJ) ftoasc.cpp

ufree\genlist.obj : genlist.cpp
    cl $(CPP_PROJ) genlist.cpp

ufree\get_ini.obj : get_ini.cpp
    cl $(CPP_PROJ) get_ini.cpp

ufree\get_tc.obj : get_tc.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) get_tc.cpp

ufree\gexedir.obj : gexedir.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) gexedir.cpp

ufree\gpvar.obj : gpvar.cpp ..\include\fileclas.h
    cl $(CPP_PROJ) gpvar.cpp

ufree\hex.obj : hex.cpp
    cl $(CPP_PROJ) hex.cpp

ufree\hourglas.obj : hourglas.cpp
    cl $(CPP_PROJ) hourglas.cpp

ufree\iniclass.obj : iniclass.cpp ..\include\iniclass.h
    cl $(CPP_PROJ) iniclass.cpp

ufree\insalph.obj : insalph.cpp
    cl $(CPP_PROJ) insalph.cpp

ufree\is_empty.obj : is_empty.cpp
    cl $(CPP_PROJ) is_empty.cpp

ufree\itoasc.obj : itoasc.cpp
    cl $(CPP_PROJ) itoasc.cpp

ufree\killdir.obj : killdir.cpp
    cl $(CPP_PROJ) killdir.cpp

ufree\listbox.obj : listbox.cpp
    cl $(CPP_PROJ) listbox.cpp

ufree\limit_switch_class.obj : limit_switch_class.cpp  ..\include\limit_switch_class.h
    cl $(CPP_PROJ) limit_switch_class.cpp

ufree\log_file_string.obj : log_file_string.cpp
    cl $(CPP_PROJ) log_file_string.cpp

ufree\machcomp.obj : machcomp.cpp ..\include\computer.h
    cl $(CPP_PROJ) machcomp.cpp

ufree\machine.obj : machine.cpp ..\include\setpoint.h
    cl $(CPP_PROJ) machine.cpp

ufree\machine_to_hmi.obj : machine_to_hmi.cpp
    cl $(CPP_PROJ) machine_to_hmi.cpp

ufree\machname.obj : machname.cpp ..\include\machname.h
    cl $(CPP_PROJ) machname.cpp

ufree\marks.obj : marks.cpp
    cl $(CPP_PROJ) marks.cpp

ufree\maximize.obj : maximize.cpp
    cl $(CPP_PROJ) maximize.cpp

ufree\mem.obj : mem.cpp
    cl $(CPP_PROJ) mem.cpp

ufree\menu.obj : menu.cpp
    cl $(CPP_PROJ) menu.cpp

ufree\menuctrl.obj : menuctrl.cpp  ..\include\menuctrl.h
    cl $(CPP_PROJ) menuctrl.cpp

ufree\mexists.obj : mexists.cpp
    cl $(CPP_PROJ) mexists.cpp

ufree\misc.obj : misc.cpp
    cl $(CPP_PROJ) misc.cpp

ufree\monrun.obj : monrun.cpp
    cl $(CPP_PROJ) monrun.cpp

ufree\msgclas.obj : msgclas.cpp ..\include\msgclas.h
    cl $(CPP_PROJ) msgclas.cpp

ufree\multipart_runlist.obj : multipart_runlist.cpp ..\include\multipart_runlist.h
    cl $(CPP_PROJ) multipart_runlist.cpp

ufree\multistring.obj : multistring.cpp ..\include\multistring.h ..\include\stringcl.h
    cl $(CPP_PROJ) multistring.cpp

ufree\nameclas.obj : nameclas.cpp ..\include\computer.h ..\include\stringcl.h ..\include\nameclas.h
    cl $(CPP_PROJ) nameclas.cpp

ufree\optocontrol.obj : optocontrol.cpp
    cl $(CPP_PROJ) optocontrol.cpp

ufree\param.obj : param.cpp
    cl $(CPP_PROJ) param.cpp

ufree\param_index_class.obj : param_index_class.cpp ..\include\param_index_class.h
    cl $(CPP_PROJ) param_index_class.cpp

ufree\part.obj : part.cpp ..\include\part.h ..\include\stringcl.h ..\include\nameclas.h ..\include\setpoint.h
    cl $(CPP_PROJ) part.cpp

ufree\partlist.obj : partlist.cpp
    cl $(CPP_PROJ) partlist.cpp

ufree\path.obj : path.cpp
    cl $(CPP_PROJ) path.cpp

ufree\pexists.obj : pexists.cpp
    cl $(CPP_PROJ) pexists.cpp

ufree\plookup.obj : plookup.cpp
    cl $(CPP_PROJ) plookup.cpp

ufree\plotclas.obj : plotclas.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) plotclas.cpp

ufree\plotsubs.obj : plotsubs.cpp
    cl $(CPP_PROJ) plotsubs.cpp

ufree\pointcl.obj : pointcl.cpp ..\include\pointcl.h
    cl $(CPP_PROJ) pointcl.cpp

ufree\popctrl.obj : popctrl.cpp ..\include\popctrl.h
    cl $(CPP_PROJ) popctrl.cpp

ufree\postsec.obj : postsec.cpp
    cl $(CPP_PROJ) postsec.cpp

ufree\profiles.obj : profiles.cpp ..\include\stringcl.h ..\include\nameclas.h
    cl $(CPP_PROJ) profiles.cpp

ufree\purge.obj : purge.cpp ..\include\computer.h
    cl $(CPP_PROJ) purge.cpp

ufree\pw.obj : pw.cpp ..\include\computer.h
    cl $(CPP_PROJ) pw.cpp

ufree\readline.obj : readline.cpp
    cl $(CPP_PROJ) readline.cpp

ufree\rectclas.obj : rectclas.cpp ..\include\rectclas.h
    cl $(CPP_PROJ) rectclas.cpp

ufree\reverse.obj : reverse.cpp
    cl $(CPP_PROJ) reverse.cpp

ufree\ringcl.obj : ringcl.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) ringcl.cpp

ufree\rjust.obj : rjust.cpp
    cl $(CPP_PROJ) rjust.cpp

ufree\runlist.obj : runlist.cpp ..\include\runlist.h
    cl $(CPP_PROJ) runlist.cpp

ufree\semaphor.obj : semaphor.cpp
    cl $(CPP_PROJ) semaphor.cpp

ufree\setpoint.obj : setpoint.CPP ..\include\setpoint.h ..\include\stringcl.h
    cl $(CPP_PROJ) setpoint.CPP

ufree\shift.obj : shift.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) shift.cpp

ufree\shot_name.obj : shot_name.cpp ..\include\shotname.h ..\include\setpoint.h
    cl $(CPP_PROJ) shot_name.cpp

ufree\shot_name_reset.obj : shot_name_reset.cpp ..\include\shot_name_reset.h ..\include\setpoint.h
    cl $(CPP_PROJ) shot_name_reset.cpp

ufree\shotclas.obj : shotclas.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) shotclas.cpp

ufree\sockcli.obj : sockcli.cpp ..\include\sockcli.h
    cl $(CPP_PROJ) sockcli.cpp

ufree\statics.obj : statics.cpp ..\include\statics.h
    cl $(CPP_PROJ) statics.cpp

ufree\static_text_parameter.obj : static_text_parameter.cpp ..\include\static_text_parameter.h
    cl $(CPP_PROJ) static_text_parameter.cpp

ufree\startpar.obj : startpar.cpp
    cl $(CPP_PROJ) startpar.cpp

ufree\stddown.obj : stddown.cpp ..\include\stddown.h
    cl $(CPP_PROJ) stddown.cpp

ufree\stparam.obj : stparam.cpp ..\include\fileclas.h
    cl $(CPP_PROJ) stparam.cpp

ufree\stpres.obj : stpres.cpp ..\include\stringcl.h ..\include\fileclas.h
    cl $(CPP_PROJ) stpres.cpp

ufree\strarray.obj : strarray.cpp ..\include\strarray.h
    cl $(CPP_PROJ) strarray.cpp

ufree\stringcl.obj : stringcl.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) stringcl.cpp

ufree\stringtable.obj : stringtable.cpp ..\include\stringtable.h
    cl $(CPP_PROJ) stringtable.cpp

ufree\structs.obj : structs.cpp
    cl $(CPP_PROJ) STRUCTS.CPP

ufree\stsetup.obj : stsetup.cpp ..\include\stringcl.h ..\include\fileclas.h
    cl $(CPP_PROJ) stsetup.cpp

ufree\textlen.obj : textlen.CPP
    cl $(CPP_PROJ) textlen.CPP

ufree\textlist.obj : textlist.CPP
    cl $(CPP_PROJ) textlist.CPP

ufree\temp_message.obj : temp_message.cpp ..\include\wclass.h
    cl $(CPP_PROJ) temp_message.cpp

ufree\time.obj : time.cpp
    cl $(CPP_PROJ) time.cpp

ufree\timeclas.obj : timeclas.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) timeclas.cpp

ufree\unitadj.obj : unitadj.cpp
    cl $(CPP_PROJ) unitadj.cpp

ufree\units.obj : units.cpp ..\include\units.h
    cl $(CPP_PROJ) units.cpp

ufree\updown.obj : updown.cpp
    cl $(CPP_PROJ) updown.cpp

ufree\v5tov7.obj : v5tov7.cpp
    cl $(CPP_PROJ) v5tov7.cpp

ufree\vdbclass.obj : vdbclass.cpp ..\include\vdbclass.h ..\include\multistring.h
    cl $(CPP_PROJ) vdbclass.cpp

ufree\visigrid.obj : visigrid.cpp ..\include\visigrid.h
    cl $(CPP_PROJ) visigrid.cpp

ufree\visiplot.obj : visiplot.cpp ..\include\visiplot.h ..\include\stringcl.h ..\include\wclass.h
    cl $(CPP_PROJ) visiplot.cpp

ufree\warmupcl.obj : warmupcl.cpp ..\include\warmupcl.h ..\include\iniclass.h
    cl $(CPP_PROJ) warmupcl.cpp

ufree\wclass.obj : wclass.cpp ..\include\wclass.h
    cl $(CPP_PROJ) wclass.cpp

ufree\winsubs.obj : winsubs.cpp
    cl $(CPP_PROJ) winsubs.cpp

ufree\wire_class.obj : wire_class.cpp ..\include\wire_class.h
    cl $(CPP_PROJ) wire_class.cpp

ufree\workclas.obj : workclas.cpp
    cl $(CPP_PROJ) workclas.cpp

ufree\write_shotparm_log.obj : write_shotparm_log.cpp
    cl $(CPP_PROJ) write_shotparm_log.cpp

ufree\zero.obj : zero.cpp
    cl $(CPP_PROJ) zero.cpp

