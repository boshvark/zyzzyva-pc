#!/bin/sh
# Zyzzyva launch script for Linux
export LD_LIBRARY_PATH=@@INSTALLDIR@@/bin:$LD_LIBRARY_PATH
@@INSTALLDIR@@/bin/@@EXECUTABLE@@
