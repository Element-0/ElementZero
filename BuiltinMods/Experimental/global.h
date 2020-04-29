#include <yaml.h>

inline struct Settings {
  bool force_experimental_gameplay = false;
  bool education_feature           = false;
  bool debug_packs                 = false;

  template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
    return f(settings.force_experimental_gameplay, node["force-experimental-gameplay"]) &&
           f(settings.education_feature, node["education-feature"]) && f(settings.debug_packs, node["debug-packs"]);
  }
} settings;