#pragma once

class Item;
class Block;

class ItemDescriptor {
public:
  Item *item{};         // 0
  Block *block{};       // 8
  unsigned short aux{}; // 16
  bool isItem{};        // 18
};