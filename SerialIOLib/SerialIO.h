#ifndef SerialIO_H
#define SerialIO_H
#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/basic_serial_port.hpp>
#include <boost/property_tree/ptree.hpp>

namespace serial_utils {
namespace pt=boost::property_tree;
namespace asio=boost::asio;

class SerialIO
{

public:
    SerialIO (const char *devName);
    /**
     * @brief Construct a new Serial Reader object. SerialIO
     * port and configuration passed in boost property tree node.
     * 
     * @param boost property tree node with following keys and default values: 
     *  port: device path
     *  baud: 9600
     *  bits: 8
     *  stopbits: 1
     *  parity: none
     */
    SerialIO( pt::ptree const& node);
    ~SerialIO
    ();
    /**
     * @brief Read one byte with timeout. Returns true if byte read, false
     * if timeout or error. A call to this function does not return until 
     * a byte is read or the read times out.
     * 
     * @param buf : pointer to buffer.
     * @param timeout : in milliseconds
     * @return bool 
     */
    bool ReadByte( uint8_t* buf, std::chrono::milliseconds timeout);
    /**
     * @brief Write buffer to port with timeout. Return true if buffer write
     * completed with error. False if error or timeout. A call to this function
     * does not return until a byte is read or the read times out.
     * 
     * @param buf : pointer to buffer. 
     * @param bufsz: size of buffer in bytes.
     * @param timeout: timeout in milliseconds. 
     * @return true: write success.
     * @return false: error. 
     */
    bool WriteBuf( const uint8_t* buf, size_t bufsz, std::chrono::milliseconds timeout);

private:
    asio::io_context ioc_;
    asio::serial_port port_;
    asio::steady_timer timer_;
    
    
};
}

#endif