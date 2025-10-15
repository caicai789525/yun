#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "trpc/client/trpc_client.h"
#include "trpc/client/client_context.h"

#include "proto/file_transfer.trpc.pb.h"

int main(int argc, char** argv) {
  // 初始化 tRPC 客户端框架，加载指定的配置文件
  if (trpc::TrpcClient::GetInstance()->Initialize("trpc_client_config.yaml") != 0) {
    std::cerr << "客户端框架初始化失败" << std::endl;
    return -1;
  }

  // 获取一个服务代理（Stub）用于发起RPC调用
  // "trpc.exp.FileTransfer" 是在 .proto 文件中定义的服务全名
  auto proxy = trpc::GetTrpcClient()->GetProxy<trpc::exp::FileTransferServiceProxy>("trpc.exp.FileTransfer");

  // 创建一个客户端上下文，用于控制单次RPC行为，例如超时
  trpc::ClientContextPtr context = trpc::MakeClientContext(proxy);
  context->SetTimeout(10000); // 设置10秒超时

  // ----------------------------------------------------------------
  // 核心逻辑开始
  // ----------------------------------------------------------------

  // 1. 发起 RPC 调用，获取流写入器
  trpc::exp::UploadResponse response;
  auto stream_writer = proxy->UploadFile(context, response);
  if (!stream_writer) {
    std::cerr << "创建流写入器失败, ret=" << context->GetStatus().GetFrameworkRetCode()
              << ", msg=" << context->GetStatus().ErrorMessage() << std::endl;
    return -1;
  }
  
  // 2. 准备要发送的文件
  const std::string filename = "sample.txt";
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "错误: 无法打开文件 " << filename << std::endl;
    return -1;
  }

  std::cout << "客户端开始发送文件: " << filename << std::endl;

  // 3. 循环读取文件并写入流中
  const size_t buffer_size = 4096; // 每次读取 4KB
  std::vector<char> buffer(buffer_size);

  while (!file.eof()) {
    file.read(buffer.data(), buffer_size);
    std::streamsize bytes_read = file.gcount(); // 获取实际读取的字节数

    if (bytes_read > 0) {
      trpc::exp::FileChunk chunk;
      chunk.set_data(buffer.data(), bytes_read);

      // 通过流写入器发送文件块
      // TO-DO: 学生需要理解这行代码的作用
      if (!stream_writer->Write(chunk)) {
        std::cerr << "写入流失败" << std::endl;
        break; // 写入失败则跳出循环
      }
    }
  }

  file.close();

  // 4. 发送结束信号
  // TO-DO: 学生需要理解为什么这行代码是必须的
  // 这是告诉服务端：“我的数据已经全部发完了”
  if (!stream_writer->WritesDone()) {
      std::cerr << "发送 WritesDone 信号失败" << std::endl;
      return -1;
  }
  std::cout << "客户端文件发送完毕" << std::endl;

  // 5. 等待服务端的最终响应
  // TO-DO: 学生需要理解 Finish 的作用
  // 程序会在这里阻塞，直到服务端处理完所有数据并返回一个 UploadResponse
  trpc::Status status = stream_writer->Finish();

  // ----------------------------------------------------------------
  // 核心逻辑结束
  // ----------------------------------------------------------------

  if (status.OK()) {
    std::cout << "客户端收到响应: success=" << std::boolalpha << response.success()
              << ", message=\"" << response.message() << "\"" << std::endl;
  } else {
    std::cerr << "RPC 调用失败: " << status.ErrorMessage() << std::endl;
  }
  
  // 销毁 tRPC 客户端框架
  trpc::TrpcClient::GetInstance()->Destroy();

  return 0;
}


