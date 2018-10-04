
ifeq ($(TARGET_CPU), $(filter $(TARGET_CPU), X86 x86_64 R5F))

include $(PRELUDE)
TARGET      := vx_target_kernels_vpac_viss
TARGETTYPE  := library
CSOURCES    := vx_kernels_hwa_target.c
IDIRS       += $(CUSTOM_KERNEL_PATH)/hwa/include
IDIRS       += $(HOST_ROOT)/kernels/include
IDIRS       += $(TDA4X_C_MODELS_PATH)/include
IDIRS       += $(VXLIB_PATH)/packages

ifeq ($(TARGET_CPU)$(BUILD_VLAB),R5Fyes)
DEFS += VLAB_HWA
CSOURCES    += vx_vpac_viss_vlab_target.c
else
CSOURCES    += vx_vpac_viss_target.c
endif

ifeq ($(HOST_COMPILER),GCC_LINUX)
CFLAGS += -Wno-unused-result
endif

include $(FINALE)

endif
