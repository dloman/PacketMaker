#include <iostream>
#include <PacketMaker.hpp>
#include <boost/hana.hpp>


struct Packet
{
  BOOST_HANA_DEFINE_STRUCT(
    Packet,
    (uint8_t, Byte0),
    (uint8_t, Byte1),
    (uint8_t, Byte2)
    );
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main(int argc, const char** argv)
{
  Packet packet{1,2,3};

  auto Bytes = pm::Encode(packet);

  Packet NewPacket = pm::Decode<Packet>(Bytes);

  BOOST_HANA_RUNTIME_CHECK(
    boost::hana::members(NewPacket) == boost::hana::members(packet));

  return 0;
}

