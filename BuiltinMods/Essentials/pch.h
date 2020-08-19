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

#include <Core/PacketSender.h>
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
#include <Packet/SetActorDataPacket.h>
#include <Level/Level.h>

#include <hook.h>

#include <base/playerdb.h>
#include <base/log.h>
#include <base/scheduler.h>
#include <mods/CommandSupport.h>
#include <mods/Essentials.h>
#include <mods/FormUI.h>