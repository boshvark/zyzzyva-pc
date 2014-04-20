#---------------------------------------------------------------------------
# zyzzyva.pri
#
# Common configuration include file for Zyzzyva using qmake.
#
# Copyright 2006-2012 Boshvark Software, LLC.
#
# This file is part of Zyzzyva.
#
# Zyzzyva is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Zyzzyva is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#---------------------------------------------------------------------------

CONFIG -= debug
CONFIG -= release
contains(BUILD, debug) {
    CONFIG += debug
}
else {
    CONFIG += release
}

CONFIG = $$unique(CONFIG)

unix {
    DEFINES += Z_UNIX
}
linux-g++|linux-g++-64 {
    DEFINES += Z_LINUX
}
win32 {
    DEFINES += Z_W32
}
macx {
    DEFINES += Z_OSX
}

#BUILD += nssc
contains(BUILD, nssc) {
    DEFINES += Z_NSSC
}
