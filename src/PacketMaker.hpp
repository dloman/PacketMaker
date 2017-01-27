#pragma once
#include <boost/hana/fwd/members.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/at_key.hpp>
#include <boost/hana/keys.hpp>
#include <boost/type_index.hpp>
#include <cstring>
#include <functional>

namespace pm
{
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template<typename T>
  std::string Encode(T object)
  {
    std::string bytes;

    auto serialize = [](std::string& output, auto const& object)
    {
      boost::hana::for_each(
        boost::hana::members(object),
        [&](auto member)
        {
          output.append(reinterpret_cast<char*>(&member), sizeof(member));
        });
    };

    serialize(bytes, object);

    return bytes;
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template<typename T>
  void Decode(T& member, std::string& bytes, const auto& object)
  {
    std::memcpy(&member, bytes.data(), sizeof(member));

    if (bytes.size() > sizeof(member))
    {
      bytes = bytes.substr(sizeof(member));
    }
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template<typename T>
  T Decode(std::string bytes)
  {
    auto Decoder =
      [](std::string bytes, auto& object)
      {
        boost::hana::for_each(boost::hana::keys(object), [&](auto&& Key)
          {
            auto& Member = boost::hana::at_key(object, Key);

            Decode(Member, bytes, object);
          });
      };

    T object;

    Decoder(bytes, object);

    return object;
  }
}
