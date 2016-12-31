file(GLOB_RECURSE SRCS LIST_DIRECTORIES false
    "*.cpp"
    "*.h"
)

foreach(SRC IN ITEMS ${SRCS})
    get_filename_component(SRC_PATH "${SRC}" PATH)
    file(RELATIVE_PATH SRC_PATH_REL "${CMAKE_CURRENT_SOURCE_DIR}" "${SRC_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "${SRC_PATH_REL}")
    source_group("Source Files\\${GROUP_PATH}" FILES "${SRC}")
endforeach()

include_directories(../../externals/asio/asio/include)
include_directories(../../externals/cryptopp)

add_library(common STATIC ${SRCS})
target_link_libraries(common cryptopp-static)
