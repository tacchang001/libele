cmake_minimum_required(VERSION 2.8)

set (CMAKE_VERBOSE_MAKEFILE 1)

set (TARGET_NAME ele)
project (${TARGET_NAME} C)
set (serial "1.0.0")
set (soserial "1")

set (CMAKE_BUILD_TYPE Debug)

file (GLOB SRCS src/*.c src/*/*.c)
set (INCLUDES
    include
)
include_directories (${INCLUDES})

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

# 以下の警告を部分的に抑制する
#    #pragma GCC diagnostic ignored "-Wcast-qual"
# 部分的に抑制していた警告を元に戻す
#    #pragma GCC diagnostic warning "-Wcast-qual"

# 厳格に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
# -Wunused-parameter は警告のまま、その他の警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-parameter")
# ある局所変数が別の局所変数を隠す場合に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow")
# 関数呼び出しがマッチしない型へのキャストに警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wbad-function-cast")
# voidポインタや関数へのポインタで加算と減算に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")
# ポインタの指す型に要求されるアライメントの値が大きくなるようなキャストに警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align")
# 文字列定数のアドレスをconstしていされないchar*型へのポインタコピーに警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wwrite-strings")
# プロトタイプの有無で引数の型変換が変わる際に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wconversion")
# signedとunsignedで比較結果が変わる場合に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsign-compare")
# 構造体や共用体が戻り値に使用されていたら警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Waggregate-return")
# 引数や戻り値の無いプロトタイプ宣言に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
# プロトタイプ宣言の無い関数に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-prototypes")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations")
# 重複した宣言に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wredundant-decls")
# 間違っているprintf系の書式に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat=2")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format-y2k")
# 配列の初期化方法に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-braces")
# 標準のC言語仕様に規定されていない計算順序に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsequence-point")
# 未使用に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-function")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-label")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-parameter")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-variable")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-value")
# 関数の引数がNULLではまずい場合に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wnonnull")
# switch構文にdefaultが無い場合に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-default")
#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-enum")
# 結果が不定となるメンバへのアクセスに警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-aliasing")

# コンパイラが最適化を断念するほど大きなソースに警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wdisabled-optimization")
# 浮動小数点数の等号比較に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wfloat-equal")
# 効果が無い構造体へのパックに警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpacked")
# 4バイト境界に配置できずパディングした構造体に警告
#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpadded")
# 実行されないコードに警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunreachable-code")
# 初期化されていない自身での初期化に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Winit-self")
# 構造体の初期化エラーに警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-field-initializers")
# includeディレクトリが無い場合に警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-include-dirs")
# 古い関数定義スタイルに警告
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wold-style-definition")
# 
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Woverlength-strings")
# ランタイム関数の堅固なバージョンを使う
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=2")

#set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CMAKE_C_FLAGS}")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")

#set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3")
# どの関数についてもスタック破壊検出コードを生成させる
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fstack-protector-all")

add_library (${TARGET_NAME} SHARED 
    ${SRCS}
)
set_target_properties (${TARGET_NAME} PROPERTIES 
    VERSION ${serial} SOVERSION ${soserial}
)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    enable_testing ()
    set (CMAKE_C_FLAGS "")
    include_directories (tests)
    set (test_libraries ${libraries} pcunit)
    macro (add_unit_test args)
        set (target ${ARGV0})
        add_executable (${ARGV})
        target_link_libraries (${target} ${test_libraries} -lpthread)
        add_test (
            ${target} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target}
            "--log_level=warning"
        )
    endmacro (add_unit_test)
    include (unittests.cmake)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif()

