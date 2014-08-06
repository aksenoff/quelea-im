SINGLEAPPLICATION_LIBCONFIG = dll

CONFIG		+= debug_and_release
CONFIG		*= qt thread warn_on
CONFIG		-= exceptions rtti

#DESTDIR		= $$PWD/bin

LIBS		+= -L$$PWD/bin


# Configuration for MacOS X
macx {
	# Using gcc
	QMAKESPEC = macx-g++
	# uncoment the following line if you want to use xcode
	# QMAKESPEC = macx-xcode
}

*-g++ {
	QMAKE_CFLAGS_DEBUG	= -O0 -g
	QMAKE_CXXFLAGS_DEBUG	= -O0 -g
	QMAKE_CFLAGS_RELEASE	= -O3 -fomit-frame-pointer -pipe
	QMAKE_CXXFLAGS_RELEASE	= -O3 -fomit-frame-pointer -pipe
}
