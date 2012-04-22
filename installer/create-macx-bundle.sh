#!/bin/bash
#-----------------------------------------------------------------------------
# create-macx-bundle.sh
#
# Create a bundle for Mac OS X.
#
# Copyright 2008-2012 Boshvark Software, LLC.
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

QTDIR=/usr/local/Trolltech/Qt-4.3.2
QTVER=4
OUTDIR=$(pwd)/installer/macosx
ZVER=$(cat $(pwd)/VERSION)
APPDIR=$OUTDIR/Zyzzyva-$ZVER.app

if [ ! -e $APPDIR/Contents/Frameworks ]; then
    COPYQT="yes"
fi

echo "Running QMake..."
$QTDIR/bin/qmake
echo "Building Zyzzyva..."
make

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

# Copy system libs to Frameworks directory
echo "Copying system libs into bundle..."
cp /usr/lib/libstdc++.6.dylib $APPDIR/Contents/Frameworks
cp /usr/lib/libgcc_s.1.dylib $APPDIR/Contents/Frameworks
cp /usr/lib/libz.1.dylib $APPDIR/Contents/Frameworks
cp /opt/local/lib/libdbus-1.3.dylib $APPDIR/Contents/Frameworks
chmod 755 $APPDIR/Contents/Frameworks/*.dylib

# Change link location for libstdc++ in libzyzzyva
echo "Changing link location for libstdc++ in libzyzzyva..."
install_name_tool -change \
    /usr/lib/libstdc++.6.dylib \
    @executable_path/../Frameworks/libstdc++.6.dylib \
    $APPDIR/Contents/Frameworks/libzyzzyva.2.dylib

# Change id of libstdc++
echo "Changing id of libstdc++..."
install_name_tool -id \
    libstdc++.6.dylib \
    $APPDIR/Contents/Frameworks/libstdc++.6.dylib

# Change id of libgcc_s
echo "Changing id of libgcc_s..."
install_name_tool -id \
    libgcc_s.1.dylib \
    $APPDIR/Contents/Frameworks/libgcc_s.1.dylib

# Change id of libdbus
echo "Changing id of libdbus..."
install_name_tool -id \
    libdbus-1.3.dylib \
    $APPDIR/Contents/Frameworks/libdbus-1.3.dylib

# Change id of libz
echo "Changing id of libz..."
install_name_tool -id \
    libz.1.dylib \
    $APPDIR/Contents/Frameworks/libz.1.dylib

# Change link location for libgcc_s in libzyzzyva
echo "Changing link location for libgcc_s in libzyzzyva..."
install_name_tool -change \
    /usr/lib/libgcc_s.1.dylib \
    @executable_path/../Frameworks/libgcc_s.1.dylib \
    $APPDIR/Contents/Frameworks/libzyzzyva.2.dylib

# Copy Qt libs to Frameworks directory unless they're already there
if [ "$COPYQT" = "yes" ]; then

    # Copy Assistant client into bundle
    echo "Copying Assistant client into bundle..."
    ASSISTANT_DIR=$OUTDIR/assistant.app
    cp -r $QTDIR/bin/assistant.app $OUTDIR
    mkdir -p $ASSISTANT_DIR/Contents/Frameworks

    # Create soft links to libraries in assistant
    pushd $ASSISTANT_DIR/Contents/Frameworks
    for i in QtCore QtGui QtNetwork QtSql QtXml QtAssistant QtDBus ; do
        ln -s ../../../Zyzzyva-$ZVER.app/Contents/Frameworks/$i.framework .
    done
    for i in libstdc++.6.dylib libgcc_s.1.dylib libz.1.dylib \
        libdbus-1.3.dylib ; do
        ln -s ../../../Zyzzyva-$ZVER.app/Contents/Frameworks/$i .
    done
    popd

    # Change link location for libstdc++ in assistant
    echo "Changing link location for libstdc++ in assistant..."
    install_name_tool -change \
        /usr/lib/libstdc++.6.dylib \
        @executable_path/../Frameworks/libstdc++.6.dylib \
        $ASSISTANT_DIR/Contents/MacOS/assistant

    # Change link location for libgcc_s in assistant
    echo "Changing link location for libgcc_s in assistant..."
    install_name_tool -change \
        /usr/lib/libgcc_s.1.dylib \
        @executable_path/../Frameworks/libgcc_s.1.dylib \
        $ASSISTANT_DIR/Contents/MacOS/assistant

    # Change link location for libz in assistant
    echo "Changing link location for libz in assistant..."
    install_name_tool -change \
        /usr/lib/libz.1.dylib \
        @executable_path/../Frameworks/libz.1.dylib \
        $ASSISTANT_DIR/Contents/MacOS/assistant

    # Change link location for libz in assistant (not a duplicate)
    echo "Changing link location for libz in assistant..."
    install_name_tool -change \
        /opt/local/lib/libz.1.dylib \
        @executable_path/../Frameworks/libz.1.dylib \
        $ASSISTANT_DIR/Contents/MacOS/assistant

    # Change link location for libdbus in assistant
    echo "Changing link location for libdbus in assistant..."
    install_name_tool -change \
        /opt/local/lib/libdbus-1.3.dylib \
        @executable_path/../Frameworks/libdbus-1.3.dylib \
        $ASSISTANT_DIR/Contents/MacOS/assistant

    # Copy Qt frameworks into bundle and tell the executable to link to them
    for i in QtCore QtGui QtNetwork QtSql QtXml QtAssistant QtDBus ; do

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
        install_name_tool -id \
            @executable_path/../Frameworks/$i.framework/Versions/$QTVER/$i \
            $APPDIR/Contents/Frameworks/$i.framework/Versions/$QTVER/$i

        # Change reference to QtCore in frameworks
        if [ "$i" != "QtCore" ]; then
            echo "Changing link location for QtCore.framework in $i.framework..."
            install_name_tool -change \
                $QTDIR/lib/QtCore.framework/Versions/$QTVER/QtCore \
                @executable_path/../Frameworks/QtCore.framework/Versions/$QTVER/QtCore \
                $APPDIR/Contents/Frameworks/$i.framework/Versions/$QTVER/$i
        fi

        # Change reference to QtCore in frameworks
        if [ "$i" != "QtXml" ]; then
            echo "Changing link location for QtXml.framework in $i.framework..."
            install_name_tool -change \
                $QTDIR/lib/QtXml.framework/Versions/$QTVER/QtXml \
                @executable_path/../Frameworks/QtXml.framework/Versions/$QTVER/QtXml \
                $APPDIR/Contents/Frameworks/$i.framework/Versions/$QTVER/$i
        fi

        # Change reference to system libs in frameworks
        echo "Changing link location for libstdc++ in $i.framework..."
        install_name_tool -change \
            /usr/lib/libstdc++.6.dylib \
            @executable_path/../Frameworks/libstdc++.6.dylib \
            $APPDIR/Contents/Frameworks/$i.framework/Versions/$QTVER/$i

        echo "Changing link location for libgcc_s in $i.framework..."
        install_name_tool -change \
            /usr/lib/libgcc_s.1.dylib \
            @executable_path/../Frameworks/libgcc_s.1.dylib \
            $APPDIR/Contents/Frameworks/$i.framework/Versions/$QTVER/$i

        echo "Changing link location for libz in $i.framework..."
        install_name_tool -change \
            /usr/lib/libz.1.dylib \
            @executable_path/../Frameworks/libz.1.dylib \
            $APPDIR/Contents/Frameworks/$i.framework/Versions/$QTVER/$i

        echo "Changing link location for libz in $i.framework..."
        install_name_tool -change \
            /opt/local/lib/libz.1.dylib \
            @executable_path/../Frameworks/libz.1.dylib \
            $APPDIR/Contents/Frameworks/$i.framework/Versions/$QTVER/$i

        echo "Changing link location for libdbus in $i.framework..."
        install_name_tool -change \
            /opt/local/lib/libdbus-1.3.dylib \
            @executable_path/../Frameworks/libdbus-1.3.dylib \
            $APPDIR/Contents/Frameworks/$i.framework/Versions/$QTVER/$i

        # Change link location for framework in assistant
        echo "Changing link location for $i.framework in assistant..."
        install_name_tool -change \
            $QTDIR/lib/$i.framework/Versions/$QTVER/$i \
            @executable_path/../Frameworks/$i.framework/Versions/$QTVER/$i \
            $ASSISTANT_DIR/Contents/MacOS/assistant

    done

    mkdir -p $APPDIR/Contents/PlugIns

    # Copy plugins into bundle and update references
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

        #  Change reference to QtGui in plugin
        echo "Changing link location for QtGui in $i plugin..."
        install_name_tool -change \
            $QTDIR/lib/QtGui.framework/Versions/$QTVER/QtGui \
            @executable_path/../Frameworks/QtGui.framework/Versions/$QTVER/QtGui \
            $APPDIR/Contents/PlugIns/$i

        # Change reference to system libs in plugin
        echo "Changing link location for libstdc++ in $i plugin..."
        install_name_tool -change \
            /usr/lib/libstdc++.6.dylib \
            @executable_path/../Frameworks/libstdc++.6.dylib \
            $APPDIR/Contents/PlugIns/$i

        echo "Changing link location for libgcc_s in $i plugin..."
        install_name_tool -change \
            /usr/lib/libgcc_s.1.dylib \
            @executable_path/../Frameworks/libgcc_s.1.dylib \
            $APPDIR/Contents/PlugIns/$i

        echo "Changing link location for libz in $i plugin..."
        install_name_tool -change \
            /usr/lib/libz.1.dylib \
            @executable_path/../Frameworks/libz.1.dylib \
            $APPDIR/Contents/PlugIns/$i

        echo "Changing link location for libz in $i plugin..."
        install_name_tool -change \
            /opt/local/lib/libz.1.dylib \
            @executable_path/../Frameworks/libz.1.dylib \
            $APPDIR/Contents/PlugIns/$i

        echo "Changing link location for libdbus in $i plugin..."
        install_name_tool -change \
            /opt/local/lib/libdbus-1.3.dylib \
            @executable_path/../Frameworks/libdbus-1.3.dylib \
            $APPDIR/Contents/PlugIns/$i

    done

    # Change reference to QtGui in Assistant client
    echo "Changing link location for QtGui.framework in QtAssistant.framework..."
    install_name_tool -change \
        $QTDIR/lib/QtGui.framework/Versions/$QTVER/QtGui \
        @executable_path/../Frameworks/QtGui.framework/Versions/$QTVER/QtGui \
        $APPDIR/Contents/Frameworks/QtAssistant.framework/Versions/$QTVER/QtAssistant

    # Change reference to QtNetwork in Assistant client
    echo "Changing link location for QtNetwork.framework in QtAssistant.framework..."
    install_name_tool -change \
        $QTDIR/lib/QtNetwork.framework/Versions/$QTVER/QtNetwork \
        @executable_path/../Frameworks/QtNetwork.framework/Versions/$QTVER/QtNetwork \
        $APPDIR/Contents/Frameworks/QtAssistant.framework/Versions/$QTVER/QtAssistant

    # Change reference to QtCore in Assistant client
    echo "Changing link location for QtCore.framework in QtAssistant.framework..."
    install_name_tool -change \
        $QTDIR/lib/QtCore.framework/Versions/$QTVER/QtCore \
        @executable_path/../Frameworks/QtCore.framework/Versions/$QTVER/QtCore \
        $APPDIR/Contents/Frameworks/QtAssistant.framework/Versions/$QTVER/QtAssistant

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

# Change reference to system libs in libzyzzyva, executable, and system libs
for i in $APPDIR/Contents/Frameworks/libzyzzyva.2.dylib \
    $APPDIR/Contents/MacOS/Zyzzyva \
    $APPDIR/Contents/Frameworks/libdbus-1.3.dylib \
    $APPDIR/Contents/Frameworks/libgcc_s.1.dylib \
    $APPDIR/Contents/Frameworks/libstdc++.6.dylib \
    $APPDIR/Contents/Frameworks/libz.1.dylib \
    ; do

   echo "Changing link location for libstdc++ in $i..."
   install_name_tool -change \
       /usr/lib/libstdc++.6.dylib \
       @executable_path/../Frameworks/libstdc++.6.dylib \
       $i

   echo "Changing link location for libgcc_s in $i..."
   install_name_tool -change \
       /usr/lib/libgcc_s.1.dylib \
       @executable_path/../Frameworks/libgcc_s.1.dylib \
       $i

   echo "Changing link location for libz in $i..."
   install_name_tool -change \
       /usr/lib/libz.1.dylib \
       @executable_path/../Frameworks/libz.1.dylib \
       $i

   echo "Changing link location for libz in $i..."
   install_name_tool -change \
       /opt/local/lib/libz.1.dylib \
       @executable_path/../Frameworks/libz.1.dylib \
       $i

    echo "Changing link location for libdbus in $i plugin..."
    install_name_tool -change \
        /opt/local/lib/libdbus-1.3.dylib \
        @executable_path/../Frameworks/libdbus-1.3.dylib \
        $i

done

echo "Done.  Bundle is called $APPDIR."
