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

# Initialise device config
$(call inherit-product, device/samsung/a5xeltexx/full_a5xeltexx.mk)

# Inherit common Lineage phone.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Set those variables here to overwrite the inherited values.
PRODUCT_NAME := lineage_a5xeltexx
PRODUCT_DEVICE := a5xeltexx
PRODUCT_MODEL := SM-A510F
PRODUCT_BRAND := samsung
PRODUCT_MANUFACTURER := samsung
