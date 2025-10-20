#include <trpc/server/trpc_server.h>
#include "file_transfer_service.h"

int main() {
    trpc::TrpcServer server;

    FileTransferServiceImpl service;
    server.AddService(&service); // 注册服务

    const std::string server_address = "0.0.0.0:50051";

    if (server.Start(server_address) != 0) {
        std::cerr << "启动服务失败" << std::endl;
        return -1;
    }

    std::cout << "服务端已启动，监听 " << server_address << std::endl;

    server.Wait(); // 阻塞等待
    return 0;
}
