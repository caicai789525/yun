#include "file_transfer_service.h"
#include <fstream>
#include <iostream>

trpc::Status FileTransferServiceImpl::UploadFile(
        const trpc::ServerContextPtr& context,
        trpc::stream::StreamReader<trpc::exp::FileChunk>& stream,
        trpc::exp::UploadResponse* response) {

    const std::string filename = "uploaded_sample.txt";
    std::ofstream out_file(filename, std::ios::binary);
    if (!out_file.is_open()) {
        response->set_success(false);
        response->set_message("无法创建文件");
        return trpc::Status::OK();
    }

    trpc::exp::FileChunk chunk;
    trpc::Status s;
    while ((s = stream.Read(&chunk)).OK()) {
        out_file.write(chunk.data().data(), chunk.data().size());
    }

    out_file.close();

    if (s.OK() || s.IsEof()) {
        response->set_success(true);
        response->set_message("上传成功");
    } else {
        response->set_success(false);
        response->set_message("上传失败: " + s.ErrorMessage());
    }

    return trpc::Status::OK();
}
