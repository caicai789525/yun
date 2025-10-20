#include <iostream>
#include <memory>

#include "trpc/server/trpc_server.h"
#include "file_transfer_service.h" // 自定义实现
#include "file_transfer.trpc.pb.h"
#include "file_transfer.pb.h"

int main(int argc, char** argv) {
    // 创建服务端
    trpc::TrpcServer server;

    // 读取配置
    trpc::ServerConfig config;
    if (!config.LoadFromFile("trpc_server_config.yaml")) {
        std::cerr << "加载服务端配置失败" << std::endl;
        return -1;
    }

    if (server.Initialize(config) != 0) {
        std::cerr << "初始化服务端失败" << std::endl;
        return -1;
    }

    // 注册服务
    auto service = std::make_shared<FileTransferServiceImpl>();
    if (server.RegisterService("trpc.exp.FileTransfer", service) != trpc::TrpcServer::kRetOk) {
        std::cerr << "注册服务失败" << std::endl;
        return -1;
    }

    std::cout << "服务端启动成功，等待客户端请求..." << std::endl;

    server.Start(); // 启动服务
    server.Wait();  // 阻塞等待

    return 0;
}
