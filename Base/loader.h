#pragma once

#include <filesystem>

#include <yaml.h>

extern void loadMods(YAML::Node &cfg_node);
extern void worldHook(std::filesystem::path const &);
extern void OnExit();