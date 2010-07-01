#
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
#

#
# This is the device-specific product configuration for bravo,
# configured with Asia-specific settings.
#

# The gps config appropriate for this device
$(call inherit-product, device/common/gps/gps_as_supl.mk)

# The rest of the configuration is inherited from a generic config
$(call inherit-product, device/htc/bravo/bravo.mk)
