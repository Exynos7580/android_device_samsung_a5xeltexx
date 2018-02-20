LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libutils \
	libcutils \
	libtinyalsa

LOCAL_C_INCLUDES := \
	external/tinyalsa/include \
	external/tinycompress/include \
	hardware/libhardware/include \
	hardware/samsung/audio \
	$(call include-path-for, audio-utils) \
	$(call include-path-for, audio-route) \
	$(call include-path-for, audio-effects)

LOCAL_SRC_FILES := \
	amplifier.c \
	tfa.c

LOCAL_MODULE := audio_amplifier.$(TARGET_BOOTLOADER_BOARD_NAME)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
