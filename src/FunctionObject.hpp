#pragma once

namespace pm
{
  class FunctionObject
  {
    virtual std::string Encode() = 0;

    virtual void Decode(std::string& bytes) = 0;
  };
}


