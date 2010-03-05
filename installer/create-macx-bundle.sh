#!/bin/bash
#-----------------------------------------------------------------------------
# create-macx-bundle.sh
#
# Create a bundle for Mac OS X.
#
# Copyright 2008, 2009, 2010 Michael W Thelen <mthelen@gmail.com>.
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

QTDIR=/usr/local/Trolltech/Qt-4.6.2
QTVER=4
OUTDIR=$(pwd)/installer/macosx
APPDIR=$OUTDIR/Zyzzyva.app

if [ ! -e $APPDIR/Contents/Frameworks ]; then
    COPYQT="yes"
fi

echo "Running QMake..."
$QTDIR/bin/qmake
echo "Building Zyzzyva..."
make -s

mkdir -p $APPDIR/Contents/Frameworks

# Copy executable into bundle
echo "Copying Zyzzyva executable into bundle..."
cp -r bin/Zyzzyva.app/Contents/* $APPDIR/Contents

# Change link location for libzyzzyva in executable
echo "Changing link location for libzyzzyva in Zyzzyva executable..."
install_name_tool -change \
    libzyzzyva.2.dylib \
    @executable_path/../Frameworks/libzyzzyva.2.dylib \
    $APPDIR/Contents/MacOS/Zyzzyva

# Copy zyzzyva.top unless it's already there
if [ ! -e $APPDIR/Contents/MacOS/zyzzyva.top ]; then
    echo "Copying zyzzyva.top into bundle..."
    cp zyzzyva.top $APPDIR/Contents/MacOS
fi

# Copy qt.conf unless it's already there
if [ ! -e $APPDIR/Contents/Resources/qt.conf ]; then
    echo "Copying qt.conf into bundle..."
    cp conf/macosx/qt.conf $APPDIR/Contents/Resources
fi

# Copy data directory into bundle unless it's already there
if [ ! -e $APPDIR/Contents/MacOS/data ]; then
    echo "Copying data directory into bundle..."
    cp -r data $APPDIR/Contents/MacOS
    find $OUTDIR -type d -name '.svn' -print0 | xargs -0 rm -rf
fi

# Copy Zyzzyva libs to Frameworks directory
echo "Copying libzyzzyva into bundle..."
cp bin/libzyzzyva.2.dylib $APPDIR/Contents/Frameworks

# Copy Qt libs to Frameworks directory unless they're already there
if [ "$COPYQT" = "yes" ]; then

    # Copy Assistant client into bundle
    echo "Copying Assistant client into bundle..."
    cp -r $QTDIR/bin/assistant.app $APPDIR/Contents/MacOS

    # Copy Qt frameworks into bundle and tell the executable to link to them
    for i in QtCore QtGui QtNetwork QtSql QtXml QtAssistant ; do

        # Copy Qt framework into bundle
        echo "Copying $i.framework into bundle..."
        pushd $QTDIR/lib > /dev/null
        tar -cf - ./$i.framework | (cd $APPDIR/Contents/Frameworks && tar -xf -)
        popd > /dev/null

        # Delete headers and debug libraries
        echo "Deleting $i.framework headers and debug libraries..."
        rm $APPDIR/Contents/Frameworks/$i.framework/Headers
        rm -rf $APPDIR/Contents/Frameworks/$i.framework/Versions/$QTVER/Headers
        find $APPDIR/Contents/Frameworks -name arch -or -name '*debug*' \
            -print0 | xargs -0 rm -rf

        # Set identification names for the private framework
        echo "Setting identification name for $i.framework..."
        install_name_tool \
            -id @executable_path/../Frameworks/$i.framework/Versions/$QTVER/$i \
            $APPDIR/Contents/Frameworks/$i.framework/Versions/$QTVER/$i

        #  Change reference to QtCore in frameworks
        if [ "$i" != "QtCore" ]; then
            echo "Changing link location for $i.framework in QtGui.framework..."
            install_name_tool -change \
                $QTDIR/lib/QtCore.framework/Versions/$QTVER/QtCore \
                @executable_path/../Frameworks/QtCore.framework/Versions/$QTVER/QtCore \
                $APPDIR/Contents/Frameworks/$i.framework/Versions/$QTVER/$i
        fi

    done

    mkdir -p $APPDIR/Contents/PlugIns

    # Copy plugins into bundle and update references
    #for i in imageformats/libqgif.dylib sqldrivers/libqsqlite.dylib ; do
    for i in imageformats/libqgif.dylib ; do

        # Copy plugin into bundle
        echo "Copying $i into bundle..."

        PLUGPATH=${i%/*}
        mkdir -p $APPDIR/Contents/PlugIns/$PLUGPATH
        cp $QTDIR/plugins/$i $APPDIR/Contents/PlugIns/$PLUGPATH

        #  Change reference to QtCore in plugin
        echo "Changing link location for QtCore.framework in $i plugin..."
        install_name_tool -change \
            $QTDIR/lib/QtCore.framework/Versions/$QTVER/QtCore \
            @executable_path/../Frameworks/QtCore.framework/Versions/$QTVER/QtCore \
            $APPDIR/Contents/PlugIns/$i

        #  Change reference to QtSql in plugin
        echo "Changing link location for QtSql.framework in $i plugin..."
        install_name_tool -change \
            $QTDIR/lib/QtSql.framework/Versions/$QTVER/QtSql \
            @executable_path/../Frameworks/QtSql.framework/Versions/$QTVER/QtSql \
            $APPDIR/Contents/PlugIns/$i

        #  Change reference to QtGui in plugin
        echo "Changing link location for QtGui in $i plugin..."
        install_name_tool -change \
            $QTDIR/lib/QtGui.framework/Versions/$QTVER/QtGui \
            @executable_path/../Frameworks/QtGui.framework/Versions/$QTVER/QtGui \
            $APPDIR/Contents/PlugIns/$i

    done

    # Change reference to frameworks in Assistant client
    for i in QtCore QtGui QtNetwork QtAssistant ; do

        echo "Changing link location for $i.framework in QtAssistant.framework..."
        install_name_tool -change \
            $QTDIR/lib/$i.framework/Versions/$QTVER/$i \
            @executable_path/../Frameworks/$i.framework/Versions/$QTVER/$i \
            $APPDIR/Contents/Frameworks/QtAssistant.framework/Versions/$QTVER/QtAssistant

    done

fi

# Update links to Qt frameworks in libzyzzyva and executable
for i in QtCore QtGui QtNetwork QtSql QtXml QtAssistant ; do

    # Change reference to framework in libzyzzyva
    echo "Changing link location for $i.framework in libzyzzyva..."
    install_name_tool -change \
        $QTDIR/lib/$i.framework/Versions/$QTVER/$i \
        @executable_path/../Frameworks/$i.framework/Versions/$QTVER/$i \
        $APPDIR/Contents/Frameworks/libzyzzyva.2.dylib

    # Change link locations for framework in executable
    echo "Changing link location for $i.framework in Zyzzyva executable..."
    install_name_tool -change \
        $QTDIR/lib/$i.framework/Versions/$QTVER/$i \
        @executable_path/../Frameworks/$i.framework/Versions/$QTVER/$i \
        $APPDIR/Contents/MacOS/Zyzzyva

done

echo "Done.  Bundle is called $APPDIR."
