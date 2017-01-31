#include <iostream>
#include <PacketMaker.hpp>
#include <boost/hana.hpp>

struct Thingy : public pm::FunctionObject
{
  BOOST_HANA_DEFINE_STRUCT(
    Thingy
    );

  std::string Encode() override
  {
    std::cout << "W00t Encode" << std::endl;

    return "stuff";
  }

  void Decode(std::string& bytes) override
  {
    std::cout << "W00t Decode" << std::endl;
  }
};

struct Packet
{
  BOOST_HANA_DEFINE_STRUCT(
    Packet,
    (uint8_t, Byte0),
    (uint8_t, Byte1),
    (uint8_t, Byte2),
    (Thingy, thingy)
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

