#pragma once
#include "FunctionObject.hpp"
#include <boost/hana/fwd/members.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/at_key.hpp>
#include <boost/hana/keys.hpp>
#include <boost/type_index.hpp>
#include <cstring>
#include <functional>
#include <type_traits>

namespace pm
{
  namespace hana = boost::hana;
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template<typename T>
  std::string Encode(const T& object)
  {
    std::string bytes;

    auto serialize = [](std::string& output, auto const& object)
    {
      auto hasEncode =
        hana::is_valid([](auto&& object) -> decltype(object.Encode()){});

      auto isChild =
        hana::is_valid([](auto&& object) ->
          std::enable_if_t<std::is_base_of<pm::FunctionObject, decltype(object)>::value> {});

      hana::for_each(
        hana::members(object),
        [&](auto member)
        {
          hana::if_(hasEncode(member) && isChild(member),
            [&output](auto& member)
            {
              output += member.Encode();
            },
            [&output](auto& member)
            {
              output.append(reinterpret_cast<char*>(&member), sizeof(member));
            }
            )(member);
        });
    };

    serialize(bytes, object);

    return bytes;
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template<typename T>
  void DecodeImpl(T& member, std::string& bytes)
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
  void Decode(T& member, std::string& bytes)
  {
    auto hasDecode =
      hana::is_valid([&bytes](auto&& member) ->decltype(member.Decode(bytes)){});

    return hana::if_(hasDecode(member),
      [&bytes](auto& member)
      {
        std::cout << "decode sucess" << std::endl;
        member.Decode(bytes);
      },
      [&bytes](auto& member)
      {
        DecodeImpl(member, bytes);
      }
      )(member);
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template<typename T>
  T Decode(std::string bytes)
  {
    auto Decoder =
      [](std::string bytes, auto& object)
      {
        hana::for_each(hana::keys(object), [&](auto&& key)
          {
            auto& Member = hana::at_key(object, key);

            Decode(Member, bytes);
          });
      };

    T object;

    Decoder(bytes, object);

    return object;
  }
}
