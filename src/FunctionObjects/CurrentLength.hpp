#pragma once

namespace pm
{
  template<typename T>
  class CurrentLength : pm::FunctionObject
  {
    public:

      //------------------------------------------------------------------------
      //------------------------------------------------------------------------
      T Encode(const std::string& stream)
      {
        mCurrentLength = static_cast<T>(stream.size() + sizeof(mCurrentLength));

        return mCurrentLength;
      }

      //------------------------------------------------------------------------
      //------------------------------------------------------------------------
      void Decode(std::string& bytes)
      {
        std::memcpy(&mCurrentLength, bytes.data(), sizeof(mCurrentLength));

        if (bytes.size() > sizeof(mCurrentLength))
        {
          bytes = bytes.substr(sizeof(mCurrentLength));
        }
      }

      //------------------------------------------------------------------------
      //------------------------------------------------------------------------
      T GetCurrentLength()
      {
        return mCurrentLength;
      }

      //------------------------------------------------------------------------
      //------------------------------------------------------------------------
      bool operator = (const CurrentLength<T>& Rhs)
      {
        return mCurrentLength == Rhs.mCurrentLength;
      }

    private:

      T mCurrentLength;
  };
}
