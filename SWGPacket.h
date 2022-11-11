/*
 * SWGPacket.h
 *
 *  Created on: Jul 30, 2021
 *      Author: dmounday
 */

#ifndef SWGPACKET_H_
#define SWGPACKET_H_
#include <iostream>
#include <string>

#include "BasicPacketProcessor.h"
#include "plog/Log.h"

namespace pentair_control {

static const uint8_t STATUS_RESP{1};
static const uint8_t SET_SANITIZER_RESP{0x12};
static const uint8_t INFO_RESP{0x03};
static const uint8_t GET_SANITIZER_RESP{0x16};

class SWGPacket : public BasicPacketProcessor {
  // IC-x0 SWG packet state for expected byte.
  const size_t SWG_DATA_LTH{23};
  enum class SWGState {
    SWG_UNDEFINED,
    SWG_STX,
    SWG_DEST,
    SWG_MSGID,
    SWG_DATA,
    SWG_DATA_DLE
  } swgState_;
  const std::size_t MSG_ID_POS{1};
  const std::size_t DEST_POS{0};
  std::vector<uint8_t> data_;
  std::string ByteToHex(uint8_t b);

 public:
  SWGPacket();
  virtual ~SWGPacket();
  const size_t DATA_MAX_SZ{24};
  inline uint8_t MsgID() const override { return data_[MSG_ID_POS]; }
  inline uint8_t Destination() const { return data_[DEST_POS]; }
  const std::vector<uint8_t>& MsgData() const override { return data_; }

  PacketState CapturePacket(uint8_t b) override;
  std::string PacketJSON() override;
};

} /* namespace pentair_control */

#endif /* SWGPACKET_H_ */
