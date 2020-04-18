#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "NBT.h"
#include "../Math/Vec3.h"
#include "../dll.h"

enum class DataItemType : unsigned char { BYTE, SHORT, INT, FLOAT, STRING, NBT, POS, LONG, VEC3 };

namespace DataTypeMap {

template <typename T> DataItemType typeFor;

template <> inline DataItemType typeFor<char>        = DataItemType::BYTE;
template <> inline DataItemType typeFor<short>       = DataItemType::SHORT;
template <> inline DataItemType typeFor<int>         = DataItemType::INT;
template <> inline DataItemType typeFor<float>       = DataItemType::FLOAT;
template <> inline DataItemType typeFor<std::string> = DataItemType::STRING;
template <> inline DataItemType typeFor<CompoundTag> = DataItemType::NBT;
template <> inline DataItemType typeFor<long>        = DataItemType::LONG;
template <> inline DataItemType typeFor<Vec3>        = DataItemType::VEC3;

} // namespace DataTypeMap

class DataItem {
public:
  DataItemType type;
  uint16_t id;
  bool dirty;

  DataItem(DataItemType type, uint16_t id, bool dirty) : type(type), id(id), dirty(dirty) {}

  virtual ~DataItem() {}
  virtual MCAPI bool isDataEqual(DataItem const &rhs) const;
  virtual std::unique_ptr<DataItem> clone() const = 0;
};

template <typename T> class DataItem2 : public DataItem {
public:
  T value;
  DataItem2(uint16_t id, T const &value) : DataItem(DataTypeMap::typeFor<T>, id, true), value(value) {}
  MCAPI std::unique_ptr<DataItem> clone() const override;
};