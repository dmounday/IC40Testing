#include "SerialIO.h"
namespace serial_utils {

SerialIO::SerialIO(pt::ptree const& node):
        ioc_{1}, port_{ioc_, node.get<std::string>("name")},
        timer_{ioc_}
{
  port_.set_option (boost::asio::serial_port_base::baud_rate (
      node.get<int>("baud", 9600)));
  port_.set_option (boost::asio::serial_port_base::character_size(
      node.get<unsigned int>("bits",8)));
  int sbits = node.get<int>("stopbits", 1);
  port_.set_option (
      asio::serial_port_base::stop_bits ( sbits ==1?
          asio::serial_port_base::stop_bits::one:
          asio::serial_port_base::stop_bits::two));
  std::string parity = node.get<std::string>("parity", "none");
  if ( parity == "none")
    port_.set_option (boost::asio::serial_port_base::parity (
          asio::serial_port_base::parity::none));
  else if ( parity == "even")
    port_.set_option (boost::asio::serial_port_base::parity (
              asio::serial_port_base::parity::even));
  else
    port_.set_option (boost::asio::serial_port_base::parity (
              asio::serial_port_base::parity::odd));
  port_.set_option (
      boost::asio::serial_port_base::flow_control (
          asio::serial_port_base::flow_control::none));
}

SerialIO::~SerialIO()
{}

bool SerialIO::ReadByte(uint8_t* buf, std::chrono::milliseconds timeout)
{
  bool readOK = false;
  // restart required between calls to io_context.run().
  ioc_.restart();
  // set next timeout duration
  timer_.expires_after( timeout );
  // start read operation.
  port_.async_read_some(asio::buffer(buf, 1),
    [this, &readOK] (const boost::system::error_code& ec, std::size_t xfered)
        {
          if ( ec ) {
            // probably a timeout. Maybe a serial port error.
            // Should sort this out and close and reopen port???
              readOK = false;
            return;
          } 
          readOK = xfered == 1;
          timer_.cancel();
          }
  );
  timer_.async_wait(
    [this, &readOK](const boost::system::error_code &ec)
    {
      if ( ec ){
        // timer was canceled by read operation completion.
        return;
      }
      // timer expired cancel read opertion.
      port_.cancel(); 
      readOK = false;
    }
  );
  // wait on async operation to complete.
  ioc_.run();
  return readOK;
}

bool SerialIO::WriteBuf(const uint8_t *buf, size_t bufsz, std::chrono::milliseconds timeout)
{
  bool writeOK = false;
  // restart required between calls to io_context.run().
  ioc_.restart();
  // set next timeout duration
  timer_.expires_after(timeout);
  // start read operation.
  async_write(port_, asio::buffer(buf, bufsz),
        [this, &writeOK, &bufsz](const boost::system::error_code &ec, std::size_t xfered)
        {
          if (ec)
          {
            // probably a timeout. Maybe a serial port error.
            // Should sort this out and close and reopen port???
            writeOK = false;
            return;
          }
          writeOK = xfered == bufsz;
          //std::cout << "async_write lambda: " << bufsz << std::endl;
          timer_.cancel();
        });
  timer_.async_wait(
      [this, &writeOK](const boost::system::error_code &ec)
      {
        if (ec)
        {
          // timer was canceled by write operation completion.
          return;
        }
        // timer expired cancel write opertion.
        port_.cancel();
        writeOK = false;
      });
  // wait on async operation to complete.
  ioc_.run();
  return writeOK;
}

} // namespace serial_utils