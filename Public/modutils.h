#pragma once

template <typename F> auto IIFE(F f) { return f(); }