/*
 * SWGCmd.h
 *
 *  Created on: Aug 1, 2021
 *      Author: dmounday
 */

#ifndef SWGCMD_H_
#define SWGCMD_H_
#include <cstdint>
#include <vector>

namespace pentair_control {
static const uint8_t STATUS_REQ{0x00};
static const uint8_t SET_SANITIZER{0x11};
static const uint8_t GET_SANITIZER{0x15};
static const uint8_t INFO_REQ{0x14};

class SWGCmd {
  const uint8_t DLE {0x10};
  const uint8_t STX {0x02};
  const uint8_t ETX {0x03};
public:
  static const uint8_t SWG_ID {0x50};
  static const uint8_t CONTROLLER_ID {0x00};

  SWGCmd (std::uint8_t);
  virtual ~SWGCmd ();

  void PushCkSum();
  void EndPacket();
  inline const std::vector<uint8_t>& SWGRequest()const{return packet_;}

  std::vector<uint8_t> packet_;
};


class SWGStatusReq : public SWGCmd {
public:
  SWGStatusReq ();
  virtual ~SWGStatusReq ();
};

class SWGSetSanitizer: public SWGCmd {
  uint8_t req_;
public:
  SWGSetSanitizer (uint8_t level);
  ~SWGSetSanitizer();
};

class SWGInfoReq: public SWGCmd {
public:
  SWGInfoReq();
  ~SWGInfoReq();
};

class SWGGetSanitizerLevel: public SWGCmd {
  public:
  SWGGetSanitizerLevel();
  ~SWGGetSanitizerLevel();
};

} /* namespace pentair_control */

#endif /* SWGCMD_H_ */
