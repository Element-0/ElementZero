#pragma once

struct ContainerContentChangeListener {
  virtual ~ContainerContentChangeListener(){};
  virtual void ContainerContentChanged(int) = 0;
};