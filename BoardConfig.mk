#
# Copyright 2016 The CyanogenMod Project
# Copyright 2017-2018 The LineageOS Project
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

# Inherit from Exynos7580-common
include device/samsung/exynos7580-common/BoardConfigCommon.mk

TARGET_OTA_ASSERT_DEVICE := a5xelte, a5xeltexx, a5xelteub, a5xeltedo

DEVICE_PATH := device/samsung/a5xeltexx

# Include makefiles from board folder
-include $(DEVICE_PATH)/configs/board/*.mk

# inherit from the proprietary version
-include vendor/samsung/a5xeltexx/BoardConfigVendor.mk
