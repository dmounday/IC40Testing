/*
 * BasicPacket.h
 *
 *  Created on: Jul 30, 2021
 *      Author: dmounday
 */

#ifndef BASICPACKETPROCESSOR_H_
#define BASICPACKETPROCESSOR_H_
#include <cstdint>
#include <vector>
#include <string>

namespace pentair_control {

enum class PacketState {
    UNDEFINED,
    PENTAIR_INCOMPLETE,  // Intelliflow
    PENTAIR_COMPLETE,   // packet is comlete
    SWG_INCOMPLETE,
    SWG_COMPLETE,       // IC40 uses unique packet preemble
  };

const uint8_t DLE {0x10};
const uint8_t STX {0x02};
const uint8_t ETX {0x03};

class BasicPacketProcessor {
public:

  BasicPacketProcessor ();
  virtual ~BasicPacketProcessor ();
  virtual PacketState CapturePacket(uint8_t b) = 0;
  virtual uint8_t MsgID()const =0;
  virtual const std::vector<uint8_t>& MsgData()const = 0;
  /**
   * @brief Return std::string in JSON format of packet data.
   * 
   * @return std::string: JSON packet data.
   */
  virtual std::string PacketJSON()=0;


  inline long ChkSumErrors(){return ckSumErrors_;}
  inline long TotalPackets(){return totalPackets_;}
  inline long BadData(){return badData_;}

  long totalPackets_;  // Total Packets received.
  long ckSumErrors_;   // Count of packets failing check sum.
  long badData_;       // corrupt packets.



};

} /* namespace pentair_control */

#endif /* BASICPACKETPROCESSOR_H_ */
