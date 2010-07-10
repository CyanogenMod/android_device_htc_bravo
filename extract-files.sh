#!/bin/sh

# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

DEVICE=bravo

mkdir -p ../../../vendor/htc/$DEVICE/proprietary
adb pull /system/bin/akmd ../../../vendor/htc/$DEVICE/proprietary/akmd
chmod 755 ../../../vendor/htc/$DEVICE/proprietary/akmd
adb pull /system/bin/mm-venc-omx-test ../../../vendor/htc/$DEVICE/proprietary/mm-venc-omx-test
chmod 755 ../../../vendor/htc/$DEVICE/proprietary/mm-venc-omx-test
adb pull /system/bin/parse_radio_log ../../../vendor/htc/$DEVICE/proprietary/parse_radio_log
chmod 755 ../../../vendor/htc/$DEVICE/proprietary/parse_radio_log
adb pull /system/etc/01_qcomm_omx.cfg ../../../vendor/htc/$DEVICE/proprietary/01_qcomm_omx.cfg
adb pull /system/etc/AudioBTID.csv ../../../vendor/htc/$DEVICE/proprietary/AudioBTID.csv
adb pull /system/etc/firmware/bcm4329.hcd ../../../vendor/htc/$DEVICE/proprietary/bcm4329.hcd
adb pull /system/etc/firmware/default.acdb ../../../vendor/htc/$DEVICE/proprietary/default.acdb
adb pull /system/etc/firmware/default_att.acdb ../../../vendor/htc/$DEVICE/proprietary/default_att.acdb
adb pull /system/etc/firmware/default_france.acdb ../../../vendor/htc/$DEVICE/proprietary/default_france.acdb
adb pull /system/etc/firmware/default_nel.acdb ../../../vendor/htc/$DEVICE/proprietary/default_nel.acdb
adb pull /system/etc/firmware/fw_bcm4329_apsta.bin ../../../vendor/htc/$DEVICE/proprietary/fw_bcm4329_apsta.bin
adb pull /system/etc/firmware/fw_bcm4329.bin ../../../vendor/htc/$DEVICE/proprietary/fw_bcm4329.bin
adb pull /system/etc/firmware/yamato_pfp.fw ../../../vendor/htc/$DEVICE/proprietary/yamato_pfp.fw
adb pull /system/etc/firmware/yamato_pm4.fw ../../../vendor/htc/$DEVICE/proprietary/yamato_pm4.fw
adb pull /system/lib/egl/libEGL_adreno200.so ../../../vendor/htc/$DEVICE/proprietary/libEGL_adreno200.so
adb pull /system/lib/egl/libGLESv1_CM_adreno200.so ../../../vendor/htc/$DEVICE/proprietary/libGLESv1_CM_adreno200.so
adb pull /system/lib/egl/libGLESv2_adreno200.so ../../../vendor/htc/$DEVICE/proprietary/libGLESv2_adreno200.so
adb pull /system/lib/egl/libq3dtools_adreno200.so ../../../vendor/htc/$DEVICE/proprietary/libq3dtools_adreno200.so
adb pull /system/lib/libcamera.so ../../../vendor/htc/$DEVICE/proprietary/libcamera.so
adb pull /system/lib/libgps.so ../../../vendor/htc/$DEVICE/proprietary/libgps.so
adb pull /system/lib/libgsl.so ../../../vendor/htc/$DEVICE/proprietary/libgsl.so
adb pull /system/lib/libhtc_acoustic.so ../../../vendor/htc/$DEVICE/proprietary/libhtc_acoustic.so
adb pull /system/lib/libhtc_ril.so ../../../vendor/htc/$DEVICE/proprietary/libhtc_ril.so
adb pull /system/lib/liblvmxipc.so ../../../vendor/htc/$DEVICE/proprietary/liblvmxipc.so
adb pull /system/lib/libmm-omxcore.so ../../../vendor/htc/$DEVICE/proprietary/libmm-omxcore.so
adb pull /system/lib/liboemcamera.so ../../../vendor/htc/$DEVICE/proprietary/liboemcamera.so
adb pull /system/lib/libOmxCore.so ../../../vendor/htc/$DEVICE/proprietary/libOmxCore.so
adb pull /system/lib/libOmxVdec.so ../../../vendor/htc/$DEVICE/proprietary/libOmxVdec.so
adb pull /system/lib/libOmxVidEnc.so ../../../vendor/htc/$DEVICE/proprietary/libOmxVidEnc.so
adb pull /system/lib/hw/sensors.bravo.so ../../../vendor/htc/$DEVICE/proprietary/sensors.bravo.so
adb pull /system/lib/libomx_wmadec_sharedlibrary.so ../../../vendor/htc/$DEVICE/proprietary/libomx_wmadec_sharedlibrary.so
adb pull /system/lib/libomx_wmvdec_sharedlibrary.so ../../../vendor/htc/$DEVICE/proprietary/libomx_wmvdec_sharedlibrary.so
adb pull /system/lib/libpvasfcommon.so ../../../vendor/htc/$DEVICE/proprietary/libpvasfcommon.so
adb pull /system/lib/libpvasflocalpbreg.so ../../../vendor/htc/$DEVICE/proprietary/libpvasflocalpbreg.so
adb pull /system/lib/libpvasflocalpb.so ../../../vendor/htc/$DEVICE/proprietary/libpvasflocalpb.so
adb pull /system/etc/pvasflocal.cfg ../../../vendor/htc/$DEVICE/proprietary/pvasflocal.cfg

