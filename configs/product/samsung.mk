# Samsung
PRODUCT_PACKAGES += \
    SamsungServiceMode \
	dtbhtoolExynos

# samsung's sswap
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ramdisk/external/sswap:root/sbin/sswap