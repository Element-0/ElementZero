#pragma once

#include <string>
#include <gsl/string_span>
#include <string_view>

#include "../dll.h"

class IDataInput {
public:
  inline IDataInput() {}
  inline virtual ~IDataInput() {}
  virtual std::string readString()            = 0;
  virtual std::string readLongString()        = 0;
  virtual float readFloat()                   = 0;
  virtual double readDouble()                 = 0;
  virtual char readByte()                     = 0;
  virtual short readShort()                   = 0;
  virtual int readInt()                       = 0;
  virtual long long readLongLong()            = 0;
  virtual bool readBytes(void *, std::size_t) = 0;
  virtual std::size_t numBytesLeft() const    = 0;
};

class IDataOutput {
public:
  inline IDataOutput() {}
  inline virtual ~IDataOutput() {}
  virtual void writeString(gsl::cstring_span<>)      = 0;
  virtual void writeLongString(gsl::cstring_span<>)  = 0;
  virtual void writeFloat(float)                     = 0;
  virtual void writeDouble(double)                   = 0;
  virtual void writeByte(char)                       = 0;
  virtual void writeShort(short)                     = 0;
  virtual void writeInt(int)                         = 0;
  virtual void writeLongLong(long long)              = 0;
  virtual void writeBytes(void const *, std::size_t) = 0;
};

class BytesDataInput : public IDataInput {
public:
  inline BytesDataInput() {}
  inline virtual ~BytesDataInput() {}
  MCAPI virtual std::string readString();
  MCAPI virtual std::string readLongString();
  MCAPI virtual float readFloat();
  MCAPI virtual double readDouble();
  MCAPI virtual char readByte();
  MCAPI virtual short readShort();
  MCAPI virtual int readInt();
  MCAPI virtual long long readLongLong();
  virtual bool readBytes(void *, std::size_t) = 0;
  virtual std::size_t numBytesLeft() const    = 0;
};

class BytesDataOutput : public IDataOutput {
public:
  inline BytesDataOutput() {}
  inline virtual ~BytesDataOutput() {}
  MCAPI virtual void writeString(gsl::cstring_span<>);
  MCAPI virtual void writeLongString(gsl::cstring_span<>);
  MCAPI virtual void writeFloat(float);
  MCAPI virtual void writeDouble(double);
  MCAPI virtual void writeByte(char);
  MCAPI virtual void writeShort(short);
  MCAPI virtual void writeInt(int);
  MCAPI virtual void writeLongLong(long long);
  virtual void writeBytes(void const *, std::size_t) = 0;
};

class ReadOnlyBinaryStream;

class BinaryDataInput : public BytesDataInput {
public:
  ReadOnlyBinaryStream &stream;
  inline BinaryDataInput(ReadOnlyBinaryStream &stream) : stream(stream) {}
  MCAPI virtual ~BinaryDataInput();
  MCAPI virtual std::string readString();
  MCAPI virtual std::string readLongString();
  MCAPI virtual float readFloat();
  MCAPI virtual double readDouble();
  MCAPI virtual char readByte();
  MCAPI virtual short readShort();
  MCAPI virtual int readInt();
  MCAPI virtual long long readLongLong();
  MCAPI virtual bool readBytes(void *, std::size_t);
  MCAPI virtual std::size_t numBytesLeft() const;
};

class BinaryStream;

class BinaryDataOutput : public BytesDataOutput {
public:
  BinaryStream &stream;
  inline BinaryDataOutput(BinaryStream &stream) : stream(stream) {}
  MCAPI virtual ~BinaryDataOutput();
  MCAPI virtual void writeString(gsl::cstring_span<>);
  MCAPI virtual void writeLongString(gsl::cstring_span<>);
  MCAPI virtual void writeFloat(float);
  MCAPI virtual void writeDouble(double);
  MCAPI virtual void writeByte(char);
  MCAPI virtual void writeShort(short);
  MCAPI virtual void writeInt(int);
  MCAPI virtual void writeLongLong(long long);
  MCAPI virtual void writeBytes(void const *, std::size_t);
};

class StringByteInput : public BytesDataInput {
public:
  int start, end;            // 8, 12
  std::string const &buffer; // 16
  inline StringByteInput(std::string const &buffer, int start, int end) : start(start), end(end), buffer(buffer) {}
  MCAPI virtual ~StringByteInput();
  MCAPI virtual bool readBytes(void *, std::size_t);
  MCAPI virtual std::size_t numBytesLeft() const;
};

class StringByteOutput : public BytesDataOutput {
public:
  std::string &str;
  inline StringByteOutput(std::string &str) : str(str) {}
  MCAPI virtual ~StringByteOutput();
  MCAPI virtual void writeBytes(void const *, std::size_t);
};

class BigEndianStringByteInput : public StringByteInput {
public:
  inline BigEndianStringByteInput(std::string const &str, int start, int end) : StringByteInput(str, start, end) {}
  MCAPI virtual ~BigEndianStringByteInput();
  MCAPI virtual float readFloat();
  MCAPI virtual double readDouble();
  MCAPI virtual char readByte();
  MCAPI virtual short readShort();
  MCAPI virtual int readInt();
  MCAPI virtual long long readLongLong();
  MCAPI virtual bool readBytes(void *, std::size_t);
  MCAPI bool readBigEndinanBytes(void *, std::size_t);
};

class BigEndianStringByteOutput : public StringByteOutput {
public:
  inline BigEndianStringByteOutput(std::string &str) : StringByteOutput(str) {}
  MCAPI virtual ~BigEndianStringByteOutput();
  MCAPI virtual void writeFloat(float);
  MCAPI virtual void writeDouble(double);
  MCAPI virtual void writeByte(char);
  MCAPI virtual void writeShort(short);
  MCAPI virtual void writeInt(int);
  MCAPI virtual void writeLongLong(long long);
  MCAPI virtual void writeReverseBytes(void const *, std::size_t);
};

namespace RakNet {
class BitStream;
}

class RakDataInput : public BytesDataInput {
public:
  RakNet::BitStream &stream;
  inline RakDataInput(RakNet::BitStream &stream) : stream(stream) {}
  MCAPI virtual ~RakDataInput();
  MCAPI virtual bool readBytes(void *, std::size_t);
  MCAPI virtual std::size_t numBytesLeft() const;
};

class RakDataOutput : BytesDataOutput {
public:
  RakNet::BitStream &stream;
  inline RakDataOutput(RakNet::BitStream &stream) : stream(stream) {}
  MCAPI virtual ~RakDataOutput();
  MCAPI virtual void writeBytes(void const *, std::size_t);
};