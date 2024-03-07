del c:\data\sorcback\backups\*.zip

cd include
pkzip25 -add c:\data\sorcback\backups\include *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd subs
pkzip25 -add c:\data\sorcback\backups\subs *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd dundas
pkzip25 -add c:\data\sorcback\backups\dundas *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd bupres
pkzip25 -add c:\data\sorcback\backups\bupres *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd copylog
pkzip25 -add c:\data\sorcback\backups\copylog *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd downtime
pkzip25 -add c:\data\sorcback\backups\downtime *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd dsbackup
pkzip25 -add c:\data\sorcback\backups\dsbackup *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd dsinstall
pkzip25 -add c:\data\sorcback\backups\dsinstall *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd dsrestor
pkzip25 -add c:\data\sorcback\backups\dsrestor *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd editdown
pkzip25 -add c:\data\sorcback\backups\editdown *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd editpart
pkzip25 -add c:\data\sorcback\backups\editpart *.cpp *.rc *.h *.bat *.mak *.dsp *.dsw
cd..

cd emachine
pkzip25 -add c:\data\sorcback\backups\emachine *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd esensor
pkzip25 -add c:\data\sorcback\backups\esensor *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd EventMan
pkzip25 -add c:\data\sorcback\backups\EventMan *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd fastrak
pkzip25 -add c:\data\sorcback\backups\fastrak *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd ftclasnt
pkzip25 -add c:\data\sorcback\backups\ftclasnt *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd Install
pkzip25 -add c:\data\sorcback\backups\Install *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd monedit
pkzip25 -add c:\data\sorcback\backups\monedit *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd netinstall
pkzip25 -add c:\data\sorcback\backups\netinstall *.cpp *.rc *.h *.bat *.mak *.dsp *.dsw
cd..

cd netsetup
pkzip25 -add c:\data\sorcback\backups\netsetup *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd help
pkzip25 -add -excl=*.chm c:\data\sorcback\backups\netsetuphelp *.*
cd..
cd..

cd profile
pkzip25 -add c:\data\sorcback\backups\profile *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd shiftrpt
pkzip25 -add c:\data\sorcback\backups\shiftrpt *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd startup
pkzip25 -add c:\data\sorcback\backups\startup *.cpp *.h *.rc *.mak *.inl *.ico *.dsp *.dsw
cd..

cd v5setups
pkzip25 -add c:\data\sorcback\backups\v5setups *.cpp *.h *.rc *.mak *.ico *.bat *.dsp *.dsw
cd..

cd test
pkzip25 -add c:\data\sorcback\backups\test *.cpp *.h *.rc *.mak *.inl *.ico *.bat *.dsp *.dsw
cd..

cd visiedit
pkzip25 -add c:\data\sorcback\backups\visiedit *.cpp *.h *.rc *.mak *.ico *.bat *.dsp *.dsw
cd..

cd vtextrac
pkzip25 -add c:\data\sorcback\backups\vtextrac *.cpp *.h *.rc *.mak *.ico *.bat *.dsp *.dsw
cd help
pkzip25 -add -excl=*.chm c:\data\sorcback\backups\vtextrachelp *.*
cd..
cd..

cd ft2
pkzip25 -add c:\data\sorcback\backups\ft2 *.cpp *.rc *.h *.bat *.mak *.ico *.dsp *.dsw
cd..

cd t2
pkzip25 -add c:\data\sorcback\backups\t2 *.cpp *.rc *.h *.bat *.mak *.ico *.dsp *.dsw
cd..

copy ball.bat c:\data\sorcback\backups\
copy makeall.bat c:\data\sorcback\backups\
copy mscvars.bat c:\data\sorcback\backups\
copy buc.bat c:\data\sorcback\backups\