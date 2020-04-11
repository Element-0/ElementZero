#pragma once

struct ContainerSizeChangeListener {
  virtual ~ContainerSizeChangeListener(){};
  virtual void ContainerSizeChanged(int) = 0;
};