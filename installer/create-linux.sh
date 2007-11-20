#!/bin/bash
#-----------------------------------------------------------------------------
# create-linux.sh
#
# Create a Linux directory for the installer to use.
#
# Copyright 2006, 2007 Michael W Thelen <mthelen@gmail.com>.
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
#-----------------------------------------------------------------------------

QTVER=4.3.1
QTDIR=/usr/local/Trolltech/Qt-$QTVER
OUTDIR=installer/linux

$QTDIR/bin/qmake
make

mkdir -p $OUTDIR/bin

cp /usr/lib/gcc/i686-pc-linux-gnu/4.1.1/libstdc++.so.6 $OUTDIR/bin
cp /usr/lib/gcc/i686-pc-linux-gnu/4.1.1/libgcc_s.so.1 $OUTDIR/bin
cp /usr/lib/libmysqlclient.so.14 $OUTDIR/bin
cp -d $QTDIR/lib/libQt{Core,Gui,Network,Sql,Xml}.so* $OUTDIR/bin
cp -d $QTDIR/bin/assistant $OUTDIR/bin
cp -d bin/libzyzzyva.so* $OUTDIR/bin
cp -d bin/zyzzyva $OUTDIR/bin
cp -d installer/zyzzyva.sh $OUTDIR/zyzzyva

cp zyzzyva.top $OUTDIR
cp AUTHORS $OUTDIR/AUTHORS
cp CHANGES $OUTDIR/CHANGES
cp COPYING $OUTDIR/COPYING
cp LICENSE $OUTDIR/LICENSE
cp README $OUTDIR/README

mkdir -p $OUTDIR/images
cp images/zyzzyva*x*.{png,ico,bmp} $OUTDIR/images

cp -r data $OUTDIR
find $OUTDIR/data -type d -name '.svn' -print0 | xargs -0 rm -rf

#perl -pi -e "\$f = '$OUTDIR';s/\@\@EXECUTABLE\@\@/zyzzyva/g; s/\@\@INSTALLDIR\@\@/\$f/g;" $OUTDIR/zyzzyva.sh
