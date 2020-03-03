#pragma once

#include <filesystem>

#include <yaml.h>

void loadMods(YAML::Node &cfg_node);
void worldHook(std::filesystem::path const &);