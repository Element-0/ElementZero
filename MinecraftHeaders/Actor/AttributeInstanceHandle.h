#pragma once

class BaseAttributeMap;

class AttributeInstanceHandle {
  int id;
  BaseAttributeMap *source;
};

static_assert(sizeof(AttributeInstanceHandle) == 16);