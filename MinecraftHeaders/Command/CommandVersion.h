#pragma once

class CommandVersion {
  int min, max;

public:
  inline CommandVersion() : min(1), max(0x7FFFFFFF) {}
  inline CommandVersion(int min, int max) : min(min), max(max) {}
  static __declspec(dllimport) int const CurrentVersion;
};