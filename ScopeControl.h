#pragma once
#include "Compress.h"
#include "PositionWriter.h"
class ScopeControl
{
private:
	PositonControl& writer;
	uint64_t start;
public:
	ScopeControl(PositonControl& w);
	uint64_t end() const;
	uint64_t size() const;
};