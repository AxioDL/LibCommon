#-------------------------------------------------
#
# Project created by QtCreator 2015-12-13T15:27:18
#
#-------------------------------------------------

QT -= core gui
DEFINES += LIBCOMMON

CONFIG += staticlib
TEMPLATE = lib

BUILD_DIR = $$PWD/../Build
EXTERNALS_DIR = $$PWD/../Externals
DESTDIR = $$BUILD_DIR

win32: {
    QMAKE_CXXFLAGS += /WX \  # Treat warnings as errors
        /wd4267 \        # Disable C4267: conversion from 'size_t' to 'type', possible loss of data
        /wd4100 \        # Disable C4100: unreferenced formal parameter
        /wd4101 \        # Disable C4101: unreferenced local variable
        /wd4189          # Disable C4189: local variable is initialized but not referenced

    QMAKE_CXXFLAGS_WARN_ON -= -w34100 -w34189 # Override C4100 and C4189 being set to w3 in Qt's default .qmake.conf file
}

unix {
    target.path = /usr/lib
    QMAKE_CXXFLAGS += /WX
    INSTALLS += target
}

CONFIG (debug, debug|release) {
    # Debug Config
    OBJECTS_DIR = $$BUILD_DIR/debug
    TARGET = LibCommond
}

CONFIG (release, debug|release) {
    # Release Config
    OBJECTS_DIR = $$BUILD_DIR/release
    TARGET = LibCommon
}

# Include Paths
INCLUDEPATH += $$EXTERNALS_DIR/CodeGen/include \
               $$EXTERNALS_DIR/tinyxml2

# Header Files
HEADERS += \
    Common/BasicTypes.h \
    Common/Common.h \
    Common/CColor.h \
    Common/CFourCC.h \
    Common/CTimer.h \
    Common/EGame.h \
    Common/EKeyInputs.h \
    Common/EMouseInputs.h \
    Common/FileIO.h \
    Common/FileUtil.h \
    Common/Flags.h \
    Common/LinkedList.h \
    Common/Log.h \
    Common/Macros.h \
    Common/NBasics.h \
    Common/TString.h \
    Common/CScopedTimer.h \
    Common/CAssetID.h \
    Common/Hash/CCRC32.h \
    Common/Hash/CFNV1A.h \
    Common/Serialization/IArchive.h \
    Common/Serialization/CXMLReader.h \
    Common/Serialization/CXMLWriter.h \
    Common/Serialization/CBasicBinaryReader.h \
    Common/Serialization/CBasicBinaryWriter.h \
    Common/Serialization/CBinaryReader.h \
    Common/Serialization/CBinaryWriter.h \
    Common/Serialization/CSerialVersion.h \
    Common/Serialization/Binary.h \
    Common/Serialization/XML.h \
    Common/FileIO/CBitStreamInWrapper.h \
    Common/FileIO/CFileInStream.h \
    Common/FileIO/CFileOutStream.h \
    Common/FileIO/CMemoryInStream.h \
    Common/FileIO/CMemoryOutStream.h \
    Common/FileIO/CVectorOutStream.h \
    Common/FileIO/IInputStream.h \
    Common/FileIO/IOutputStream.h \
    Common/FileIO/IOUtil.h \
    Common/FileIO/CFileLock.h \
    Common/Math/CAABox.h \
    Common/Math/CFrustumPlanes.h \
    Common/Math/CMatrix4f.h \
    Common/Math/CPlane.h \
    Common/Math/CQuaternion.h \
    Common/Math/CRay.h \
    Common/Math/CTransform4f.h \
    Common/Math/CVector2f.h \
    Common/Math/CVector2i.h \
    Common/Math/CVector3f.h \
    Common/Math/CVector4f.h \
    Common/Math/ETransformSpace.h \
    Common/Math/MathUtil.h

# Source Files
SOURCES += \
    Common/CAssetID.cpp \
    Common/CColor.cpp \
    Common/CTimer.cpp \
    Common/EGame.cpp \
    Common/FileUtil.cpp \
    Common/Log.cpp \
    Common/TString.cpp \
    Common/FileIO/CFileInStream.cpp \
    Common/FileIO/CFileOutStream.cpp \
    Common/FileIO/CMemoryInStream.cpp \
    Common/FileIO/CMemoryOutStream.cpp \
    Common/FileIO/CVectorOutStream.cpp \
    Common/FileIO/IOUtil.cpp \
    Common/FileIO/IInputStream.cpp \
    Common/FileIO/IOutputStream.cpp \
    Common/FileIO/CBitStreamInWrapper.cpp \
    Common/Hash/CCRC32.cpp \
    Common/Serialization/CSerialVersion.cpp \
    Common/Math/CAABox.cpp \
    Common/Math/CFrustumPlanes.cpp \
    Common/Math/CMatrix4f.cpp \
    Common/Math/CQuaternion.cpp \
    Common/Math/CTransform4f.cpp \
    Common/Math/CVector2f.cpp \
    Common/Math/CVector2i.cpp \
    Common/Math/CVector3f.cpp \
    Common/Math/CVector4f.cpp \
    Common/Math/MathUtil.cpp

# Codegen
CODEGEN_DIR = $$EXTERNALS_DIR/CodeGen
CODEGEN_OUT_PATH = $$BUILD_DIR/CodeGen/auto_codegen.cpp
CODEGEN_SRC_PATH = $$PWD/Common
include($$EXTERNALS_DIR/CodeGen/codegen.pri)

# Library Sources
SOURCES += $$EXTERNALS_DIR/tinyxml2/tinyxml2.cpp
