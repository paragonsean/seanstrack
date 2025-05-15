CPP_PROJ=/nologo /MT /W3 /Zp1 /O1 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_LIB" /Fp"../include/free.pch" /Yu"windows.h" /Fo"free/" /c

LIB32_FLAGS=/nologo /out:"..\freelib\v5subs.lib"

..\freelib\v5subs.lib : free\alarmsum.obj free\array.obj free\ascfloat.obj \
 free\asensor.obj free\bitclass.obj free\callfile.obj free\catclass.obj free\chaxis.obj free\choose_wire.obj \
 free\client.obj free\color.obj free\colorcl.obj free\computer.obj free\copymach.obj \
 free\copypart.obj free\current.obj free\dateclas.obj free\dbsubs.obj \
 free\dcurve.obj free\dow.obj free\downsubs.obj free\downtime.obj free\dstat.obj free\dtotchar.obj \
 free\dynamic_file_name.obj free\dynamic_output_wire_description.obj \
 free\error_ms.obj free\evclass.obj free\execwait.obj free\exists.obj free\extract.obj \
 free\fifo.obj free\fileclas.obj free\fillpart.obj \
 free\fillshot.obj free\fix.obj free\fontclas.obj free\ftcalc.obj free\ftanalog.obj \
 free\ftii_file.obj  free\ftii_parameters.obj free\ftii_shot_data.obj free\ftiiclas.obj \
 free\ftime.obj free\ftoasc.obj free\genlist.obj free\get_ini.obj \
 free\get_tc.obj free\autoback.obj free\gexedir.obj free\gpvar.obj free\hex.obj free\hourglas.obj free\iniclass.obj \
 free\insalph.obj free\is_empty.obj free\itoasc.obj free\killdir.obj free\listbox.obj free\limit_switch_class.obj free\log_file_string.obj \
 free\machcomp.obj free\machine.obj free\machine_to_hmi.obj free\machname.obj free\marks.obj free\maximize.obj \
 free\mem.obj free\menu.obj free\menuctrl.obj free\mexists.obj free\misc.obj free\monrun.obj free\msgclas.obj \
 free\nameclas.obj free\optocontrol.obj free\output_label_class.obj free\param.obj free\param_index_class.obj free\part.obj free\partlist.obj \
 free\path.obj \
 free\pexists.obj free\plookup.obj free\plotclas.obj free\plotsubs.obj free\pointcl.obj free\popctrl.obj free\postsec.obj \
 free\profiles.obj free\pw.obj \
 free\readline.obj free\rectclas.obj free\reverse.obj free\ringcl.obj free\rjust.obj \
 free\runlist.obj \
 free\semaphor.obj free\setpoint.obj free\shift.obj free\shotclas.obj free\sockcli.obj free\startpar.obj \
 free\stddown.obj free\stpres.obj free\strarray.obj \
 free\stparam.obj free\stringcl.obj free\stringtable.obj free\structs.obj free\stsetup.obj free\textlen.obj \
 free\textlist.obj free\temp_message.obj free\time.obj \
 free\timeclas.obj free\unitadj.obj free\units.obj free\updown.obj free\v5tov7.obj free\visigrid.obj free\visiplot.obj \
 free\warmupcl.obj free\wclass.obj free\winsubs.obj free\workclas.obj free\writelin.obj free\zero.obj
 link -lib $(LIB32_FLAGS) free\alarmsum.obj free\array.obj free\ascfloat.obj \
 free\asensor.obj free\bitclass.obj free\callfile.obj free\catclass.obj free\chaxis.obj free\choose_wire.obj \
 free\client.obj free\color.obj free\colorcl.obj free\computer.obj free\copymach.obj \
 free\copypart.obj free\current.obj free\dateclas.obj free\dbsubs.obj \
 free\dcurve.obj free\dow.obj free\downsubs.obj free\downtime.obj free\dstat.obj free\dtotchar.obj \
 free\dynamic_file_name.obj free\dynamic_output_wire_description.obj \
 free\error_ms.obj free\evclass.obj free\execwait.obj free\exists.obj free\extract.obj \
 free\fifo.obj free\fileclas.obj free\fillpart.obj \
 free\fillshot.obj free\fix.obj free\fontclas.obj free\ftcalc.obj free\ftanalog.obj \
 free\ftii_file.obj free\ftii_parameters.obj free\ftii_shot_data.obj free\ftiiclas.obj \
 free\ftime.obj free\ftoasc.obj free\genlist.obj free\get_ini.obj \
 free\get_tc.obj free\autoback.obj free\gexedir.obj free\gpvar.obj free\hex.obj free\hourglas.obj free\iniclass.obj \
 free\insalph.obj free\is_empty.obj free\itoasc.obj free\killdir.obj free\listbox.obj free\limit_switch_class.obj free\log_file_string.obj \
 free\machcomp.obj free\machine.obj free\machine_to_hmi.obj free\machname.obj free\marks.obj free\maximize.obj \
 free\mem.obj free\menu.obj free\menuctrl.obj free\mexists.obj free\misc.obj free\monrun.obj free\msgclas.obj \
 free\nameclas.obj free\optocontrol.obj free\output_label_class.obj free\param.obj free\param_index_class.obj free\part.obj free\partlist.obj \
 free\path.obj \
 free\pexists.obj free\plookup.obj free\plotclas.obj free\plotsubs.obj free\pointcl.obj free\popctrl.obj free\postsec.obj \
 free\profiles.obj free\pw.obj \
 free\readline.obj free\rectclas.obj free\reverse.obj free\ringcl.obj free\rjust.obj \
 free\runlist.obj \
 free\semaphor.obj free\setpoint.obj free\shift.obj free\shotclas.obj free\sockcli.obj free\startpar.obj \
 free\stddown.obj free\stpres.obj free\strarray.obj \
 free\stparam.obj free\stringcl.obj free\stringtable.obj free\structs.obj free\stsetup.obj free\textlen.obj \
 free\textlist.obj free\temp_message.obj free\time.obj \
 free\timeclas.obj free\unitadj.obj free\units.obj free\updown.obj free\v5tov7.obj free\visigrid.obj free\visiplot.obj \
 free\warmupcl.obj free\wclass.obj free\winsubs.obj free\workclas.obj free\writelin.obj free\zero.obj

