#include <boost/property_tree/json_parser.hpp>
#include <iostream>

#include "SWGCmd.h"
#include "SWGPacket.h"
#include "SerialIO.h"

namespace pt = boost::property_tree;
namespace su = serial_utils;
namespace pc = pentair_control;

bool verbose{true};

bool WriteRequestBytes(su::SerialIO &sio, pc::SWGCmd &req) {
  if (verbose) {
    std::cout << "IC40 Command Packet: ";
    for (auto b : req.SWGRequest())
      std::cout << std::hex << static_cast<int>(b) << " ";
    std::cout << '\n';
  }
  size_t lth = req.SWGRequest().size();
  if (sio.WriteBuf(req.SWGRequest().data(), lth, std::chrono::milliseconds(500)))
    return true;
  std::cout << "Write request failed or timedout\n";
  return false;
}

void DumpPacket(pc::SWGPacket &packet) {
  const std::vector<uint8_t> p = packet.MsgData();
  std::cout << "IC40 Response Data only:   ";
  for (auto i = 0; i < p.size(); ++i) {
    std::cout << std::hex << static_cast<int>(p[i]) << " " << std::flush;
  }
  std::cout << "\n";
}
bool GetResponsePacket(su::SerialIO &sio, pc::SWGPacket &packet) {
  int timeout{0};
  uint8_t b;
  pc::PacketState ps;
  while (timeout < 10) {
    if (sio.ReadByte(&b, std::chrono::milliseconds(500))) {
      ps = packet.CapturePacket(b);
      if (ps == pc::PacketState::SWG_COMPLETE) {
        if (verbose)
          DumpPacket(packet);
        return true;
      } else if (ps == pc::PacketState::SWG_INCOMPLETE)
        continue;
      else {
        if (verbose) {
          DumpPacket(packet);
          std::cout << " Packet Error.";
        }
        return false;
      }
    } else
      timeout++;
  }
  std::cout << "Waiting Response timeout\n" << std::flush;
  return false;
}

std::string GetIC40Info(su::SerialIO &sio) {
  pc::SWGPacket response;
  pc::SWGInfoReq req;
  if (WriteRequestBytes(sio, req))
    if (GetResponsePacket(sio, response)) {
      return response.PacketJSON();
    }
  return {"error"};
}

std::string GetIC40Status(su::SerialIO &sio) {
  pc::SWGPacket response;
  pc::SWGStatusReq req;
  if (WriteRequestBytes(sio, req))
    if (GetResponsePacket(sio, response)) {
      return response.PacketJSON();
    }
  return {"error"};
}

std::string GetSanitizerLevel(su::SerialIO &sio) {
  pc::SWGPacket response;
  pc::SWGGetSanitizerLevel req;
  if (WriteRequestBytes(sio, req))
    if (GetResponsePacket(sio, response)) {
      return response.PacketJSON();
    }
  return {"error"};
}

std::string SetIC40Sanitizer(su::SerialIO &sio, uint8_t level) {
  pc::SWGPacket response;
  pc::SWGSetSanitizer req(level);
  if (WriteRequestBytes(sio, req))
    if (GetResponsePacket(sio, response)) {
      return response.PacketJSON();
    }
  return {"error"};
}

void usage() {
  std::cerr << "Use: SerialUtils <device-path> {status|info|sanitizer <0..100>|percent} [-v]\n";
}

int main(int argc, char **argv) {
  pt::ptree tree;
  char *device;
  if (argc < 3) {
    usage();
    exit(-1);
  }
  device = argv[1];
  verbose = strcmp(argv[argc - 1], "-v") == 0;
  if ( verbose )
    std::cout << "All numerics are hexidecimal\n";
  tree.put("Device.name", std::string(device));
  tree.put("Device.baud", 9600);
  tree.put("Device.bits", 8);
  tree.put("Device.stopbits", 1);
  tree.put("Device.parity", "none");
  // Construct SerialIO
  pt::ptree node = tree.get_child("Device");

  // write_json("/dev/stdout", tree);
  // std::cout << "Device: " << node.get<std::string>("name") << '\n';
  try {
    su::SerialIO sio(tree.get_child("Device"));  // construct, open and config.
    if (strcmp(argv[2], "status") == 0)
      std::cout << GetIC40Status(sio) << '\n';
    else if (strcmp(argv[2], "info") == 0) {
      std::cout << GetIC40Info(sio) << '\n';
    } else if (strcmp(argv[2], "sanitizer") == 0) {
      if (argc > 3 && isdigit(*argv[3]))
        std::cout << SetIC40Sanitizer(sio, atoi(argv[3])) << '\n';
      else {
        std::cout << "No level percent specificed.\n";
        usage();
        exit(-1);
      }
    } else if (strcmp(argv[2], "percent") == 0) {
      std::cout << GetSanitizerLevel(sio) << '\n';
    } else {
      std::cout << "No request specified.\n";
      usage();
    }
  } catch (boost::system::system_error &ec) {
    std::cerr << "Device port open/config error: " << ec.what() << '\n';
  }
  std::cout << "Program exit\n";
}
