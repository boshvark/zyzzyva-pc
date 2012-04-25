#!/bin/bash
#-----------------------------------------------------------------------------
# create-linux.sh
#
# Create a Linux directory for the installer to use.
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
#-----------------------------------------------------------------------------

set -e

QTVER=4.3.2
INSTDIR=installer

if [ "$QTDIR" = "" ]; then
    QTDIR=/usr/local/Trolltech/Qt-$QTVER
fi

if [ "$(uname -m)" = "x86_64" ]; then
    PLATFORM=linux-x64
else 
    PLATFORM=linux
fi

OUTDIR=$INSTDIR/$PLATFORM

$QTDIR/bin/qmake
make

mkdir -p $OUTDIR/bin

cp $(find /usr/lib -name libstdc++.so.6 | head -n 1) $OUTDIR/bin
cp $(find /lib -name libgcc_s.so.1 | head -n 1) $OUTDIR/bin
cp -d $QTDIR/lib/libQt{AssistantClient,Core,Gui,Network,Sql,Xml}.so* $OUTDIR/bin
rm $OUTDIR/bin/*.debug
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
cp images/zyzzyva-32x32.png $OUTDIR/images

cp -r data $OUTDIR
find $OUTDIR/data -type d -name '.svn' -print0 | xargs -0 rm -rf

pushd $INSTDIR
tar -czf $PLATFORM.tar.gz $PLATFORM
popd

#perl -pi -e "\$f = '$OUTDIR';s/\@\@EXECUTABLE\@\@/zyzzyva/g; s/\@\@INSTALLDIR\@\@/\$f/g;" $OUTDIR/zyzzyva.sh
