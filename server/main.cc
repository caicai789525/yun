#include "trpc/server/trpc_server.h"
#include "file_transfer_service.h"

int main(int argc, char** argv) {
    // 初始化服务端配置
    trpc::ServerConfig config("trpc_server_config.yaml");
    trpc::TrpcServer server(config);

    // 注册服务
    auto service = std::make_shared<FileTransferServiceImpl>();
    server.RegisterService("trpc.exp.FileTransfer", service);

    // 启动服务
    server.Run();

    return 0;
}
