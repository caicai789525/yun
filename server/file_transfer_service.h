#pragma once

#include <string>
#include "trpc/server/stream_rpc_method_handler.h"
#include "proto/file_transfer.trpc.pb.h"

// FileTransferServiceImpl 继承自 tRPC 生成的 FileTransfer 服务基类
class FileTransferServiceImpl : public trpc::exp::FileTransfer {
public:
    // 重写 UploadFile，实现文件接收逻辑
    trpc::Status UploadFile(const trpc::ServerContextPtr& context,
                            trpc::stream::StreamReader<trpc::exp::FileChunk>& stream,
                            trpc::exp::UploadResponse* response) override;
};
