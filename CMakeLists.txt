cmake_minimum_required(VERSION 2.8.12)
project(Bugle)

if (NOT WIN32 OR MSYS OR CYGWIN)
  find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config DOC
               "llvm-config executable")
  if(LLVM_CONFIG_EXECUTABLE STREQUAL "LLVM_CONFIG_EXECUTABLE-NOTFOUND")
    message(FATAL_ERROR "llvm-config could not be found!")
  endif()

  execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --cxxflags
    OUTPUT_VARIABLE LLVM_CXXFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  set(LLVM_CXXFLAGS "${LLVM_CXXFLAGS} -fno-exceptions -fno-rtti")

  execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs bitreader ipo target transformutils
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --system-libs
    OUTPUT_VARIABLE LLVM_SYSTEM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if (NOT  "${LLVM_SYSTEM_LIBS}" STREQUAL "")
    set(LLVM_LIBS "${LLVM_LIBS} ${LLVM_SYSTEM_LIBS}")
  endif()

  execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags
    OUTPUT_VARIABLE LLVM_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --bindir
    OUTPUT_VARIABLE LLVM_BINDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --src-root
    OUTPUT_VARIABLE LLVM_SRC
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --obj-root
    OUTPUT_VARIABLE LLVM_BUILD
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  set(PLUGIN_CXXFLAGS "${LLVM_CXXFLAGS} -I\"${LLVM_SRC}/tools/clang/include\" -I\"${LLVM_BUILD}/tools/clang/include\"")

  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLUGIN_CXXFLAGS "${PLUGIN_CXXFLAGS} -Wno-strict-aliasing")
  endif()

  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_MACOSX_RPATH ON)
    set(PLUGIN_LDFLAGS "-Wl,-undefined,dynamic_lookup")
  endif()

else()
  set(LLVM_SRC "" CACHE PATH "LLVM source directory")
  set(LLVM_BUILD "" CACHE PATH "LLVM build directory")
  set(LLVM_BUILD_TYPE "" CACHE STRING "LLVM build type")

  if (NOT EXISTS "${LLVM_SRC}/include/llvm")
    message(FATAL_ERROR "Invalid LLVM source directory: ${LLVM_SRC}")
  endif()

  set(LLVM_LIBDIR "${LLVM_BUILD}/${LLVM_BUILD_TYPE}/lib")
  if (NOT EXISTS "${LLVM_LIBDIR}")
    message(FATAL_ERROR "Invalid LLVM build directory: ${LLVM_BUILD}")
  endif()

  set(LLVM_CXXFLAGS "\"/I${LLVM_SRC}/include\" \"/I${LLVM_BUILD}/include\" -D_SCL_SECURE_NO_WARNINGS -wd4141 -wd4146 -wd4244 -wd4291 -wd4624 -wd4800")
  set(LLVM_LDFLAGS "")
  set(LLVM_LIBS "${LLVM_LIBDIR}/LLVMipo.lib" "${LLVM_LIBDIR}/LLVMObjCARCOpts.lib" "${LLVM_LIBDIR}/LLVMVectorize.lib" "${LLVM_LIBDIR}/LLVMScalarOpts.lib" "${LLVM_LIBDIR}/LLVMInstCombine.lib" "${LLVM_LIBDIR}/LLVMTransformUtils.lib" "${LLVM_LIBDIR}/LLVMAnalysis.lib" "${LLVM_LIBDIR}/LLVMTarget.lib" "${LLVM_LIBDIR}/LLVMMC.lib" "${LLVM_LIBDIR}/LLVMObject.lib" "${LLVM_LIBDIR}/LLVMBitReader.lib" "${LLVM_LIBDIR}/LLVMCore.lib" "${LLVM_LIBDIR}/LLVMBinaryFormat.lib" "${LLVM_LIBDIR}/LLVMSupport.lib")

endif()

include_directories(include)

