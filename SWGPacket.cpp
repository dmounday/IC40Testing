/*
 * SWGPacket.cpp
 *
 *  Created on: Jul 30, 2021
 *      Author: dmounday
 */

#include "SWGPacket.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt=boost::property_tree;
namespace pentair_control {

SWGPacket::SWGPacket ():
    swgState_ {SWGState::SWG_UNDEFINED}
{
}

SWGPacket::~SWGPacket () {}

PacketState
SWGPacket::CapturePacket(uint8_t b){

  switch ( swgState_ ){
  case SWGState::SWG_UNDEFINED:
    data_.clear();
    if ( b == DLE ){
      swgState_ = SWGState::SWG_STX;
    } else {
      ++badData_;
      return PacketState::UNDEFINED;
    }
    break;
  case SWGState::SWG_STX:
    if ( b == STX ){
      swgState_ = SWGState::SWG_DEST;
    } else {
      ++badData_;
      swgState_= SWGState::SWG_UNDEFINED;
      return PacketState::UNDEFINED;
    }
    break;
  case SWGState::SWG_DEST:
    data_.push_back(b);
    swgState_ = SWGState::SWG_MSGID;
    break;
  case SWGState::SWG_MSGID:
    data_.push_back( b );
    swgState_ = SWGState::SWG_DATA;
    // now accumulate data until DLE ETX character sequence.
    break;
  case SWGState::SWG_DATA:
    data_.push_back(b);
    if ( b == DLE ) // check for DLE
      swgState_ = SWGState::SWG_DATA_DLE; // may be data or start of DLE ETX
    // else stay in data state
    if ( data_.size() > SWG_DATA_LTH){
      ++badData_;
      swgState_ = SWGState::SWG_UNDEFINED;
      return PacketState::UNDEFINED;
    }
    break;
  case SWGState::SWG_DATA_DLE:
    if ( b == ETX ) { // end of packet
      data_.pop_back(); // pop DLE
      uint8_t cksum = data_.back();
      data_.pop_back(); // remove ckeck sum
      uint8_t calc_ck_sum =DLE + STX; // check sum includes leading DLE/STX.
      for ( uint8_t x: data_) {
        calc_ck_sum += x;
      }
      if ( cksum == calc_ck_sum ){
        swgState_ = SWGState::SWG_UNDEFINED; // reset state for next msg.
        ++totalPackets_;
        return PacketState::SWG_COMPLETE;  // return complete SWG packet.
      } else {
        swgState_ = SWGState::SWG_UNDEFINED;
        ++ckSumErrors_;
        return PacketState::UNDEFINED;
      }
    } else {
      // just more data so go back to SWG_DATA state;
      data_.push_back(b);
      swgState_ = SWGState::SWG_DATA;
    }
    break;
  default:
    PLOG(plog::error)<< "SWG Packet state error";
    ++badData_;
    swgState_ = SWGState::SWG_UNDEFINED;
    return PacketState::UNDEFINED;
    break;
  }
  return PacketState::SWG_INCOMPLETE;
}

std::string SWGPacket::ByteToHex(uint8_t b)
{
  std::stringstream ss;
  ss << "0x" << std::setfill('0') << std::setw(2)
      << std::hex << static_cast<int>(b);
  return ss.str();
}

std::string SWGPacket::PacketJSON()
{
  std::stringstream ss;
  pt::ptree tree;
  if (MsgID() == INFO_RESP)
  {
    pt::ptree info;
    info.add("SaltPPM", data_[2]);
    std::string model;
    for (auto i = 3; i < data_.size(); ++i)
      model += data_[i];
    info.add("model", model);
    tree.push_back(std::make_pair("IC40Info", info));
  }
  else if (MsgID() == GET_SANITIZER_RESP)
  {
    pt::ptree resp;
    resp.add("status", ByteToHex(data_[2]));
    resp.add("percent", data_[3]);
    resp.add("byte2", ByteToHex(data_[2]));
    resp.add("byte6", ByteToHex(data_[4]));
    resp.add("byte7", ByteToHex(data_[5]));
    tree.push_back(std::make_pair("Sanitizer", resp));
  }
  else if (MsgID() == SET_SANITIZER_RESP)
  {
    pt::ptree resp;
    resp.add("salinity", data_[2]);
    resp.add("status", ByteToHex(data_[3]));
    resp.add("flow", data_[3] & 0x01 ? "No Flow" : "Flow OK");
    tree.push_back(std::make_pair("Sanitizer", resp));
  }
  else if (MsgID() == STATUS_RESP)
  {
    pt::ptree resp;
    resp.add("Status", ByteToHex(data_[2]));
    tree.push_back(std::make_pair("IC40.status", resp));
  }
  pt::write_json(ss, tree);
  return ss.str();
}

} /* namespace pentair_control */
