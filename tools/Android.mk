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

ifeq ($(TARGET_DEVICE),bravo)
ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH:= $(call my-dir)

#
# btconfig
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= btconfig.c

LOCAL_C_INCLUDES := \
    system/bluetooth/bluedroid/include \
    system/bluetooth/bluez-clean-headers

LOCAL_SHARED_LIBRARIES := \
    libbluedroid

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE:= btconfig

include $(BUILD_EXECUTABLE)

endif # not BUILD_TINY_ANDROID
endif # TARGET_DEVICE
