
ifeq ($(TARGET_CPU), $(filter $(TARGET_CPU), X86 x86_64 R5F))

include $(PRELUDE)
TARGET      := vx_target_kernels_dmpac_sde
TARGETTYPE  := library
CSOURCES    := $(call all-c-files)
IDIRS       += $(CUSTOM_KERNEL_PATH)/hwa/include
IDIRS       += $(HOST_ROOT)/kernels/include
IDIRS       += $(J7_C_MODELS_PATH)/include
IDIRS       += $(VXLIB_PATH)/packages
DEFS        += TIOVX_SDE

ifeq ($(TARGET_CPU)$(BUILD_VLAB),R5Fyes)
DEFS += VLAB_HWA
endif

include $(FINALE)

endif