free\alarmsum.obj : alarmsum.cpp
    cl $(CPP_PROJ) alarmsum.cpp

free\array.obj : array.cpp ..\include\stringcl.h ..\include\fileclas.h ..\include\array.h
    cl $(CPP_PROJ) array.cpp

free\ascfloat.obj : ascfloat.cpp
    cl $(CPP_PROJ) ascfloat.cpp

free\asensor.obj : asensor.cpp
    cl $(CPP_PROJ) asensor.cpp

free\autoback.obj : autoback.cpp ..\include\autoback.h
    cl $(CPP_PROJ) autoback.cpp

free\bitclass.obj : bitclass.cpp ..\include\bitclass.h
    cl $(CPP_PROJ) bitclass.cpp

free\callfile.obj : callfile.cpp
    cl $(CPP_PROJ) callfile.cpp

free\catclass.obj : catclass.cpp
    cl $(CPP_PROJ) catclass.cpp

free\chaxis.obj : chaxis.cpp ..\include\chaxis.h ..\include\fileclas.h
    cl $(CPP_PROJ) chaxis.cpp

free\choose_wire.obj : choose_wire.cpp
    cl $(CPP_PROJ) choose_wire.cpp

free\client.obj : client.cpp ..\include\events.h
    cl $(CPP_PROJ) client.cpp

free\color.obj : color.cpp ..\include\color.h
    cl $(CPP_PROJ) color.cpp

free\colorcl.obj : colorcl.cpp ..\include\colorcl.h
    cl $(CPP_PROJ) colorcl.cpp

free\computer.obj : computer.cpp ..\include\computer.h
    cl $(CPP_PROJ) computer.cpp

free\copymach.obj : copymach.cpp ..\include\computer.h ..\include\setpoint.h
    cl $(CPP_PROJ) copymach.cpp

free\copypart.obj : copypart.cpp ..\include\computer.h ..\include\ftii.h ..\include\part.h ..\include\setpoint.h
    cl $(CPP_PROJ) copypart.cpp

free\current.obj : current.cpp ..\include\setpoint.h
    cl $(CPP_PROJ) current.cpp

free\dateclas.obj : dateclas.cpp
    cl $(CPP_PROJ) dateclas.cpp

