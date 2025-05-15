$Id: README.win32,v 1.46 2003/06/02 18:40:00 gerald Exp $

Installing Ethereal, Tethereal, and Editcap on Win32
====================================================
These are the instructions for installing Ethereal
from the installation executable that is provided on
the Ethereal website and any of its mirrors.

The installation package allows you to install:

	o Ethereal - the GUI version
	o Tethereal - the console, line-mode version
	o Editcap - a console, line-mode utility to convert
	  capture files from one format to another.
	  (The same functions are available in Ethereal)
	o Text2Pcap - a console, line-mode utility to generate 
	  a capture file from an ASCII hexdump of packets
	o Mergecap - a console, line-mode utility to merge two 
	  capture files into one

Additionally, the installation package contains a "plugins"
option, which installs the Gryphon, MGCP and GIOP dissector plugins
for use with Ethereal and Tethereal.

All binaries in Ethereal package are now built with debugging
information embedded. If you are experiencing a crash when running
Ethereal or other binaries, Dr. Watson or your debugger
can use the information embedded in the binary to provide useful 
information to the Ethereal developers that will help them pinpoint 
the problem. 

In the past, two versions of Ethereal binaries were published -- a
version that could capture packets and a version which could not.
The latter is useful if you're only reading files produced by
another product (e.g., a sniffer, firewall, or intrustion detection system)
and did not wish to install WinPcap, the library Ethereal uses
to capture packets on Win32 platforms.

As of WinPcap 2.1, all the WinPcap libraries have been released as DLLs. 
This means that Ethereal can detect the presence of WinPcap at run time,
which means that only one version of the Ethereal binaries needs to be
shipped.

If you don't want to capture packets, just install the Ethereal
package. If you do want to capture packets, install Ethereal *and*
install the latest non-beta version of WinPcap, available from:

	http://winpcap.polito.it/

and mirrored at

	http://winpcap.mirror.ethereal.com/

and

	http://www.mirrors.wiretapped.net/security/packet-capture/winpcap/

If you already have an earlier version of WinPcap installed, you need to
un-install it and install the latest version.  If the older version is
WinPcap 2.0 or 2.02, and you have other applications that use the older
version , you will have to decide which applications to keep, since
WinPcap 2.0/2.02 and later versions cannot be installed on the same
system at the same time.

If Ethereal is not capturing packets and you have WinPcap installed, you
can test your WinPcap installation by installing WinDump (tcpdump for
Windows) ported by the same folks who make WinPcap.  It's at:

	http://windump.polito.it/

and mirrored at

	http://windump.mirror.ethereal.com/

and

	http://www.mirrors.wiretapped.net/security/packet-capture/windump/

They also make Analyzer, a GUI sniffer for Win32:

	http://analyzer.polito.it/

The rest of this documentation is only interesting if
you want to compile Ethereal yourself.


Running Ethereal, Tethereal, and Editcap on Win32
=================================================
You need the glib and gtk libraries for running Ethereal. 

These packages for win32 can be found at:

	http://www.ethereal.com/distribution/win32

and at the home page for the GTK+ for Win32 project:

	http://www.gimp.org/~tml/gimp/win32

or
	http://www.iki.fi/tml/gimp/win32/

(the mirror nearer to you may be faster).

Plugins (gryphon.dll and mgcp.dll) can go in:
	<Ethereal installation directory>\plugins\<version>

Where <version> is the version number, without brackets.  For example,
if you have Ethereal 0.9.8 installed in the default location, plugins
will reside in C:\Program Files\Ethereal\plugins\0.9.8

Yes, the location of plugins needs to be more flexible.

Make sure the glib and gtk DLL's are in your path - i.e., that your path
includes the directory (folder) or directories (folders) in which those
DLLs are found - when you run Ethereal.  This includes gtk-*.dll,
glib-*.dll, gmodule-*.dll, gdk-*.dll, gnu-intl.dll, and iconv-*.dll.
As of the 20000805 GTK+/GLIB distribution, gthread-*.dll is no longer needed.

The Win32 Binary distribution, available from

	http://www.ethereal.com/distribution/win32

used different version of the GTK+/GLIB libraries at different points
in time:

Ethereal Version		GTK+/GLIB version
----------------		-----------------
0.8.16 and after		20001226
0.8.11 - 0.8.15			20000805
0.8.9 - 0.8.10			20000416
0.8.8 and before		19990828


Capturing Packets
-----------------
In order to capture with Win32, you need to install the NDIS
packet capture driver for your particular Win32 OS; drivers for Windows
9x, Windows NT 4.0, and Windows 2000 can be downloaded from the
WinPcap home page:

	http://winpcap.polito.it/

