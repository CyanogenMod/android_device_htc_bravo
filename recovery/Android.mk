ifeq ($(TARGET_BOOTLOADER_BOARD_NAME),bravo)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng

LOCAL_C_INCLUDES += bootable/recovery
LOCAL_SRC_FILES := recovery_ui.c

# should match TARGET_RECOVERY_UI_LIB set in BoardConfig.mk
LOCAL_MODULE := librecovery_ui_bravo

include $(BUILD_STATIC_LIBRARY)

endif   # TARGET_BOOTLOADER_BOARD_NAME
