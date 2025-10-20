#pragma once
#include "trpc/server/trpc_server.h"
#include "file_transfer.trpc.pb.h"
#include "file_transfer.pb.h"

class FileTransferServiceImpl : public trpc::exp::FileTransferService {
public:
    // 重写 UploadFile 流式接口
    trpc::Status UploadFile(
            const trpc::ServerContextPtr& context,
            trpc::stream::StreamReader<trpc::exp::FileChunk>& stream,
            trpc::exp::UploadResponse* response) override;
};
