/*
 * BasicPacket.cpp
 *
 *  Created on: Jul 30, 2021
 *      Author: dmounday
 */

#include "BasicPacketProcessor.h"

namespace pentair_control {

BasicPacketProcessor::BasicPacketProcessor ():
  totalPackets_{0L}, ckSumErrors_{0L},
  badData_{0L}
{}

BasicPacketProcessor::~BasicPacketProcessor () {
}

} /* namespace pentair_control */
