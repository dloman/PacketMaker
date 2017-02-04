#include <iostream>
#include <PacketMaker.hpp>
#include <FunctionObjects/CurrentLength.hpp>
#include <boost/hana.hpp>

struct Header
{
  BOOST_HANA_DEFINE_STRUCT(
    Header,
    (uint8_t, Byte0),
    (uint8_t, Byte1)
    );
};


struct Packet
{
  BOOST_HANA_DEFINE_STRUCT(
    Packet,
    (Header, header),
    (uint8_t, Byte0),
    (uint8_t, Byte1),
    (uint8_t, Byte2),
    (pm::CurrentLength<uint8_t>, Length)
    );
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main(int argc, const char** argv)
{
  Header header{4,5};

  Packet packet{header,1,2,3};

  auto Bytes = pm::Encode(packet);

  Packet NewPacket = pm::Decode<Packet>(Bytes);

  std::cout
    << "NewPacket"
    << " Header.Byte0 = " << +NewPacket.header.Byte0
    << " Header.Byte1 = " << +NewPacket.header.Byte1
    << " Byte0 = " << +NewPacket.Byte0
    << " Byte1 = " << +NewPacket.Byte1
    << " Byte2 = " << +NewPacket.Byte2
    << " Length = " << +NewPacket.Length.GetCurrentLength() << std::endl;

  //BOOST_HANA_RUNTIME_CHECK(
  //boost::hana::members(NewPacket) == boost::hana::members(packet));

  return 0;
}