or the mirror site at

	http://www.wiretapped.net/security/packet-capture/winpcap/default.htm

Compiling the Ethereal distribution from source
===============================================

You'll need the development packages for GLIB, GTK+, WinPcap, zlib,
Net-SNMP, and ADNS.  The GLIB, GTK+, and WinPcap packages are available
from the respctive home pages for each project (the same URLs as listed
above). The development packages contain header files and stub libaries
to link against.  Precompiled zlib, Net-SNMP, and ADNS packages are
available at

	http://www.ethereal.com/distribution/win32/

The ADNS package is also available at its homepage:

	http://adns.jgaa.com/


Instructions for MS Visual C++
----------------------------
Modify the config.nmake file in the top directory of the Ethereal source
tree to work for your local configuration; if you don't have Python,
comment out the line that defines PYTHON, otherwise set it to refer to
the pathname of your Python interpreter executable.  You should not have
to modify any other Makefile.

Many of the file and directory names used in the build process go past
the old 8.3 naming limitations.  As a result, at least on Windows NT 4.0,
Windows 2000, Windows XP, and Windows .NET Server, you should use the
newer "cmd.exe" command interpreter instead of the old "command.com",
as the "command.com" on Windows 2000, at least, can't handle non-8.3
directory names.  (It may be that the "command.com" in Windows 95, Windows
98, and Windows Me, as it's the only command interpreter in those systems,
can handle those directories.  If not, it may not be possible to build
Ethereal from the command line on those versions of Windows.)

Be sure that your command-line environment is set up to compile
and link with MSVC++. When installing MSVC++, you can have your
system's environment set up to always allow compiling from the
command line, or you can invoke the vcvars32.bat script, which can
usually be found in the "VC98\Bin" subdirectory of the directory in
which Visual Studio was installed.

The first time you build Ethereal, run the script "cleanbld.bat" in the
top-level Ethereal source directory to make sure that the "config.h"
files will be reconstructed from the "config.h.win32" files.  (If, for
example, you have "config.h" files left over from a Unix build, a
Windows build will fail.)

In the ethereal directory, type "nmake -f makefile.nmake". It will
recurse into the subdirectories as appropriate.

Some generated source is created by traditionally "Unix-ish" tools.

If you are building from an official distribution, these files are
already generated, although they were generated on a Unix-compatible
system.  In most cases, the generated files can be used when building on
Windows, but the files listed below as being generated by Flex can be
used when building on Windows only when generated by a Windows version
of Flex, so you will need a Windows version of Flex to do a Windows
build.  Those generated files are removed by the "cleanbld.bat" script,
to make sure that versions left over from a Unix build aren't used.

If you are building from a modified version of an official distribution,
and you modified any of the source files listed below, you will need the
tool(s) that generate output from those source files.

If building from a CVS image, you'll need all the tools to generate C
source.

The "special" files and their requisite tools are:

