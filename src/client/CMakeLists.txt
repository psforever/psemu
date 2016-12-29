set(SUB_DIRS
)

FOREACH (sub_dir ${SUB_DIRS})
    file(GLOB DIR_SRCS
        "${sub_dir}/*.cpp"
        "${sub_dir}/*.h"
    )
    source_group("Source Files\\${sub_dir}" FILES ${DIR_SRCS})
    list(APPEND SRCS ${DIR_SRCS})
ENDFOREACH ()

file(GLOB DIR_SRCS
    "*.cpp"
    "*.h"
)
source_group("Source Files" FILES ${DIR_SRCS})
list(APPEND SRCS ${DIR_SRCS})

include_directories(../../externals/asio/asio/include)
include_directories(../../externals/cryptopp)

add_executable(client ${SRCS})
target_link_libraries(client common cryptopp-static)
