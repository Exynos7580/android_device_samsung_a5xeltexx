# NFC
PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/configs/nfc/libnfc-sec-hal.conf:system/etc/libnfc-sec-hal.conf \
	$(LOCAL_PATH)/configs/nfc/libnfc-sec.conf:system/etc/libnfc-brcm.conf \
	$(LOCAL_PATH)/configs/nfc/nfcee_access.xml:system/etc/nfcee_access.xml

PRODUCT_PACKAGES += \
	libnfc \
    libnfc_jni \
    com.broadcom.nfc \
    NfcNci \
    Tag \
	com.android.nfc_extras \
	android.hardware.nfc@1.0-impl-bcm \
	android.hardware.nfc@1.0-service

