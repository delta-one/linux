#!/bin/bash
# SPDX-License-Identifier: GPL-2.0

psinstdir=$(mktemp -d)
if [ $? -ne 0 ]; then
	echo "mktemp failed"
	exit 1
fi
cd $psinstdir
wget "https://fmv.jku.at/picosat/picosat-965.tar.gz"
tar -xf picosat-965.tar.gz
cd picosat-965
cp makefile.in makefile.in2
# change soname to conform with packages for Debian and Fedora
sed -e "s,-soname -Xlinker libpicosat.so,-soname -Xlinker	\
	libpicosat-trace.so.0," makefile.in2 > makefile.in
./configure.sh -O -t --shared
make libpicosat.so
install -m 0755 -p libpicosat.so /usr/local/lib/libpicosat-trace.so.0.0.965	\
&& ln -s -f libpicosat-trace.so.0.0.965 /usr/local/lib/libpicosat-trace.so.0	\
&& ln -s -f libpicosat-trace.so.0 /usr/local/lib/libpicosat-trace.so \
&& ldconfig
echo
if [ $? -ne 0 ]; then
	echo "Installation of PicoSAT failed, make sure you are running with root privileges."
	exit 1
else
	echo "Installation of PicoSAT succeeded."
fi
