# Copyright (C) 2009 The Android Open Source Project
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

# config.mk
#
# Product-specific compile-time definitions.
#

TARGET_NO_BOOTLOADER := true

TARGET_BOARD_PLATFORM := qsd8k
TARGET_BOARD_PLATFORM_GPU := qcom-adreno200

TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_ARCH_VARIANT := armv7-a-neon

TARGET_BOOTLOADER_BOARD_NAME := bravo

# Wifi related defines
BOARD_WPA_SUPPLICANT_DRIVER := WEXT
WPA_SUPPLICANT_VERSION      := VER_0_6_X
BOARD_WLAN_DEVICE           := bcm4329
WIFI_DRIVER_MODULE_PATH     := "/system/lib/modules/bcm4329.ko"
WIFI_DRIVER_FW_STA_PATH     := "/system/etc/firmware/fw_bcm4329.bin"
WIFI_DRIVER_MODULE_ARG      := "firmware_path=/system/etc/firmware/fw_bcm4329.bin nvram_path=/proc/calibration"
WIFI_DRIVER_MODULE_NAME     := "bcm4329"

BOARD_USES_GENERIC_AUDIO := false

BOARD_KERNEL_CMDLINE := no_console_suspend=1 msmsdcc_sdioirq=1 wire.search_count=5
BOARD_KERNEL_BASE := 0x20000000

BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true

BOARD_VENDOR_QCOM_AMSS_VERSION := 3200

BOARD_VENDOR_USE_AKMD := akm8973

BOARD_EGL_CFG := device/htc/bravo/egl.cfg

# # cat /proc/mtd
# dev:    size   erasesize  name
# mtd0: 000e0000 00020000 "misc"
# mtd1: 00500000 00020000 "recovery"
# mtd2: 00280000 00020000 "boot"
# mtd3: 07800000 00020000 "system"
# mtd4: 07800000 00020000 "cache"
# mtd5: 0c440000 00020000 "userdata"
# mtd6: 00200000 00020000 "crashdata"
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x00280000
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x00500000
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 0x08400000   # limited so we enforce room to grow
BOARD_USERDATAIMAGE_PARTITION_SIZE := 0x0c440000
BOARD_FLASH_BLOCK_SIZE := 131072

TARGET_RECOVERY_UI_LIB := librecovery_ui_bravo librecovery_ui_htc

TARGET_RECOVERY_UPDATER_LIBS += librecovery_updater_htc

TARGET_RELEASETOOLS_EXTENSIONS := device/htc/common
