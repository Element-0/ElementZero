add_repositories "repo https://github.com/codehz/xrepo"

add_requires ("funchook", {system = false, configs = {vs_runtime = "MD"}})
add_requires ("boost", {system = false, configs = {vs_runtime = "MD", filesystem = false}})
add_requires ("sqlite3 3.31.0100", {system = false, configs = {vs_runtime = "MD"}})
add_requires ("sqlite_cpp", {system = false, configs = {vs_runtime = "MD"}})
add_requires ("yaml-cpp", {system = false, configs = {vs_runtime = "MD"}})
add_requires ("jsoncpp", {system = false, configs = {vs_runtime = "MD"}})
add_requires ("chakra_core", {system = false, configs = {vs_runtime = "MD"}})
add_requires ("msgpack", {system = false, configs = {vs_runtime = "MD"}})