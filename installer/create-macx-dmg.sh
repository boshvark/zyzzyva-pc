#!/bin/bash
#-----------------------------------------------------------------------------
# create-macx-dmg.sh
#
# Create a disk image file for Mac OS X.
#
# Copyright 2006 Michael W Thelen <mthelen@gmail.com>.
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

QTDIR=/usr/local/Trolltech/Qt-4.2.1
QTVER=4

# Copy executable into bundle
echo "Copying Zyzzyva executable into bundle..."
rm -rf Zyzzyva.app
mkdir Zyzzyva.app
cp -r bin/Zyzzyva.app/Contents Zyzzyva.app

# Copy zyzzyva.tap
echo "Copying zyzzyva.top into bundle..."
cp zyzzyva.top Zyzzyva.app/Contents/MacOS

# Copy data directory into bundle
echo "Copying data directory into bundle..."
cp -r data Zyzzyva.app/Contents/MacOS

# Copy Assistant client into bundle
echo "Copying Assistant client into bundle..."
cp $QTDIR/bin/assistant.app/Contents/MacOS/assistant \
    Zyzzyva.app/Contents/MacOS

# Copy assistant.icns into bundle
echo "Copying assistant.icns into bundle..."
mkdir -p Zyzzyva.app/Contents/Resources
cp $QTDIR/bin/assistant.app/Contents/Resources/assistant.icns \
    Zyzzyva.app/Contents/Resources

# Create Contents/Frameworks directory in bundle
echo "Creating Contents/Frameworks directory in bundle..."
rm -rf Zyzzyva.app/Contents/Frameworks
mkdir -p Zyzzyva.app/Contents/Frameworks

# Copy Zyzzyva libs to Frameworks directory
echo "Copying libzyzzyva into bundle..."
cp bin/libzyzzyva.1.dylib Zyzzyva.app/Contents/Frameworks

# Change link location for libzyzzyva in executable
echo "Changing link location for libzyzzyva in Zyzzyva executable..."
install_name_tool -change \
    libzyzzyva.1.dylib \
    @executable_path/../Frameworks/libzyzzyva.1.dylib \
    Zyzzyva.app/Contents/MacOS/Zyzzyva

# Copy Qt frameworks into bundle and tell the executable to link to them
for i in QtCore QtGui QtNetwork QtSql QtXml QtAssistantClient ; do

    # Copy Qt framework into bundle
    echo "Copying $i.framework into bundle..."
    cp -R $QTDIR/lib/$i.framework \
        Zyzzyva.app/Contents/Frameworks

    # Delete headers and debug libraries
    echo "Deleting $i.framework headers and debug libraries..."
    rm Zyzzyva.app/Contents/Frameworks/$i.framework/Headers
    rm -rf Zyzzyva.app/Contents/Frameworks/$i.framework/Versions/$QTVER/Headers
    find Zyzzyva.app/Contents/Frameworks -name arch -or -name '*debug*' \
        -print0 | xargs -0 rm -rf

    # Set identification names for the private framework
    echo "Setting identification name for $i.framework..."
    install_name_tool \
        -id @executable_path/../Frameworks/$i.framework/Versions/$QTVER/$i \
        Zyzzyva.app/Contents/Frameworks/$i.framework/Versions/$QTVER/$i

    # Change reference to framework in libzyzzyva
    echo "Changing link location for $i.framework in libzyzzyva..."
    install_name_tool -change \
        $QTDIR/lib/$i.framework/Versions/$QTVER/$i \
        @executable_path/../Frameworks/$i.framework/Versions/$QTVER/$i \
        Zyzzyva.app/Contents/Frameworks/libzyzzyva.1.dylib

    # Change link locations for framework in executable
    echo "Changing link location for $i.framework in Zyzzyva executable..."
    install_name_tool -change \
        $QTDIR/lib/$i.framework/Versions/$QTVER/$i \
        @executable_path/../Frameworks/$i.framework/Versions/$QTVER/$i \
        Zyzzyva.app/Contents/MacOS/Zyzzyva

    #  Change reference to QtCore in frameworks
    if [ "$i" != "QtCore" ]; then
        echo "Changing link location for $i.framework in QtGui.framework..."
        install_name_tool -change \
            $QTDIR/lib/QtCore.framework/Versions/$QTVER/QtCore \
            @executable_path/../Frameworks/QtCore.framework/Versions/$QTVER/QtCore \
            Zyzzyva.app/Contents/Frameworks/$i.framework/Versions/$QTVER/$i
    fi

done

# Change reference to QtGui in Assistant client
echo "Changing link location for QtGui.framework in QtAssistantClient.framework..."
install_name_tool -change \
    $QTDIR/lib/QtGui.framework/Versions/$QTVER/QtGui \
    @executable_path/../Frameworks/QtGui.framework/Versions/$QTVER/QtGui \
    Zyzzyva.app/Contents/Frameworks/QtAssistantClient.framework/Versions/$QTVER/QtAssistantClient

# Change reference to QtNetwork in Assistant client
echo "Changing link location for QtNetwork.framework in QtAssistantClient.framework..."
install_name_tool -change \
    $QTDIR/lib/QtNetwork.framework/Versions/$QTVER/QtNetwork \
    @executable_path/../Frameworks/QtNetwork.framework/Versions/$QTVER/QtNetwork \
    Zyzzyva.app/Contents/Frameworks/QtAssistantClient.framework/Versions/$QTVER/QtAssistantClient

# Change reference to QtCore in Assistant client
echo "Changing link location for QtCore.framework in QtAssistantClient.framework..."
install_name_tool -change \
    $QTDIR/lib/QtCore.framework/Versions/$QTVER/QtCore \
    @executable_path/../Frameworks/QtCore.framework/Versions/$QTVER/QtCore \
    Zyzzyva.app/Contents/Frameworks/QtAssistantClient.framework/Versions/$QTVER/QtAssistantClient

# Create disk image
echo "Creating disk image..."
rm -rf Zyzzyva.dmg
hdiutil create -srcfolder Zyzzyva.app -volname Zyzzyva Zyzzyva.dmg
hdiutil attach Zyzzyva.dmg
DEVS=$(hdiutil attach Zyzzyva.dmg | cut -f 1)
DEV=$(echo $DEVS | cut -f 1 -d ' ')
hdiutil detach $DEV
hdiutil internet-enable -yes Zyzzyva.dmg
hdiutil convert Zyzzyva.dmg -format UDZO -o Zyzzyva-output.dmg
mv Zyzzyva-output.dmg Zyzzyva.dmg

echo "Done.  Disk image is called Zyzzyva.dmg."
