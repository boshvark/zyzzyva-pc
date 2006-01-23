#!/bin/bash
#-----------------------------------------------------------------------------
# create-macx-dmg.sh
#
# Create a disk image file for Mac OS X.
#
# Copyright 2006 Michael W Thelen <mike@pietdepsi.com>.
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

QTDIR=/usr/local/Trolltech/Qt-4.0.1

# Copy data directory into bundle
echo "Copying data directory into bundle..."
rm -rf zyzzyva.app/Contents/MacOS/data
cp -r data zyzzyva.app/Contents/MacOS

# Copy Assistant client into bundle
echo "Copying Assistant client into bundle..."
cp $QTDIR/bin/assistant.app/Contents/MacOS/assistant \
    zyzzyva.app/Contents/MacOS

# Copy assistant.icns into bundle
echo "Copying assistant.icns into bundle..."
mkdir -p zyzzyva.app/Contents/Resources
cp $QTDIR/bin/assistant.app/Contents/Resources/assistant.icns \
    zyzzyva.app/Contents/Resources

# Create Contents/Frameworks directory in bundle
echo "Creating Contents/Frameworks directory in bundle..."
rm -rf zyzzyva.app/Contents/Frameworks
mkdir -p zyzzyva.app/Contents/Frameworks

# Copy Qt frameworks into bundle and tell the executable to link to them
for i in Qt3Support QtGui QtNetwork QtSql QtXml QtCore ; do

    # Copy Qt framework into bundle
    echo "Copying $i.framework into bundle..."
    cp -R /usr/local/Trolltech/Qt-4.0.1/lib/$i.framework \
        zyzzyva.app/Contents/Frameworks

    # Delete headers and debug libraries
    echo "Deleting $i.framework headers and debug libraries..."
    rm zyzzyva.app/Contents/Frameworks/$i.framework/Headers
    rm -rf zyzzyva.app/Contents/Frameworks/$i.framework/Versions/4.0/Headers
    find zyzzyva.app/Contents/Frameworks -name arch -or -name '*debug*' \
        -print0 | xargs -0 rm -rf

    # Set identification names for the private framework
    echo "Setting identification name for $i.framework..."
    install_name_tool \
        -id @executable_path/../Frameworks/$i.framework/Versions/4.0/$i \
        zyzzyva.app/Contents/Frameworks/$i.framework/Versions/4.0/$i

    # Change link locations for framework in executable - ORIG
    echo "Changing link location for $i.framework in Zyzzyva executable..."
    install_name_tool -change \
        /usr/local/Trolltech/Qt-4.0.1/lib/$i.framework/Versions/4.0/$i \
        @executable_path/../Frameworks/$i.framework/Versions/4.0/$i \
        zyzzyva.app/Contents/MacOS/zyzzyva
done

# Change reference to QtCore in QtGui framework
echo "Changing link location for QtCore.framework in QtGui.framework..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtCore.framework/Versions/4.0/QtCore \
    @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore \
    zyzzyva.app/Contents/Frameworks/QtGui.framework/Versions/4.0/QtGui

# Change reference to QtCore in QtNetwork framework
echo "Changing link location for QtCore.framework in QtNetwork.framework..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtCore.framework/Versions/4.0/QtCore \
    @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore \
    zyzzyva.app/Contents/Frameworks/QtNetwork.framework/Versions/4.0/QtNetwork

# Change reference to QtCore in QtSql framework
echo "Changing link location for QtCore.framework in QtSql.framework..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtCore.framework/Versions/4.0/QtCore \
    @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore \
    zyzzyva.app/Contents/Frameworks/QtSql.framework/Versions/4.0/QtSql

# Change reference to QtCore in QtXml framework
echo "Changing link location for QtCore.framework in QtXml.framework..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtCore.framework/Versions/4.0/QtCore \
    @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore \
    zyzzyva.app/Contents/Frameworks/QtXml.framework/Versions/4.0/QtXml

# Change reference to QtCore in Qt3Support framework
echo "Changing link location for QtCore.framework in Qt3Support.framework..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtCore.framework/Versions/4.0/QtCore \
    @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore \
    zyzzyva.app/Contents/Frameworks/Qt3Support.framework/Versions/4.0/Qt3Support

# Change reference to QtGui in Qt3Support framework
echo "Changing link location for QtGui.framework in Qt3Support.framework..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtGui.framework/Versions/4.0/QtGui \
    @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui \
    zyzzyva.app/Contents/Frameworks/Qt3Support.framework/Versions/4.0/Qt3Support

# Change reference to QtNetwork in Qt3Support framework
echo "Changing link location for QtNetwork.framework in Qt3Support.framework..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtNetwork.framework/Versions/4.0/QtNetwork \
    @executable_path/../Frameworks/QtNetwork.framework/Versions/4.0/QtNetwork \
    zyzzyva.app/Contents/Frameworks/Qt3Support.framework/Versions/4.0/Qt3Support

# Change reference to QtSql in Qt3Support framework
echo "Changing link location for QtSql.framework in Qt3Support.framework..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtSql.framework/Versions/4.0/QtSql \
    @executable_path/../Frameworks/QtSql.framework/Versions/4.0/QtSql \
    zyzzyva.app/Contents/Frameworks/Qt3Support.framework/Versions/4.0/Qt3Support

# Change reference to QtXml in Qt3Support framework
echo "Changing link location for QtXml.framework in Qt3Support.framework..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtXml.framework/Versions/4.0/QtXml \
    @executable_path/../Frameworks/QtXml.framework/Versions/4.0/QtXml \
    zyzzyva.app/Contents/Frameworks/Qt3Support.framework/Versions/4.0/Qt3Support

# Change reference to QtCore in Assistant client
echo "Changing link location for QtCore.framework in Assistant client..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtCore.framework/Versions/4.0/QtCore \
    @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore \
    zyzzyva.app/Contents/MacOS/assistant

# Change reference to QtGui in Assistant client
echo "Changing link location for QtGui.framework in Assistant client..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtGui.framework/Versions/4.0/QtGui \
    @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui \
    zyzzyva.app/Contents/MacOS/assistant

# Change reference to QtNetwork in Assistant client
echo "Changing link location for QtNetwork.framework in Assistant client..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtNetwork.framework/Versions/4.0/QtNetwork \
    @executable_path/../Frameworks/QtNetwork.framework/Versions/4.0/QtNetwork \
    zyzzyva.app/Contents/MacOS/assistant

# Change reference to QtXml in Assistant client
echo "Changing link location for QtXml.framework in Assistant client..."
install_name_tool -change \
    /usr/local/Trolltech/Qt-4.0.1/lib/QtXml.framework/Versions/4.0/QtXml \
    @executable_path/../Frameworks/QtXml.framework/Versions/4.0/QtXml \
    zyzzyva.app/Contents/MacOS/assistant

# Create disk image
echo "Creating disk image..."
rm -rf zyzzyva.dmg
hdiutil create -srcfolder zyzzyva.app -volname Zyzzyva zyzzyva.dmg
hdiutil attach zyzzyva.dmg
DEVS=$(hdiutil attach zyzzyva.dmg | cut -f 1)
DEV=$(echo $DEVS | cut -f 1 -d ' ')
hdiutil detach $DEV
hdiutil internet-enable -yes zyzzyva.dmg
hdiutil convert zyzzyva.dmg -format UDZO -o zyzzyva-output.dmg
mv zyzzyva-output.dmg zyzzyva.dmg

echo "Done.  Disk image is called zyzzyva.dmg."
