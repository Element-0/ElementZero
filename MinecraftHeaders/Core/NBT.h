#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../dll.h"

class IDataInput;
class IDataOutput;
class PrintStream;

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
  virtual void setName(std::string const &);
  virtual std::string getName() const;
  virtual std::unique_ptr<Tag> copy() const;
};

class MCAPI EndTag : public Tag {
public:
  virtual ~EndTag();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual std::unique_ptr<Tag> copy() const;
};

class MCAPI DoubleTag : public Tag {
public:
  double value;
  virtual ~DoubleTag();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual std::unique_ptr<Tag> copy() const;
};

class TagMemoryChunk {
public:
  size_t m_cap, m_size;
  std::unique_ptr<unsigned char[]> m_data;

  template <typename T> MCAPI T *alloc(unsigned long);
  MCAPI TagMemoryChunk copy() const;
  MCAPI bool operator!=(TagMemoryChunk const &) const;
  inline size_t size() const { return m_size; }
};

class MCAPI ByteArrayTag : public Tag {
public:
  TagMemoryChunk value;
  virtual ~ByteArrayTag();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual std::unique_ptr<Tag> copy() const;
};

class MCAPI CompoundTag : public Tag {
public:
  std::map<std::string, std::unique_ptr<Tag>> value;
  virtual ~CompoundTag();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual void print(std::string const &, PrintStream &) const;
  virtual std::unique_ptr<Tag> copy() const;
};

class MCAPI ListTag : public Tag {
public:
  std::vector<std::unique_ptr<Tag>> value;
  size_t unk;
  void add(std::unique_ptr<Tag>);
  virtual ~ListTag();
  virtual void deleteChildren();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual void print(std::string const &, PrintStream &) const;
  virtual std::unique_ptr<Tag> copy() const;
};

class MCAPI FloatTag : public Tag {
public:
  float value;
  virtual ~FloatTag();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual std::unique_ptr<Tag> copy() const;
};

class MCAPI ByteTag : public Tag {
public:
  unsigned char value;
  virtual ~ByteTag();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual std::unique_ptr<Tag> copy() const;
};

class MCAPI ShortTag : public Tag {
public:
  short value;
  virtual ~ShortTag();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual std::unique_ptr<Tag> copy() const;
};

class MCAPI Int64Tag : public Tag {
public:
  int64_t value;
  virtual ~Int64Tag();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual std::unique_ptr<Tag> copy() const;
};

class MCAPI IntArrayTag : public Tag {
public:
  TagMemoryChunk value;
  virtual ~IntArrayTag();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual std::unique_ptr<Tag> copy() const;
};

class MCAPI IntTag : public Tag {
public:
  int32_t value;
  virtual ~IntTag();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual std::unique_ptr<Tag> copy() const;
};

class MCAPI StringTag : public Tag {
public:
  std::string value;
  virtual ~StringTag();
  virtual void write(IDataOutput &) const;
  virtual void load(IDataInput &);
  virtual std::string toString() const;
  virtual int getId() const;
  virtual bool equals(Tag const &) const;
  virtual std::unique_ptr<Tag> copy() const;
};