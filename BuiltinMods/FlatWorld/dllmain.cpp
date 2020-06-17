#include <vector>

#include <Core/json.h>

#include <dllentry.h>
#include <hook.h>
#include <yaml.h>

#include <base/base.h>
#include <base/log.h>

DEF_LOGGER("FlatWorld");

struct BlockDef {
  std::string block_name;
  int count;

  BlockDef(std::string const &block_name, int count) : block_name(block_name), count(count) {}
};

std::vector<BlockDef> block_layers;

void ApplySettings(YAML::Node const &node) {
  auto layers = node["layers"];
  if (layers && layers.IsSequence()) {
    for (auto layer : layers) {
      if (layer.IsMap()) {
        auto block_name = layer["block_name"].as<std::string>();
        auto count      = layer["count"].as<int>();
        block_layers.emplace_back(block_name, count);
      }
    }
  } else {
    block_layers.emplace_back("minecraft:bedrock", 1);
    block_layers.emplace_back("minecraft:dirt", 2);
    block_layers.emplace_back("minecraft:grass", 1);
  }
}

bool GenerateSettings(YAML::Node &node) {
  auto layers = node["layers"] = {};
  for (auto &block_layer : block_layers) {
    YAML::Node layer;
    layer["block_name"] = block_layer.block_name;
    layer["count"]      = block_layer.count;
    layers.push_back(layer);
  }
  return true;
}

void dllenter() {}
void dllexit() {}

TClasslessInstanceHook(
    bool, "?_load@FlatWorldGeneratorOptions@@IEAA_NAEBVValue@Json@@AEBVBlockPalette@@@Z", Json::Value const &json,
    void *option) {
  static bool first = true;
  if (first)
    first = false;
  else {
    LOGI("generate level");
    Json::Value root{Json::objectValue};
    root["encoding_version"]  = 5;
    root["biome_id"]          = 0;
    root["structure_options"] = {Json::nullValue};
    Json::Value json_layers{Json::arrayValue};
    for (auto &block_layer : block_layers) {
      Json::Value layer{Json::objectValue};
      layer["block_name"] = block_layer.block_name.c_str();
      layer["count"]      = block_layer.count;
      json_layers.append(layer);
    }
    root["block_layers"] = json_layers;
    return original(this, root, option);
  }
  return original(this, json, option);
}