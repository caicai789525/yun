#include <iostream>
#include "trpc/server/trpc_server.h"
#include "server/file_transfer_service.h"

int main(int argc, char** argv) {
  // 创建一个 tRPC Server 对象
  trpc::TrpcServer server;

  // 初始化 Server，加载指定的配置文件
  if (server.Initialize("trpc_server_config.yaml") != 0) {
      std::cerr << "服务端框架初始化失败" << std::endl;
      return -1;
  }

  std::cout << "服务端框架初始化成功，正在监听中..." << std::endl;

  // 注册我们自己实现的服务
  server.RegisterService("trpc.exp.FileTransfer", std::make_shared<FileTransferServiceImpl>());

  std::cout << "服务[" << "trpc.exp.FileTransfer" << "]注册成功" << std::endl;

  // 启动服务器，开始监听端口并处理请求
  server.Serve();

  return 0;
}

