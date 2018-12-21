# currently required to be set to yes
BUILD_CONFORMANCE_TEST?=yes
# currently required to be set to yes
BUILD_TUTORIAL?=yes
# currently required to be set to no
BUILD_BAM?=no
# currently required to be set to yes
BUILD_IGNORE_LIB_ORDER?=yes

# Build for SoC
BUILD_TARGET_MODE?=yes
# Build for x86 PC
BUILD_EMULATION_MODE?=yes
# valid values: X86 x86_64 all
BUILD_EMULATION_ARCH?=x86_64

# valid values: release debug all
PROFILE?=all

# Applied to target mode only
BUILD_LINUX_A72?=yes
# Applied to target mode only
BUILD_VLAB?=yes
