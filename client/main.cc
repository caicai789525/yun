#include <fstream>
#include <iostream>
#include <vector>
#include <memory>

#include "trpc/client/trpc_client.h"
#include "file_transfer.trpc.pb.h"
#include "file_transfer.pb.h"

int main() {
    // 初始化客户端
    trpc::TrpcClient client;
    if (!client.Init("trpc_client_config.yaml")) {
        std::cerr << "初始化客户端失败" << std::endl;
        return -1;
    }

    // 创建服务代理
    auto proxy = trpc::MakeRefCounted<trpc::exp::FileTransferServiceProxy>();

    // 创建客户端上下文
    auto context = client.MakeContext();
    context->SetTimeout(10000); // 10秒超时

    // 准备响应对象
    trpc::exp::UploadResponse response;

    // 创建流式写入器
    auto stream_writer = proxy->UploadFile(context, &response);
    if (!stream_writer) {
        std::cerr << "创建流写入器失败" << std::endl;
        return -1;
    }

    // 打开文件
    const std::string filename = "sample.txt";
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return -1;
    }

    std::cout << "开始上传文件: " << filename << std::endl;

    const size_t buffer_size = 4096;
    std::vector<char> buffer(buffer_size);

    while (!file.eof()) {
        file.read(buffer.data(), buffer_size);
        std::streamsize bytes_read = file.gcount();

        if (bytes_read > 0) {
            trpc::exp::FileChunk chunk;
            chunk.set_data(buffer.data(), bytes_read);

            if (!stream_writer->Write(chunk)) {
                std::cerr << "写入流失败" << std::endl;
                break;
            }
        }
    }

    file.close();

    if (!stream_writer->WritesDone()) {
        std::cerr << "发送 WritesDone 失败" << std::endl;
        return -1;
    }

    // 等待服务端响应
    trpc::Status status = stream_writer->Finish();
    if (status.OK()) {
        std::cout << "上传完成, success=" << std::boolalpha << response.success()
                  << ", message=\"" << response.message() << "\"" << std::endl;
    } else {
        std::cerr << "RPC 调用失败: " << status.ErrorMessage() << std::endl;
    }

    return 0;
}
