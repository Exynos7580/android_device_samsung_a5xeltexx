#ifeq ($(TARGET_BUILD_VARIANT),eng)

PRODUCT_PROPERTY_OVERRIDES += \
       ro.secure=0 \
       ro.adb.secure=0 \
       ro.hardware=universal7580 \
       persist.service.adb.enable=1 \
       persist.service.debuggable=1 \
       persist.sys.usb.config=adb \
       ro.securestorage.support=false

# endif
