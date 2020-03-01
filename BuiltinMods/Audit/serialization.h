#pragma once

#include <string>
#include <Packet/InventoryTransactionPacket.h>

std::string SerializeInventoryTransaction(ComplexInventoryTransaction const &trans);