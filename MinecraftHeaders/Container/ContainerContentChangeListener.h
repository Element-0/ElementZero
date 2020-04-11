#pragma once

struct ContainerContentChangeListener {
  virtual void ContainerContentChanged(int) = 0;
  virtual ~ContainerContentChangeListener(){};
};