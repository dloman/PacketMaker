#pragma once

namespace pm
{
  template<typename T>
  class TotalLength : pm::DelayedEvaluation
  {
    public:

      //------------------------------------------------------------------------
      //------------------------------------------------------------------------
      T Encode(const std::string& stream)
      {
        mTotalLength = static_cast<T>(stream.size());

        return mTotalLength;
      }

      //------------------------------------------------------------------------
      //------------------------------------------------------------------------
      void Decode(std::string& bytes)
      {
        std::memcpy(&mTotalLength, bytes.data(), sizeof(mTotalLength));

        if (bytes.size() > sizeof(mTotalLength))
        {
          bytes = bytes.substr(sizeof(mTotalLength));
        }
      }

      //------------------------------------------------------------------------
      //------------------------------------------------------------------------
      T GetTotalLength()
      {
        return mTotalLength;
      }

      //------------------------------------------------------------------------
      //------------------------------------------------------------------------
      bool operator == (const TotalLength<T>& Rhs)
      {
        return mTotalLength == Rhs.mTotalLength;
      }

    private:

      T mTotalLength;
  };
}