free\dbsubs.obj : dbsubs.cpp
    cl $(CPP_PROJ) dbsubs.cpp

free\dcurve.obj : dcurve.cpp
    cl $(CPP_PROJ) dcurve.cpp

free\dow.obj : dow.cpp
    cl $(CPP_PROJ) dow.cpp

free\downsubs.obj : downsubs.cpp
    cl $(CPP_PROJ) downsubs.cpp

free\downtime.obj : downtime.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) downtime.cpp

free\dstat.obj : dstat.cpp
    cl $(CPP_PROJ) dstat.cpp

free\dtotchar.obj : dtotchar.cpp
    cl $(CPP_PROJ) dtotchar.cpp

free\dynamic_file_name.obj : dynamic_file_name.cpp ..\include\stringcl.h ..\include\computer.h ..\include\timeclas.h
    cl $(CPP_PROJ) dynamic_file_name.cpp

free\dynamic_output_wire_description.obj : dynamic_output_wire_description.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) dynamic_output_wire_description.cpp

free\error_ms.obj : error_ms.cpp
    cl $(CPP_PROJ) error_ms.cpp

free\evclass.obj : evclass.cpp ..\include\evclass.h
    cl $(CPP_PROJ) evclass.cpp

free\execwait.obj : execwait.cpp
    cl $(CPP_PROJ) execwait.cpp

free\exists.obj : exists.cpp
    cl $(CPP_PROJ) exists.cpp

free\extract.obj : extract.cpp
    cl $(CPP_PROJ) extract.cpp

free\fifo.obj : fifo.cpp ..\include\fifo.h
    cl $(CPP_PROJ) fifo.cpp

free\fileclas.obj : fileclas.cpp ..\include\fileclas.h
    cl $(CPP_PROJ) fileclas.cpp

free\fillpart.obj : fillpart.cpp
    cl $(CPP_PROJ) fillpart.cpp

free\fillshot.obj : fillshot.cpp
    cl $(CPP_PROJ) fillshot.cpp

free\fix.obj : fix.cpp
    cl $(CPP_PROJ) fix.cpp

free\fontclas.obj : fontclas.cpp ..\include\fontclas.h
    cl $(CPP_PROJ) fontclas.cpp

free\ftanalog.obj : ftanalog.cpp ..\include\ftanalog.h
    cl $(CPP_PROJ) ftanalog.cpp

free\ftcalc.obj : ftcalc.cpp ..\include\ftcalc.h
    cl $(CPP_PROJ) ftcalc.cpp

free\ftii_file.obj : ftii_file.cpp ..\include\ftii_file.h
    cl $(CPP_PROJ) ftii_file.cpp

free\ftii_parameters.obj : ftii_parameters.cpp ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h ..\include\ftanalog.h
    cl $(CPP_PROJ) ftii_parameters.cpp

free\ftii_shot_data.obj : ftii_shot_data.cpp ..\include\ftii.h ..\include\ftii_curve_types.h ..\include\ftii_shot_data.h
    cl $(CPP_PROJ) ftii_shot_data.cpp

free\ftiiclas.obj : ftiiclas.cpp ..\include\ftii.h ..\include\ftii_file.h
    cl $(CPP_PROJ) ftiiclas.cpp

free\ftime.obj : ftime.cpp
    cl $(CPP_PROJ) ftime.cpp

free\ftoasc.obj : ftoasc.cpp
    cl $(CPP_PROJ) ftoasc.cpp

free\genlist.obj : genlist.cpp
    cl $(CPP_PROJ) genlist.cpp

free\get_ini.obj : get_ini.cpp
    cl $(CPP_PROJ) get_ini.cpp

free\get_tc.obj : get_tc.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) get_tc.cpp

free\gexedir.obj : gexedir.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) gexedir.cpp

free\gpvar.obj : gpvar.cpp ..\include\fileclas.h
    cl $(CPP_PROJ) gpvar.cpp

free\hex.obj : hex.cpp
    cl $(CPP_PROJ) hex.cpp

free\hourglas.obj : hourglas.cpp
    cl $(CPP_PROJ) hourglas.cpp

free\iniclass.obj : iniclass.cpp ..\include\iniclass.h
    cl $(CPP_PROJ) iniclass.cpp