add_library(bugleBoogie STATIC
  lib/Boogie/BPLExprWriter.cpp
  lib/Boogie/BPLFunctionWriter.cpp
  lib/Boogie/BPLModuleWriter.cpp
  lib/Boogie/BVIntegerRepresentation.cpp
  lib/Boogie/Expr.cpp
  lib/Boogie/Ident.cpp
  lib/Boogie/MathIntegerRepresentation.cpp
  lib/Boogie/SourceLocWriter.cpp
  lib/Boogie/Stmt.cpp
  include/bugle/BPLExprWriter.h
  include/bugle/BPLFunctionWriter.h
  include/bugle/BPLModuleWriter.h
  include/bugle/BasicBlock.h
  include/bugle/Casting.h
  include/bugle/Expr.h
  include/bugle/Function.h
  include/bugle/GlobalArray.h
  include/bugle/Ident.h
  include/bugle/IntegerRepresentation.h
  include/bugle/Module.h
  include/bugle/OwningPtrVector.h
  include/bugle/RaceInstrumenter.h
  include/bugle/Ref.h
  include/bugle/SourceLoc.h
  include/bugle/SourceLocWriter.h
  include/bugle/SpecificationInfo.h
  include/bugle/Stmt.h
  include/bugle/Type.h
  include/bugle/Var.h
)

add_library(buglePreprocessing STATIC
  lib/Preprocessing/ArgumentPromotionPass.cpp
  lib/Preprocessing/ArgumentRenamePass.cpp
  lib/Preprocessing/CycleDetectPass.cpp
  lib/Preprocessing/FreshArrayPass.cpp
  lib/Preprocessing/InlinePass.cpp
  lib/Preprocessing/RestrictDetectPass.cpp
  lib/Preprocessing/SimpleInternalizePass.cpp
  lib/Preprocessing/StructSimplificationPass.cpp
  lib/Preprocessing/Vector3SimplificationPass.cpp
  include/bugle/Preprocessing/ArgumentPromotionPass.h
  include/bugle/Preprocessing/ArgumentRenamePass.h
  include/bugle/Preprocessing/CycleDetectPass.h
  include/bugle/Preprocessing/FreshArrayPass.h
  include/bugle/Preprocessing/InlinePass.h
  include/bugle/Preprocessing/RestrictDetectPass.h
  include/bugle/Preprocessing/SimpleInternalizePass.h
  include/bugle/Preprocessing/StructSimplificationPass.h
  include/bugle/Preprocessing/Vector3SimplificationPass.h
)

add_library(bugleTranslator STATIC
  lib/Translator/TranslateModule.cpp
  lib/Translator/TranslateFunction.cpp
  include/bugle/Translator/TranslateModule.h
  include/bugle/Translator/TranslateFunction.h
)

add_library(bugleTransform STATIC
  lib/Transform/SimplifyStmt.cpp
  include/bugle/Transform/SimplifyStmt.h
)

add_library(bugleUtil STATIC
  lib/Util/ErrorReporter.cpp
  lib/Util/UniqueNameSet.cpp
  include/bugle/util/ErrorReporter.h
  include/bugle/util/UniqueNameSet.h
  include/bugle/util/Functional.h
)

add_executable(bugle
  tools/bugle.cpp
)

set_target_properties(bugle bugleBoogie buglePreprocessing
                      bugleTransform bugleTranslator bugleUtil
    PROPERTIES COMPILE_FLAGS "${LLVM_CXXFLAGS}")

target_link_libraries(bugle
  buglePreprocessing
  bugleTranslator
  bugleTransform
  bugleBoogie
  bugleUtil
  ${LLVM_LIBS} ${LLVM_LDFLAGS}
)

if(NOT WIN32 OR MSYS OR CYGWIN)

add_library(bugleInlineCheckPlugin SHARED
  lib/InlineCheckPlugin/InlineCheckPlugin.cpp
)

set_target_properties(bugleInlineCheckPlugin
    PROPERTIES COMPILE_FLAGS "${PLUGIN_CXXFLAGS}")

target_link_libraries(bugleInlineCheckPlugin
  ${PLUGIN_LIBS} ${PLUGIN_LDFLAGS}
)

endif()
