###########################################
# this part compiles your game to static android library
# modify include paths and source files suitable for your game!
###########################################

# kajak3d-root directory can be set or overwritten with this variable in here.
KAJAK3D_ROOT := /home/niko/kajak3d

#
# LOCAL_PATH points to location of source files. All sourcefile paths are relative to LOCAL_PATH. 
# $(call my-dir) means location of this (Android.mk) file.
#
LOCAL_PATH := $(call my-dir)/../

include $(CLEAR_VARS)

LOCAL_MODULE := RainbowShuttle

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include/ \
            $(KAJAK3D_ROOT)/framework/include \
            $(KAJAK3D_ROOT)/scenemanager/include \
            $(KAJAK3D_ROOT)/core/include \
            $(KAJAK3D_ROOT)/HAL/include \
            $(KAJAK3D_ROOT)/render/include \
	$(KAJAK3D_ROOT)/kajak2d/include/ \
            $(KAJAK3D_ROOT)/external/slmath/include


#
# List your game sourcefiles in LOCAL_SRC_FILES 
#

LOCAL_SRC_FILES := RainbowShuttle.cpp Shuttle.cpp

LOCAL_CFLAGS=-fexceptions -frtti

LOCAL_ARM_MODE := arm
include $(BUILD_STATIC_LIBRARY)


####################################################################################################
# second library. kajak3d wrapper. modify local ld libs to include whatever custom libraries you used (if any).
####################################################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := Kajak3DGameWrapper
LOCAL_SRC_FILES := Kajak3DGameWrapper.cpp
LOCAL_C_INCLUDES := \
    $(KAJAK3D_ROOT)/core/include/ \
    $(KAJAK3D_ROOT)/framework/include/ \
    $(KAJAK3D_ROOT)/render/include/ \
    $(KAJAK3D_ROOT)/HAL/include/ \
    $(KAJAK3D_ROOT)/kajak2d/include/ \
    $(KAJAK3D_ROOT)/external/slmath/include \
    $(KAJAK3D_ROOT)/external/tinyxml/include \
    $(KAJAK3D_ROOT)/external/lua-5.1.4/include/lua \
    $(KAJAK3D_ROOT)/external/lua-5.1.4/include \
    $(KAJAK3D_ROOT)/external/swigwin-1.3.40/include \
    $(KAJAK3D_ROOT)/external/libvorbis-1.3.1/include \
    $(KAJAK3D_ROOT)/external/libogg-1.2.0/include

LOCAL_LDLIBS := -L$(KAJAK3D_ROOT)/lib/Android/armeabi \
	-L$(KAJAK3D_ROOT)/lib/Android/armeabi/gamelib \
        -L$(LOCAL_PATH) \
        -lGLESv1_CM -ldl -llog -lz -lkajak2d -lode -lframework -lenet -llua -lrender -ltinyxml -lslmath -lpng -lcore -lzip -lHAL

LOCAL_STATIC_LIBRARIES := RainbowShuttle

LOCAL_ARM_MODE := arm
include $(BUILD_SHARED_LIBRARY)

