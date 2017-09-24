####################
# Radio            #
####################

PRODUCT_PACKAGES += \
    libprotobuf-cpp-full \
    modemloader

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/etc/rild.rc:system/etc/init/rild.rc
