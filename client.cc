#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "packet.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using packet::Request;
using packet::Reply;
using packet::Logic;

class Client {
 public:
  Client(std::shared_ptr<Channel> channel)
      : stub_(Logic::NewStub(channel)) {}

  std::string GetAns() {
    Reply reply;

    ClientContext context;

    Status status = stub_->GetAns(&context, request, &reply);

    if (status.ok()) {
      return std::to_string(reply.ansver());
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

  void ReadMas(){
    std::string str;
    std::cout << "Enter massive ( example: 1010111101 )" << std::endl;
    std::cin >> str;
    for(auto ch: str){
      bool b = false;
      switch(ch){
        case '1':
          b = true;
          break;
        case '0':
          b = false;
          break;
      }
      request.add_massive(b);
    }
  }
 private:
  std::unique_ptr<Logic::Stub> stub_;
  Request request;
};

int main(int argc, char** argv) {
  std::string target_str = "localhost:50051";
  Client client(grpc::CreateChannel(
      target_str, grpc::InsecureChannelCredentials()));
  client.ReadMas();
  std::cout << "Ansver: " << client.GetAns() << std::endl;

  return 0;
}
