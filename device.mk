#
# Copyright (C) 2016 The CyanogenMod Project
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

LOCAL_PATH := device/samsung/a5xeltexx

$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

# Overlays
DEVICE_PACKAGE_OVERLAYS += device/samsung/a5xeltexx/overlay

# Device uses high-density artwork where available
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := 480dpi
PRODUCT_AAPT_PREBUILT_DPI := xxhdpi xhdpi hdpi

# Boot animation
TARGET_SCREEN_HEIGHT := 1980
TARGET_SCREEN_WIDTH := 1080

# Audio
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/audio/audio_policy.conf:system/etc/audio_policy.conf \
    $(LOCAL_PATH)/configs/audio/mixer_paths.xml:system/etc/mixer_paths.xml

# Bluetooth
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/bluetooth/bt_vendor.conf:system/etc/bluetooth/bt_vendor.conf

# GPS
PRODUCT_PACKAGES += \
    libdmitry

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/gps/gps.conf:system/etc/gps.conf \
    $(LOCAL_PATH)/configs/gps/gps.xml:system/etc/gps.xml

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:system/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:system/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml \
    frameworks/native/data/etc/com.android.nfc_extras.xml:system/etc/permissions/com.android.nfc_extras.xml

# Lights
PRODUCT_PACKAGES += \
    lights.universal7580

# Media profile
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/media/media_profiles.xml:system/etc/media_profiles.xml

# Power
PRODUCT_PACKAGES += \
    power.universal7580

# Ramdisk
PRODUCT_PACKAGES += \
    fstab.samsungexynos7580 \
    init.baseband.rc \
    init.rilcommon.rc \
    init.samsungexynos7580.rc \
    init.samsungexynos7580.usb.rc \
    init.wifi.rc \
    ueventd.samsungexynos7580.rc

# NFC
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/nfc/libnfc-sec-hal.conf:system/etc/libnfc-sec-hal.conf \
    $(LOCAL_PATH)/configs/nfc/libnfc-sec.conf:system/etc/libnfc-brcm.conf \
    $(LOCAL_PATH)/configs/nfc/nfcee_access.xml:system/etc/nfcee_access.xml

PRODUCT_PACKAGES += \
    com.android.nfc_extras \
    NfcNci \
    Tag

PRODUCT_PROPERTY_OVERRIDES += \
    ro.nfc.sec_hal=true


# SYMBOLS FOR BLOBS
PRODUCT_PACKAGES += \
    libsamsung_symbols

# cpboot-daemon for modem
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ril/sbin/cbd:root/sbin/cbd

# Inherit from Exynos7580-common
$(call inherit-product, device/samsung/exynos7580-common/device-common.mk)

# Get non-open-source specific aspects
$(call inherit-product-if-exists, vendor/samsung/a5xeltexx/a5xeltexx-vendor.mk)
