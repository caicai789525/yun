#include "file_transfer_service.h"
#include <fstream>
#include <iostream>

trpc::Status FileTransferServiceImpl::UploadFile(
        const trpc::ServerContextPtr& context,
        trpc::stream::StreamReader<trpc::exp::FileChunk>& stream,
        trpc::exp::UploadResponse* response) {

    const std::string output_filename = "received_file.txt";
    std::ofstream output_file(output_filename, std::ios::binary);
    if (!output_file.is_open()) {
        response->set_success(false);
        response->set_message("无法创建文件");
        return trpc::Status(trpc::TrpcRetCode::TRPC_SERVER_SYSTEM_ERR, "Cannot create file");
    }

    trpc::exp::FileChunk chunk;
    while (stream.Read(&chunk)) {
        output_file.write(chunk.data().c_str(), chunk.data().size());
    }
    output_file.close();

    response->set_success(true);
    response->set_message("文件保存成功");
    return trpc::Status::OK;
}
