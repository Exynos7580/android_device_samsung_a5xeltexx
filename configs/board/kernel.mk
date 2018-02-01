# Boot
BOARD_KERNEL_SEPARATED_DT := true
BOARD_MKBOOTIMG_ARGS := --kernel_offset 0x00008000 --ramdisk_offset 0x01000000 --tags_offset 0x00000100 --board SRPOJ08A000KU
BOARD_CUSTOM_BOOTIMG := true
BOARD_CUSTOM_BOOTIMG_MK := hardware/samsung/mkbootimg.mk
TARGET_CUSTOM_DTBTOOL := dtbhtoolExynos

# Kernel
#BOARD_KERNEL_CMDLINE := The bootloader ignores the cmdline from the boot.img
TARGET_KERNEL_CONFIG := lineageos_a5xelte_defconfig
TARGET_KERNEL_SOURCE := kernel/samsung/exynos7580-common
BOARD_KERNEL_IMAGE_NAME := Image
