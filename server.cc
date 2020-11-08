#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "packet.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using packet::Request;
using packet::Reply;
using packet::Logic;

class ServerImpl final : public Logic::Service {
  Status GetAns(ServerContext* context, const Request* request,
                  Reply* reply) override {
    auto mas = request->massive();
    auto masSize = request->massive_size();
    int ans = 0;
    int current = 0;
    int buff = 0; // [1, {current},..1, 0, {buff}, 0, 1..]
    bool findFirstNull = false;
    for(int i=0; i < masSize; i++){
      if(mas[i]){
        if(findFirstNull){
            buff++;
        }else{
            current++;
        }
        ans = std::max(current + buff, ans);
      }else{
        if(!findFirstNull){
          findFirstNull = true;
          buff = 0;
        }else{
          findFirstNull = false;
          current = 0;
        }
      }
    }
    reply->set_ansver(ans);
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  ServerImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  
  builder.RegisterService(&service);
  
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
