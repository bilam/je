
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := jconsole-nopie

LOCAL_LDLIBS := -ldl -llog

ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
  LOCAL_CFLAGS := -fPIC -Os -fvisibility=hidden -fwrapv -Werror -fno-strict-aliasing -DREADLINE -DUSE_LINENOISE -march=armv8-a
  LOCAL_ARM_NEON := true
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
  LOCAL_CFLAGS := -DARMEABI_V7A -fPIC -Os -fvisibility=hidden -fwrapv -Werror -fno-strict-aliasing -DREADLINE -DUSE_LINENOISE -mfloat-abi=softfp -mfpu=vfpv3-d16 -march=armv7-a
  LOCAL_ARM_MODE := arm
endif
LOCAL_LDFLAGS += -Wl,-z,noexecstack

LOCAL_SRC_FILES :=  jconsole.c jeload.c linenoise.c

include $(BUILD_EXECUTABLE)

