#pragma once
#include <FunctionObjects/FunctionObject.hpp>
#include <FunctionObjects/DelayedEvaluation.hpp>
#include <boost/hana/fwd/members.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/at_key.hpp>
#include <boost/hana/keys.hpp>
#include <boost/type_index.hpp>
#include <cstring>
#include <functional>
#include <type_traits>
#include <vector>

namespace pm
{
  namespace hana = boost::hana;
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template<typename T>
  std::string Encode(
    const T& object,
    std::vector<std::function<void(std::string&)>>& delayedFunctionObjects)
  {
    std::string bytes;

    auto serialize = [&delayedFunctionObjects](std::string& output, auto const& object)
    {
      auto isHanaStruct =
        hana::is_valid([](auto&& object) ->
          std::enable_if_t<hana::Struct<std::decay_t<decltype(object)>>::value> {});

      auto isDelayedEvaluation =
        hana::is_valid([](auto&& object) ->
          std::enable_if_t<std::is_base_of<
            pm::DelayedEvaluation,
            std::decay_t<decltype(object)>>::value> {});

      auto isFunctionObject =
        hana::is_valid([](auto&& object) ->
          std::enable_if_t<std::is_base_of<
            pm::FunctionObject,
            std::decay_t<decltype(object)>>::value> {});

      hana::for_each(
        hana::members(object),
        [&](auto member)
        {
          hana::if_(isHanaStruct(member),
            [&](auto& member)
            {
              auto temp = pm::Encode(member, delayedFunctionObjects);

              output += temp;
            },
            [&](auto& member)
            {
              hana::if_(isFunctionObject(member),
                [&output, &isDelayedEvaluation, &delayedFunctionObjects](auto& member)
                {
                  hana::if_(isDelayedEvaluation(member),
                    [&output, &delayedFunctionObjects](auto& member)
                    {
                      auto index = output.size();

                      //append placeholder data
                      decltype(member.Encode(std::string{})) temp;

                      output.append(reinterpret_cast<char*>(&temp), sizeof(temp));

                      delayedFunctionObjects.push_back(
                        [index, &member] (std::string& output)
                        {
                          auto temp = member.Encode(output);

                          output.replace(
                            index,
                            sizeof(temp),
                            reinterpret_cast<char*>(&temp),
                            sizeof(temp));
                        });
                    },
                    [&output](auto& member)
                    {
                      auto temp =  member.Encode(output);

                      output.append(reinterpret_cast<char*>(&temp), sizeof(temp));
                    }
                    )(member);
                },
                [&output](auto& member)
                {
                  output.append(reinterpret_cast<char*>(&member), sizeof(member));
                }
                )(member);
            }
            )(member);
        });
    };

    serialize(bytes, object);

    for (const auto& delayedFunctionObject : delayedFunctionObjects)
    {
      delayedFunctionObject(bytes);
    }

    return bytes;
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  template<typename T>
  std::string Encode(const T& object)
  {
    static std::vector<std::function<void(std::string&)>> delayedFunctionObjects;

    return Encode(object, delayedFunctionObjects);
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
  T Decode(std::string& bytes)
  {
    auto isHanaStruct =
      hana::is_valid([](auto&& object) ->
        std::enable_if_t<hana::Struct<std::decay_t<decltype(object)>>::value> {});

    auto Decoder =
      [&isHanaStruct](std::string& bytes, auto& object)
      {
        hana::for_each(hana::keys(object), [&](auto&& key)
          {
            auto& member = hana::at_key(object, key);

            hana::if_(isHanaStruct(member),
              [&bytes](auto& member)
              {
                member = pm::Decode<std::decay_t<decltype(member)>>(bytes);
              },
              [&bytes](auto& member)
              {
                Decode(member, bytes);
              }
              )(member);
          });
      };

    T object;

    Decoder(bytes, object);

    return object;
  }
}