(cat << EOF) | sed s/__DEVICE__/$DEVICE/g > ../../../vendor/htc/$DEVICE/$DEVICE-vendor-blobs.mk
# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# This file is generated by device/htc/__DEVICE__/extract-files.sh

# Prebuilt libraries that are needed to build open-source libraries
PRODUCT_COPY_FILES := \\
    vendor/htc/__DEVICE__/proprietary/libgps.so:obj/lib/libgps.so \\
    vendor/htc/__DEVICE__/proprietary/libcamera.so:obj/lib/libcamera.so

# All the blobs necessary for bravo
PRODUCT_COPY_FILES += \\
    vendor/htc/__DEVICE__/proprietary/akmd:/system/bin/akmd \\
    vendor/htc/__DEVICE__/proprietary/mm-venc-omx-test:/system/bin/mm-venc-omx-test \\
    vendor/htc/__DEVICE__/proprietary/parse_radio_log:/system/bin/parse_radio_log \\
    vendor/htc/__DEVICE__/proprietary/01_qcomm_omx.cfg:/system/etc/01_qcomm_omx.cfg \\
    vendor/htc/__DEVICE__/proprietary/AudioBTID.csv:/system/etc/AudioBTID.csv \\
    vendor/htc/__DEVICE__/proprietary/bcm4329.hcd:/system/etc/firmware/bcm4329.hcd \\
    vendor/htc/__DEVICE__/proprietary/default.acdb:/system/etc/firmware/default.acdb \\
    vendor/htc/__DEVICE__/proprietary/default_att.acdb:/system/etc/firmware/default_att.acdb \\
    vendor/htc/__DEVICE__/proprietary/default_france.acdb:/system/etc/firmware/default_france.acdb \\
    vendor/htc/__DEVICE__/proprietary/default_nel.acdb:/system/etc/firmware/default_nel.acdb \\
    vendor/htc/__DEVICE__/proprietary/fw_bcm4329_apsta.bin:/system/etc/firmware/fw_bcm4329_apsta.bin \\
    vendor/htc/__DEVICE__/proprietary/fw_bcm4329.bin:/system/etc/firmware/fw_bcm4329.bin \\
    vendor/htc/__DEVICE__/proprietary/yamato_pfp.fw:/system/etc/firmware/yamato_pfp.fw \\
    vendor/htc/__DEVICE__/proprietary/yamato_pm4.fw:/system/etc/firmware/yamato_pm4.fw \\
    vendor/htc/__DEVICE__/proprietary/libEGL_adreno200.so:/system/lib/egl/libEGL_adreno200.so \\
    vendor/htc/__DEVICE__/proprietary/libGLESv1_CM_adreno200.so:/system/lib/egl/libGLESv1_CM_adreno200.so \\
    vendor/htc/__DEVICE__/proprietary/libGLESv2_adreno200.so:/system/lib/egl/libGLESv2_adreno200.so \\
    vendor/htc/__DEVICE__/proprietary/libq3dtools_adreno200.so:/system/lib/egl/libq3dtools_adreno200.so \\
    vendor/htc/__DEVICE__/proprietary/libcamera.so:/system/lib/libcamera.so \\
    vendor/htc/__DEVICE__/proprietary/libgps.so:/system/lib/libgps.so \\
    vendor/htc/__DEVICE__/proprietary/libgsl.so:/system/lib/libgsl.so \\
    vendor/htc/__DEVICE__/proprietary/libhtc_acoustic.so:/system/lib/libhtc_acoustic.so \\
    vendor/htc/__DEVICE__/proprietary/libhtc_ril.so:/system/lib/libhtc_ril.so \\
    vendor/htc/__DEVICE__/proprietary/liblvmxipc.so:/system/lib/liblvmxipc.so \\
    vendor/htc/__DEVICE__/proprietary/libmm-omxcore.so:/system/lib/libmm-omxcore.so \\
    vendor/htc/__DEVICE__/proprietary/liboemcamera.so:/system/lib/liboemcamera.so \\
    vendor/htc/__DEVICE__/proprietary/libOmxCore.so:/system/lib/libOmxCore.so \\
    vendor/htc/__DEVICE__/proprietary/libOmxVdec.so:/system/lib/libOmxVdec.so \\
    vendor/htc/__DEVICE__/proprietary/libOmxVidEnc.so:/system/lib/libOmxVidEnc.so \\
    vendor/htc/__DEVICE__/proprietary/sensors.bravo.so:/system/lib/hw/sensors.bravo.so

ifdef WITH_WINDOWS_MEDIA
PRODUCT_COPY_FILES += \\
    vendor/htc/__DEVICE__/proprietary/libomx_wmadec_sharedlibrary.so:system/lib/libomx_wmadec_sharedlibrary.so \\
    vendor/htc/__DEVICE__/proprietary/libomx_wmvdec_sharedlibrary.so:system/lib/libomx_wmvdec_sharedlibrary.so \\
    vendor/htc/__DEVICE__/proprietary/libpvasfcommon.so:system/lib/libpvasfcommon.so \\
    vendor/htc/__DEVICE__/proprietary/libpvasflocalpbreg.so:system/lib/libpvasflocalpbreg.so \\
    vendor/htc/__DEVICE__/proprietary/libpvasflocalpb.so:system/lib/libpvasflocalpb.so \\
    vendor/htc/__DEVICE__/proprietary/pvasflocal.cfg:system/etc/pvasflocal.cfg
endif

EOF

./setup-makefiles.sh
