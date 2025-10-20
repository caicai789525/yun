#include "trpc/client/trpc_client.h"
#include "file_transfer.trpc.pb.h"
#include "file_transfer.pb.h"

int main() {
    // 初始化客户端并加载配置
    trpc::Client client("trpc_client_config.yaml");
    if (!client.Init()) {
        std::cerr << "客户端初始化失败" << std::endl;
        return -1;
    }

    // 创建服务代理
    trpc::exp::FileTransferService_Stub proxy(&client);

    // 构造请求
    trpc::exp::UploadRequest request;
    trpc::exp::UploadResponse response;

    // 示例：上传文件
    std::ifstream file("sample.txt", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件" << std::endl;
        return -1;
    }

    std::vector<char> buffer(4096);
    while (!file.eof()) {
        file.read(buffer.data(), buffer.size());
        std::streamsize bytes_read = file.gcount();
        if (bytes_read > 0) {
            trpc::exp::FileChunk chunk;
            chunk.set_data(buffer.data(), bytes_read);
            request.add_chunks()->CopyFrom(chunk);
        }
    }
    file.close();

    // 发送 RPC
    if (!proxy.UploadFile(nullptr, &request, &response)) {
        std::cerr << "RPC 调用失败" << std::endl;
        return -1;
    }

    std::cout << "上传完成, success=" << std::boolalpha << response.success()
              << ", message=\"" << response.message() << "\"" << std::endl;

    return 0;
}
