#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <memory>

#include <boost/format.hpp>
#include <boost/scope_exit.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <yaml-cpp/yaml.h>
#include <SQLiteCpp/SQLiteCpp.h>

#include <Command/Command.h>
#include <Command/CommandRegistry.h>
#include <Command/CommandPermissionLevel.h>
#include <Command/CommandFlag.h>
#include <Command/CommandParameterData.h>
#include <Command/CommandOutput.h>
#include <Command/CommandOrigin.h>
#include <TextObject/TextObjectRoot.h>
#include <TextObject/TextObjectLocalizedTextWithParams.h>
#include <Actor/ServerPlayer.h>
#include <Actor/Player.h>
#include <Packet/TransferPacket.h>
#include <Packet/TextPacket.h>
#include <Level/Level.h>

#include <command.h>
#include <hook.h>
#include <playerdb.h>
#include <log.h>
#include <scheduler.h>
#include <essentials.h>