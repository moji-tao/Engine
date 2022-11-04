# Install script for directory: D:/dev/Engine/engine/3rd/Assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.2.5-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/dev/Engine/engine/3rd/Assimp/lib/Debug/assimp-vc143-mtd.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/dev/Engine/engine/3rd/Assimp/lib/Release/assimp-vc143-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/dev/Engine/engine/3rd/Assimp/lib/MinSizeRel/assimp-vc143-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/dev/Engine/engine/3rd/Assimp/lib/RelWithDebInfo/assimp-vc143-mt.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.2.5" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/dev/Engine/engine/3rd/Assimp/bin/Debug/assimp-vc143-mtd.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/dev/Engine/engine/3rd/Assimp/bin/Release/assimp-vc143-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/dev/Engine/engine/3rd/Assimp/bin/MinSizeRel/assimp-vc143-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/dev/Engine/engine/3rd/Assimp/bin/RelWithDebInfo/assimp-vc143-mt.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/anim.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/aabb.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/ai_assert.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/camera.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/color4.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/color4.inl"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/config.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/ColladaMetaData.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/commonMetaData.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/defs.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/cfileio.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/light.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/material.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/material.inl"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/matrix3x3.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/matrix3x3.inl"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/matrix4x4.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/matrix4x4.inl"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/mesh.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/ObjMaterial.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/pbrmaterial.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/GltfMaterial.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/postprocess.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/quaternion.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/quaternion.inl"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/scene.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/metadata.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/texture.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/types.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/vector2.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/vector2.inl"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/vector3.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/vector3.inl"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/version.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/cimport.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/importerdesc.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Importer.hpp"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/DefaultLogger.hpp"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/ProgressHandler.hpp"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/IOStream.hpp"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/IOSystem.hpp"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Logger.hpp"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/LogStream.hpp"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/NullLogger.hpp"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/cexport.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Exporter.hpp"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/DefaultIOStream.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/DefaultIOSystem.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/SceneCombiner.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/fast_atof.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/qnan.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/BaseImporter.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Hash.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/MemoryIOWrapper.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/ParsingUtils.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/StreamReader.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/StreamWriter.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/StringComparison.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/StringUtils.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/SGSpatialSort.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/GenericProperty.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/SpatialSort.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/SmallVector.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/SmoothingGroups.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/SmoothingGroups.inl"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/StandardShapes.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/RemoveComments.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Subdivision.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Vertex.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/LineSplitter.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/TinyFormatter.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Profiler.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/LogAux.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Bitmap.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/XMLTools.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/IOStreamBuffer.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/CreateAnimMesh.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/XmlParser.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/BlobIOSystem.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/MathFunctions.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Exceptional.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/ByteSwapper.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Compiler/pushpack1.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Compiler/poppack1.h"
    "D:/dev/Engine/engine/3rd/Assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/dev/Engine/engine/3rd/Assimp/code/Debug/assimp-vc143-mtd.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/dev/Engine/engine/3rd/Assimp/code/RelWithDebInfo/assimp-vc143-mt.pdb")
  endif()
endif()

