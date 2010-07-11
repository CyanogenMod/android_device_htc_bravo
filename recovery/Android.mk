ifneq ($(TARGET_SIMULATOR),true)
ifeq ($(TARGET_ARCH),arm)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng

LOCAL_C_INCLUDES += bootable/recovery
LOCAL_SRC_FILES := recovery_ui.c

# should match TARGET_RECOVERY_UI_LIB set in BoardConfig.mk
LOCAL_MODULE := librecovery_ui_inc

include $(BUILD_STATIC_LIBRARY)

endif   # TARGET_ARCH == arm
endif   # !TARGET_SIMULATOR