free\insalph.obj : insalph.cpp
    cl $(CPP_PROJ) insalph.cpp

free\is_empty.obj : is_empty.cpp
    cl $(CPP_PROJ) is_empty.cpp

free\itoasc.obj : itoasc.cpp
    cl $(CPP_PROJ) itoasc.cpp

free\killdir.obj : killdir.cpp
    cl $(CPP_PROJ) killdir.cpp

free\listbox.obj : listbox.cpp
    cl $(CPP_PROJ) listbox.cpp

free\limit_switch_class.obj : limit_switch_class.cpp  ..\include\limit_switch_class.h
    cl $(CPP_PROJ) limit_switch_class.cpp

free\log_file_string.obj : log_file_string.cpp
    cl $(CPP_PROJ) log_file_string.cpp

free\machcomp.obj : machcomp.cpp ..\include\computer.h
    cl $(CPP_PROJ) machcomp.cpp

free\machine.obj : machine.cpp ..\include\setpoint.h
    cl $(CPP_PROJ) machine.cpp

free\machine_to_hmi.obj : machine_to_hmi.cpp
    cl $(CPP_PROJ) machine_to_hmi.cpp

free\machname.obj : machname.cpp ..\include\machname.h
    cl $(CPP_PROJ) machname.cpp

free\marks.obj : marks.cpp
    cl $(CPP_PROJ) marks.cpp

free\maximize.obj : maximize.cpp
    cl $(CPP_PROJ) maximize.cpp

free\mem.obj : mem.cpp
    cl $(CPP_PROJ) mem.cpp

free\menu.obj : menu.cpp
    cl $(CPP_PROJ) menu.cpp

free\menuctrl.obj : menuctrl.cpp  ..\include\menuctrl.h
    cl $(CPP_PROJ) menuctrl.cpp

free\mexists.obj : mexists.cpp
    cl $(CPP_PROJ) mexists.cpp

free\misc.obj : misc.cpp
    cl $(CPP_PROJ) misc.cpp

free\monrun.obj : monrun.cpp
    cl $(CPP_PROJ) monrun.cpp

free\msgclas.obj : msgclas.cpp ..\include\msgclas.h
    cl $(CPP_PROJ) msgclas.cpp

free\nameclas.obj : nameclas.cpp ..\include\computer.h ..\include\stringcl.h ..\include\nameclas.h
    cl $(CPP_PROJ) nameclas.cpp

free\optocontrol.obj : optocontrol.cpp
    cl $(CPP_PROJ) optocontrol.cpp

free\output_label_class.obj : output_label_class.cpp ..\include\nameclas.h ..\include\output_label_class.h ..\include\stringcl.h
    cl $(CPP_PROJ) output_label_class.cpp

free\param.obj : param.cpp
    cl $(CPP_PROJ) param.cpp

free\param_index_class.obj : param_index_class.cpp ..\include\param_index_class.h
    cl $(CPP_PROJ) param_index_class.cpp

free\part.obj : part.cpp ..\include\part.h ..\include\stringcl.h ..\include\nameclas.h ..\include\setpoint.h
    cl $(CPP_PROJ) part.cpp

free\partlist.obj : partlist.cpp
    cl $(CPP_PROJ) partlist.cpp

free\path.obj : path.cpp
    cl $(CPP_PROJ) path.cpp

free\pexists.obj : pexists.cpp
    cl $(CPP_PROJ) pexists.cpp

free\plookup.obj : plookup.cpp
    cl $(CPP_PROJ) plookup.cpp

free\plotclas.obj : plotclas.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) plotclas.cpp

free\plotsubs.obj : plotsubs.cpp
    cl $(CPP_PROJ) plotsubs.cpp

free\pointcl.obj : pointcl.cpp ..\include\pointcl.h
    cl $(CPP_PROJ) pointcl.cpp

free\popctrl.obj : popctrl.cpp ..\include\popctrl.h
    cl $(CPP_PROJ) popctrl.cpp

free\postsec.obj : postsec.cpp
    cl $(CPP_PROJ) postsec.cpp

free\profiles.obj : profiles.cpp ..\include\stringcl.h ..\include\nameclas.h
    cl $(CPP_PROJ) profiles.cpp

free\pw.obj : pw.cpp ..\include\computer.h
    cl $(CPP_PROJ) pw.cpp

