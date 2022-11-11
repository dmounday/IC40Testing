/*
 * SWGCmd.cpp
 *
 *  Created on: Aug 1, 2021
 *      Author: dmounday
 */

#include "SWGCmd.h"

namespace pentair_control {

SWGCmd::SWGCmd(std::uint8_t cmd) : packet_{DLE, STX, SWG_ID, cmd} {}

SWGCmd::~SWGCmd() {
}

void SWGCmd::PushCkSum() {
  unsigned cs{0};
  for (auto b : packet_) {
    cs += b;
  }
  packet_.push_back(cs);
}
void SWGCmd::EndPacket() {
  PushCkSum();
  packet_.push_back(DLE);
  packet_.push_back(ETX);
}

SWGStatusReq::SWGStatusReq() : SWGCmd(STATUS_REQ) {
  packet_.push_back(0x00);
  EndPacket();
}

SWGStatusReq::~SWGStatusReq() {
}

SWGSetSanitizer::SWGSetSanitizer(uint8_t level) : SWGCmd(SET_SANITIZER) {
  packet_.push_back(level);
  EndPacket();
}
SWGSetSanitizer::~SWGSetSanitizer() {
}

SWGInfoReq::SWGInfoReq() : SWGCmd(INFO_REQ) {
  packet_.push_back(0x02);  //
  EndPacket();
}
SWGInfoReq::~SWGInfoReq() {}

SWGGetSanitizerLevel::SWGGetSanitizerLevel() : SWGCmd(GET_SANITIZER) {
  packet_.push_back(0x00);
  EndPacket();
}
SWGGetSanitizerLevel::~SWGGetSanitizerLevel() {}
} /* namespace pentair_control */
