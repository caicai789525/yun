#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "trpc/common/trpc_app.h"
#include "trpc/client/client_context.h"

#include "proto/file_transfer.trpc.pb.h"

int main(int argc, char** argv) {
    // ---------------- 初始化 tRPC ----------------
    if (!trpc::Init("trpc_client_config.yaml")) {
        std::cerr << "客户端框架初始化失败" << std::endl;
        return -1;
    }

    // ---------------- 创建服务代理 ----------------
    auto proxy = trpc::MakeRefCounted<trpc::exp::FileTransferServiceProxy>("trpc.exp.FileTransfer");

    // ---------------- 创建客户端上下文 ----------------
    auto context = trpc::CreateClientContext(proxy);
    context->SetTimeout(10000); // 设置10秒超时

    // ---------------- 核心逻辑 ----------------
    trpc::exp::UploadResponse response;
    auto stream_writer = proxy->UploadFile(context, &response); // ✅ 注意传指针

    if (!stream_writer) {
        std::cerr << "创建流写入器失败" << std::endl;
        trpc::Destroy();
        return -1;
    }

    // 打开文件
    const std::string filename = "sample.txt";
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "错误: 无法打开文件 " << filename << std::endl;
        trpc::Destroy();
        return -1;
    }

    std::cout << "客户端开始发送文件: " << filename << std::endl;

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
        std::cerr << "发送 WritesDone 信号失败" << std::endl;
        trpc::Destroy();
        return -1;
    }

    std::cout << "客户端文件发送完毕" << std::endl;

    // 等待服务端最终响应
    trpc::Status status = stream_writer->Finish();

    if (status.OK()) {
        std::cout << "客户端收到响应: success=" << std::boolalpha << response.success()
                  << ", message=\"" << response.message() << "\"" << std::endl;
    } else {
        std::cerr << "RPC 调用失败: " << status.ErrorMessage() << std::endl;
    }

    // 销毁 tRPC 框架
    trpc::Destroy();

    return 0;
}
