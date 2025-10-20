#pragma once
#include "trpc/server/stream_rpc_method_handler.h"
#include "file_transfer.trpc.pb.h"
#include "file_transfer.pb.h"

class FileTransferServiceImpl : public trpc::exp::FileTransfer {
public:
    trpc::Status UploadFile(const trpc::ServerContextPtr& context,
                            trpc::stream::StreamReader<trpc::exp::FileChunk>& stream,
                            trpc::exp::UploadResponse* response) override;
};
