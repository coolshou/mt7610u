#!/bin/bash

make clean
#fakeroot debian/rules clean
VERSION=`cat include/version.h | grep DRIVER_VERSION | cut -d" " -f3 | cut -d"\"" -f2`
DRIVER_BUILD_VERSION=`cat os/linux/config.mk | grep 'STA_DRIVER_BUILD' | cut -d' ' -f3 |cut -d'"' -f2 | head -n1`
sed -e "s|#VERSION#|${VERSION}|" \
	-e "s|#DRIVER_BUILD_VERSION#|${DRIVER_BUILD_VERSION}|" \
			debian/changelog.in > debian/changelog; \

dpkg-buildpackage -b -uc -tc