free\readline.obj : readline.cpp
    cl $(CPP_PROJ) readline.cpp

free\rectclas.obj : rectclas.cpp ..\include\rectclas.h
    cl $(CPP_PROJ) rectclas.cpp

free\reverse.obj : reverse.cpp
    cl $(CPP_PROJ) reverse.cpp

free\ringcl.obj : ringcl.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) ringcl.cpp

free\rjust.obj : rjust.cpp
    cl $(CPP_PROJ) rjust.cpp

free\runlist.obj : runlist.cpp ..\include\runlist.h
    cl $(CPP_PROJ) runlist.cpp

free\semaphor.obj : semaphor.cpp
    cl $(CPP_PROJ) semaphor.cpp

free\setpoint.obj : setpoint.CPP ..\include\setpoint.h ..\include\stringcl.h
    cl $(CPP_PROJ) setpoint.CPP

free\shift.obj : shift.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) shift.cpp

free\shotclas.obj : shotclas.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) shotclas.cpp

free\sockcli.obj : sockcli.cpp ..\include\sockcli.h
    cl $(CPP_PROJ) sockcli.cpp

free\startpar.obj : startpar.cpp
    cl $(CPP_PROJ) startpar.cpp

free\stddown.obj : stddown.cpp ..\include\stddown.h
    cl $(CPP_PROJ) stddown.cpp

free\stparam.obj : stparam.cpp ..\include\fileclas.h
    cl $(CPP_PROJ) stparam.cpp

free\stpres.obj : stpres.cpp ..\include\stringcl.h ..\include\fileclas.h
    cl $(CPP_PROJ) stpres.cpp

free\strarray.obj : strarray.cpp ..\include\strarray.h
    cl $(CPP_PROJ) strarray.cpp

free\stringcl.obj : stringcl.cpp ..\include\stringcl.h
    cl $(CPP_PROJ) stringcl.cpp

free\stringtable.obj : stringtable.cpp ..\include\stringtable.h
    cl $(CPP_PROJ) stringtable.cpp

free\structs.obj : structs.cpp
    cl $(CPP_PROJ) STRUCTS.CPP

free\stsetup.obj : stsetup.cpp ..\include\stringcl.h ..\include\fileclas.h
    cl $(CPP_PROJ) stsetup.cpp

free\textlen.obj : textlen.CPP
    cl $(CPP_PROJ) textlen.CPP

free\textlist.obj : textlist.CPP
    cl $(CPP_PROJ) textlist.CPP

free\temp_message.obj : temp_message.cpp ..\include\wclass.h
    cl $(CPP_PROJ) temp_message.cpp

free\time.obj : time.cpp
    cl $(CPP_PROJ) time.cpp

free\timeclas.obj : timeclas.cpp ..\include\timeclas.h
    cl $(CPP_PROJ) timeclas.cpp

free\unitadj.obj : unitadj.cpp
    cl $(CPP_PROJ) unitadj.cpp

free\units.obj : units.cpp ..\include\units.h
    cl $(CPP_PROJ) units.cpp

free\updown.obj : updown.cpp
    cl $(CPP_PROJ) updown.cpp

free\v5tov7.obj : v5tov7.cpp
    cl $(CPP_PROJ) v5tov7.cpp

free\visigrid.obj : visigrid.cpp ..\include\visigrid.h
    cl $(CPP_PROJ) visigrid.cpp

free\visiplot.obj : visiplot.cpp ..\include\visiplot.h ..\include\stringcl.h ..\include\wclass.h
    cl $(CPP_PROJ) visiplot.cpp

free\warmupcl.obj : warmupcl.cpp ..\include\warmupcl.h ..\include\iniclass.h
    cl $(CPP_PROJ) warmupcl.cpp

free\wclass.obj : wclass.cpp ..\include\wclass.h
    cl $(CPP_PROJ) wclass.cpp

free\winsubs.obj : winsubs.cpp
    cl $(CPP_PROJ) winsubs.cpp

free\workclas.obj : workclas.CPP
    cl $(CPP_PROJ) workclas.CPP

free\writelin.obj : writelin.cpp
    cl $(CPP_PROJ) writelin.cpp

free\zero.obj : zero.cpp
    cl $(CPP_PROJ) zero.cpp

