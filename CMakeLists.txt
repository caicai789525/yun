cmake_minimum_required(VERSION 3.16)
project(trpc_file_transfer_experiment CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------- tRPC 安装路径 ----------------
set(TRPC_DIR "/usr/local/trpc-cpp/trpc")

# 查找 Protobuf
find_package(Protobuf REQUIRED)

# 查找 tRPC
find_package(trpc REQUIRED PATHS ${TRPC_DIR})

# ---------------- Protobuf 代码生成 ----------------
file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto")
set(PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/gen_proto")
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

set(GENERATED_FILES)
foreach(proto_file ${PROTO_FILES})
    get_filename_component(proto_name ${proto_file} NAME_WE)
    set(proto_cc "${PROTO_GEN_DIR}/${proto_name}.pb.cc")
    set(proto_h  "${PROTO_GEN_DIR}/${proto_name}.pb.h")
    set(trpc_cc "${PROTO_GEN_DIR}/${proto_name}.trpc.pb.cc")
    set(trpc_h  "${PROTO_GEN_DIR}/${proto_name}.trpc.pb.h")

    add_custom_command(
            OUTPUT ${proto_cc} ${proto_h} ${trpc_cc} ${trpc_h}
            COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
            --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
            --cpp_out=${PROTO_GEN_DIR}
            --trpc_out=${PROTO_GEN_DIR}
            ${proto_file}
            DEPENDS ${proto_file}
            COMMENT "Generating C++ and tRPC code for ${proto_name}"
    )

    list(APPEND GENERATED_FILES ${proto_cc} ${proto_h} ${trpc_cc} ${trpc_h})
endforeach()

add_custom_target(generate_proto ALL DEPENDS ${GENERATED_FILES})
include_directories(${PROTO_GEN_DIR})

# ---------------- 服务端 ----------------
add_executable(file_transfer_server
        server/main.cc
        server/file_transfer_service.cc
        server/file_transfer_service.h
)
add_dependencies(file_transfer_server generate_proto)
target_include_directories(file_transfer_server PRIVATE ${PROTO_GEN_DIR})
target_link_libraries(file_transfer_server PRIVATE
        trpc::server
        trpc::client
        protobuf::libprotobuf
        pthread
)

# ---------------- 客户端 ----------------
add_executable(file_transfer_client
        client/main.cc
)
add_dependencies(file_transfer_client generate_proto)
target_include_directories(file_transfer_client PRIVATE ${PROTO_GEN_DIR})
target_link_libraries(file_transfer_client PRIVATE
        trpc::client
        protobuf::libprotobuf
        pthread
)

# ---------------- 安装 ----------------
install(TARGETS file_transfer_server file_transfer_client RUNTIME DESTINATION bin)
install(FILES
        server/trpc_server_config.yaml
        client/trpc_client_config.yaml
        sample.txt
        DESTINATION bin
)
