# RIL
PRODUCT_PACKAGES += \
    libprotobuf-cpp-full \
    modemloader \
    libxml2 \
    rild \
    libril \
    libreference-ril \
    libsecril-client \
    libsecril-client-sap \
    android.hardware.radio@1.0 \
    android.hardware.radio.deprecated@1.0

# cpboot-daemon for modem
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ril/sbin/cbd:system/bin/cbd \
    $(LOCAL_PATH)/rootdir/etc/vendor/rild.rc:system/vendor/etc/init/rild.rc
