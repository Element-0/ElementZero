#pragma once

#include <hook.h>

class BlockSource {
public:
	inline BlockSource(const BlockSource&);
	inline BlockSource& operator=(const BlockSource&);
	inline BlockSource();
inline bool isConsideredSolidBlock(class BlockPos const& a0) {
return CallServerClassMethod<bool>("?isConsideredSolidBlock@BlockSource@@QEAA_NAEBVBlockPos@@@Z", this, &a0);
	}
inline class Block const& getBlock(class BlockPos const& a0) const {
	return CallServerClassMethod<class Block const&>("?getBlock@BlockSource@@QEBAAEBVBlock@@AEBVBlockPos@@@Z", this, &a0);
}
};