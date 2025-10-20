#include <fstream>
#include <iostream>
#include <vector>

#include "trpc/client/trpc_client.h"
#include "file_transfer.trpc.pb.h"
#include "file_transfer.pb.h"

int main() {
    // 初始化客户端
    trpc::TrpcClient client("trpc_client_config.yaml");

    // 创建服务代理
    auto proxy = std::make_shared<trpc::exp::FileTransferServiceProxy>(&client);

    // 打开文件
    const std::string filename = "sample.txt";
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return -1;
    }

    // 创建请求和响应
    trpc::exp::UploadRequest request;
    trpc::exp::UploadResponse response;

    const size_t buffer_size = 4096;
    std::vector<char> buffer(buffer_size);

    while (!file.eof()) {
        file.read(buffer.data(), buffer_size);
        std::streamsize bytes_read = file.gcount();
        if (bytes_read > 0) {
            trpc::exp::FileChunk* chunk = request.add_chunks();
            chunk->set_data(buffer.data(), bytes_read);
        }
    }

    file.close();

    // 调用 UploadFile RPC
    if (!proxy->UploadFile(nullptr, &request, &response)) {
        std::cerr << "RPC 调用失败" << std::endl;
        return -1;
    }

    std::cout << "上传完成, success=" << std::boolalpha << response.success()
              << ", message=\"" << response.message() << "\"" << std::endl;

    return 0;
}
