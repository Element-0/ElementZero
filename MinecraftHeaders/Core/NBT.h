#pragma once
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../dll.h"

class IDataInput;
class IDataOutput;
class PrintStream;

class TagMemoryChunk {
public:
  size_t m_cap, m_size;
  std::unique_ptr<unsigned char[]> m_data;

  template <typename T> MCAPI T *alloc(unsigned long);
  MCAPI TagMemoryChunk copy() const;
  MCAPI bool operator!=(TagMemoryChunk const &) const;
  inline size_t size() const { return m_size; }
};

class MCAPI Tag {
public:
  virtual ~Tag();
  virtual void deleteChildren();
  virtual void write(IDataOutput &) const = 0;
  virtual void load(IDataInput &)         = 0;
  virtual std::string toString() const    = 0;
  virtual int getId() const               = 0;
  virtual bool equals(Tag const &) const;
  virtual void print(PrintStream &) const;
  virtual void print(std::string const &, PrintStream &) const;
  virtual std::unique_ptr<Tag> copy() const;
  virtual std::uint64_t hash() const;
};

class MCAPI EndTag : public Tag {
public:
  virtual ~EndTag() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 0
  virtual bool equals(Tag const &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};

class MCAPI ByteTag : public Tag {
public:
  unsigned char value;
  virtual ~ByteTag() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 1
  virtual bool equals(Tag const &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};

class MCAPI ShortTag : public Tag {
public:
  short value;
  virtual ~ShortTag() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 2
  virtual bool equals(Tag const &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};

class MCAPI IntTag : public Tag {
public:
  int32_t value;
  virtual ~IntTag() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 3
  virtual bool equals(Tag const &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};

class MCAPI Int64Tag : public Tag {
public:
  int64_t value;
  virtual ~Int64Tag() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 4
  virtual bool equals(Tag const &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};

class MCAPI FloatTag : public Tag {
public:
  float value;
  virtual ~FloatTag() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 5
  virtual bool equals(Tag const &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};

class MCAPI DoubleTag : public Tag {
public:
  double value;
  virtual ~DoubleTag() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 6
  virtual bool equals(Tag const &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};

class MCAPI ByteArrayTag : public Tag {
public:
  TagMemoryChunk value;
  virtual ~ByteArrayTag() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 7
  virtual bool equals(Tag const &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};

class MCAPI StringTag : public Tag {
public:
  std::string value;
  virtual ~StringTag() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 8
  virtual bool equals(Tag const &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};

class MCAPI ListTag : public Tag {
public:
  std::vector<std::unique_ptr<Tag>> value;
  virtual ~ListTag() override;
  virtual void deleteChildren() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 9
  virtual bool equals(Tag const &) const override;
  virtual void print(std::string const &, PrintStream &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};

class MCAPI CompoundTag : public Tag {
public:
  std::map<std::string, std::unique_ptr<Tag>> value;
  virtual ~CompoundTag() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 10
  virtual bool equals(Tag const &) const override;
  virtual void print(std::string const &, PrintStream &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};

class MCAPI IntArrayTag : public Tag {
public:
  TagMemoryChunk value;
  virtual ~IntArrayTag() override;
  virtual void write(IDataOutput &) const override;
  virtual void load(IDataInput &) override;
  virtual std::string toString() const override;
  virtual int getId() const override; // 11
  virtual bool equals(Tag const &) const override;
  virtual std::unique_ptr<Tag> copy() const override;
  virtual std::uint64_t hash() const override;
};