Source				Output			Tool
------				------			----
config.h.win32			config.h		sed
epan/config.h.win32		epan/config.h		sed
image/ethereal.rc.in		image/ethereal.rc	sed
image/tethereal.rc.in		image/tethereal.rc	sed
image/editcap.rc.in		image/editcap.rc	sed
image/mergecap.rc.in		image/mergecap.rc	sed
image/text2pcap.rc.in		image/text2pcap.rc	sed
packaging/nsis/ethereal.nsi.in	packaging/ethereal.nsi	sed
wiretap/config.h.win32		wiretap/config.h	sed
epan/dfilter/dfilter-scanner.l  epan/dfilter/*.c	Flex
text2pcap-scanner.l		*.c			Flex
wiretap/ascend-scanner.l	*.c			Flex
wiretap/ascend-grammar.y	*.c,*.h			Bison/Yacc
ncp2222.py			packet-ncp2222.c	Python

make-reg-dotc, packet*.c	register.c		Bash + grep + sed
or
make-reg-dotc.py, packet*.c	register.c		Python

make-tapreg-dotc, tap-*.c	tethereal-tap-register.c
							Bash + grep + sed

The Makefile.nmake supplied with the Ethereal distribution will, if
PYTHON is defined in config.nmake, attempt to make register.c with
Python, since it is much much much faster than the shell version.  The
reason it is faster is because the shell version launches multiple
processes (grep, sed) for each source file, multiple times.  The Python
script is one process.  This matters a lot on Win32.

If you have a Unix system handy, you can first build on Unix to create
most of the source files that these tools make, then run the build on
Windows.  That will avoid the need for these tools on your Windows
computer.  This won't work for the files in the "image" directory,
however, as those aren't built on Unix - they're only for Windows
builds.  It also won't work for the "config.h" files; whilst those are
built for Unix, they're specific to the platform on which you're
building, and the "config.h" files constructed for a Unix build will not
work with a Windows build.  In addition, it won't work for the files
generated by Flex, as, for a Windows build, those have to be generated
by a Windows version of Flex.

Most of those tools are available for Win32 systems as part of the
Cygwin package:

	http://sources.redhat.com/cygwin/

After installing them, you will probably have to modify the config.nmake
file to specify where the Cygwin binaries are installed.

Python for Win32 is available from

	http://www.python.org/


Instructions for Cygwin
-----------------------

It is possible to build Ethereal under Cygwin using their version
of XFree86. References:
 - http://www.ethereal.com/lists/ethereal-dev/200205/msg00107.html
 - http://www.ethereal.com/lists/ethereal-dev/200302/msg00026.html
 
To get it running, execute the following steps:

1. Install the required cygwin packages (compiler, scripting, X, zlib)
   with the CygWin setup.exe tool (http://www.cygwin.com/).
   You need the base Xfree86 support plus the X headers package in order
   to be able to compile the gtk+ package.

2. Download glib-1.2.10 and gtk+-1.2.10 from a mirror of www.gnome.org.

3. Retrieve the patches for glib-1.2.10 and gtk+-1.2.10 from
   http://homepage.ntlworld.com/steven.obrien2/

 + glib-1.2.10
   http://homepage.ntlworld.com/steven.obrien2/ (URL cont'd on next line)
          /libs/patches/glib-1.2.10-cygwin.patch

 + gtk+-1.2.10
   http://homepage.ntlworld.com/steven.obrien2/ (URL cont'd on next line)
          /libs/patches/gtk+-1.2.10-cygwin.patch

4. Compile and install both packages after patching (see instructions
   at the bottom of http://homepage.ntlworld.com/steven.obrien2/):

   Set the path:

     $ PATH=/opt/gnome/bin:/usr/X11R6/bin:$PATH

   For glib-1.2.10:
   
     $ cd glib-1.2.10
     $ patch -p1 < /path/to/glib-1.2.10-cygwin.patch
     $ CFLAGS=-O2 ./configure --prefix=/opt/gnome --with-threads=posix
     $ make
     $ make check
     $ make install

   For gtk+-1.2.10:

     $ cd gtk+-1.2.10
     $ patch -p1 < /path/to/gtk+-1.2.10-cygwin.patch
     $ CFLAGS=-O2 ./configure --prefix=/opt/gnome
     $ make
     $ make check
     $ make install

5. Patch Makefile.am in <ethereal-src>/gtk/Makefile.am by
   removing "gtkclist.c" from the dependencies.

   This patch is required since the private GTK+ clist widget
   (was required for earlier versions of GTK+ but prevents Ethereal
   from running with cygwin).

6. Configure and make Ethereal:

   Set the path (if this has not yet been done earlier)

     $ PATH=/opt/gnome/bin:$PATH

     $ ./autogen.sh --without-pcap --without-plugins
     $ ./configure --without-pcap --without-plugins
     $ make

   This make will eventually stop, but it is required as e.g., the
   GTK binaries are built then.

     $ make ethereal.exe

7. Start X

     $ sh /usr/X11R6/bin/startxwin.sh

   For non-US keyboard layouts, use (replace 'be' with your layout):

     $ setxkbmap.exe -layout be

8. Run ethereal (add /opt/gnome/bin to $PATH if this is not yet done)

     $ <ethereal-src>/ethereal

   And voila! Behold the mighty sniffer in all its glory!

Something is wrong with the makefile that gets generated, so it doesn't work
just running make.
I am not curious enough to look at why 'make' doesnt work; 'make ethereal.exe'
works well enough for me.

Note: Compiling Ethereal under cygwin takes a lot of time, because the
generation of 'register.c' takes ages. If you only edit one dissector and
you know what you're doing, it is acceptable to uncomment the generation
of the file 'register.c' in Makefile. Look for the 'register.c' target:

    register.c: $(DISSECTOR_SRC) $(srcdir)/make-reg-dotc
        @echo Making register.c
        # @$(srcdir)/make-reg-dotc register.c $(srcdir) $(DISSECTOR_SRC)
	@echo Skipping generation of register.c

Of course, you need to generate the 'register.c' file at least once.
