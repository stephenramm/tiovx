ifeq ($(BUILD_VIDEO_IO_KERNELS),yes)
ifeq ($(BUILD_CAPTURE), yes)

ifneq ($(SOC),am62a)
ifeq ($(TARGET_CPU), $(filter $(TARGET_CPU), R5F))

include $(PRELUDE)
TARGET      := vx_target_kernels_capture
TARGETTYPE  := library
CSOURCES    := $(call all-c-files)
IDIRS       += $(CUSTOM_KERNEL_PATH)/include
IDIRS       += $(CUSTOM_KERNEL_PATH)/video_io/include
IDIRS       += $(VXLIB_PATH)/packages
ifeq ($(RTOS_SDK), mcu_plus_sdk)
IDIRS       += $(MCU_PLUS_SDK_PATH)/source
IDIRS       += $(MCU_PLUS_SDK_PATH)/source/drivers
else
IDIRS       += $(PDK_PATH)/packages
IDIRS       += $(PDK_PATH)/packages/ti/drv
endif
IDIRS       += $(TIOVX_PATH)/source/include

include $(FINALE)

endif

else # ifneq ($(SOC),am62a)

ifeq ($(SOC)$(TARGET_OS),am62aQNX)

include $(PRELUDE)
TARGET      := vx_target_kernels_capture
TARGETTYPE  := library
CSOURCES    := $(call all-c-files)
IDIRS       += $(CUSTOM_KERNEL_PATH)/include
IDIRS       += $(CUSTOM_KERNEL_PATH)/video_io/include
IDIRS       += $(VXLIB_PATH)/packages
ifeq ($(RTOS_SDK), mcu_plus_sdk)
IDIRS       += $(MCU_PLUS_SDK_PATH)/source
IDIRS       += $(MCU_PLUS_SDK_PATH)/source/drivers
else
IDIRS       += $(PDK_QNX_PATH)/packages
IDIRS       += $(PDK_QNX_PATH)/packages/ti/drv
endif
IDIRS       += $(TIOVX_PATH)/source/include

include $(FINALE)

endif
endif # ifneq ($(SOC),am62a)

endif # ifeq ($(BUILD_CAPTURE), yes)
endif # ifeq ($(BUILD_VIDEO_IO_KERNELS),yes)
