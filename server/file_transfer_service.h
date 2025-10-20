#pragma once
#include <fstream>
#include <iostream>
#include "trpc/server/trpc_server.h"
#include "file_transfer.trpc.pb.h"

class FileTransferServiceImpl : public trpc::exp::FileTransferService {
public:
    // 流式上传
    trpc::Status UploadFile(
            const trpc::ServerContextPtr& context,
            trpc::stream::StreamReader<trpc::exp::FileChunk>& stream,
            trpc::exp::UploadResponse* response) override;
};
