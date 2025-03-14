#
# If not stated otherwise in this file or this component's Licenses.txt file the
# following copyright and licenses apply:
#
# Copyright 2017 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
SUMMARY = "This recipe add a new sample component "
SECTION = "console/utils"
LICENSE = "Apache-2.0"
FILESEXTRAPATHS_prepend := "${THISDIR}:"
SRC_URI = "file://MotionDetection.zip"
S = "${WORKDIR}/MotionDetection"
LIC_FILES_CHKSUM = "file://LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57"

DEPENDS = "ccsp-common-library dbus openssl util-linux utopia cjson telemetry libunpriv libnl-tiny"
DEPENDS_append = " ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'systemd', '', d)}"

CFLAGS_append = " \
    -I=${includedir}/dbus-1.0 \
    -I=${libdir}/dbus-1.0/include \
    -I=${includedir}/ccsp \
    -I=${includedir}/libnl-tiny  \
    "

CXXFLAGS_append = " -I=${includedir}/libnl-tiny "

LDFLAGS_append = " \
    -ldbus-1 \
    -lccsp_common \
    -lnl-tiny \
    "

inherit pkgconfig cmake

do_install() {
        install -d ${D}${libdir}
        install -m 0644 libmotiondetector.a ${D}${libdir}
        install -d ${D}${includedir}/MotionDetection/
        install -m 0644 ../MotionDetection/md.h ${D}${includedir}/MotionDetection/
}

FILES:${PN} += "${libdir}/libmotiondetector.a"
FILES:${PN}-dev += "${includedir}/MotionDetection/md.h"
