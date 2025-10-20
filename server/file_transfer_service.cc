#include "file_transfer_service.h"

trpc::Status FileTransferServiceImpl::UploadFile(
        const trpc::ServerContextPtr& context,
        trpc::stream::StreamReader<trpc::exp::FileChunk>& stream,
        trpc::exp::UploadResponse* response) {

    std::ofstream out("uploaded.txt", std::ios::binary);
    trpc::exp::FileChunk chunk;

    while (true) {
        trpc::Status s = stream.Read(&chunk);
        if (!s.OK()) break;
        out.write(chunk.data().data(), chunk.data().size());
    }

    out.close();
    response->set_success(true);
    response->set_message("上传成功");

    return trpc::Status(); // 默认 OK
}
