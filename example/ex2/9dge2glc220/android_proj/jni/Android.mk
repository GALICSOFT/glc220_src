# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


###############################################################################
# path
DEV_SDK   = E:/_Document/_Afew_work/glc220
EABISYS   = D:/android-ndk-r8c/platforms/android-9/arch-arm/usr
STL_INC   = D:/android-ndk-r8c/sources/cxx-stl/gnu-libstdc++/4.6


###############################################################################
#module name
LOCAL_MODULE    := g_pack_app


###############################################################################
#source file

LOCAL_SRC_FILES := g_pack.cpp
LOCAL_SRC_FILES += galic_app.cpp
LOCAL_SRC_FILES += GpApp3.cpp GpApp2.cpp GpApp.cpp GameObject.cpp

###############################################################################
#library linking
LOCAL_LDLIBS    := -lgpan_t22 -lgpan_x22 -lgpan_c22 -lgpan_b22
LOCAL_LDLIBS    += -lgpan_png144 -lgpan_zlib125 -lgpan_ftf243 -lgpan_lua514
LOCAL_LDLIBS    += -lgpan_vorbis -lgpan_ogg -lgpan_alut11 -lgpan_al11
LOCAL_LDLIBS    += -lGLESv1_CM -llog -lstdc++

LOCAL_LDLIBS    += -lOpenSLES
#LOCAL_LDLIBS    += -lEGL -landroid

LOCAL_LDLIBS    += -L$(DEV_SDK)/lib


###############################################################################
#define, flag and header file include
LOCAL_CPPFLAGS  += -D__ANDROID__
LOCAL_CPPFLAGS  += -w -fno-common -march=armv5te -mtune=arm9tdmi
LOCAL_CPPFLAGS  += -mapcs -msoft-float -fno-short-enums
LOCAL_CPPFLAGS  += -fexceptions -frtti

LOCAL_CPPFLAGS  += -I./
LOCAL_CPPFLAGS  += -I./lsc
LOCAL_CPPFLAGS  += -I$(DEV_SDK)/include
LOCAL_CPPFLAGS  += -I$(STL_INC)/include
LOCAL_CPPFLAGS  += -I$(STL_INC)/libs/armeabi/include


include $(BUILD_SHARED_LIBRARY)
