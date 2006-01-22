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

# Copy data directory into bundle
echo "Copying data directory into bundle..."
rm -rf zyzzyva.app/Contents/MacOS/data
cp -r data zyzzyva.app/Contents/MacOS

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

    # Change link locations for framework in executable
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
