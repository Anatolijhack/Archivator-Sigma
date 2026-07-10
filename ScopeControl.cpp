#include "ScopeControl.h"

ScopeControl::ScopeControl(PositonControl& w) : writer(w) { start = writer.tell(); };
uint64_t ScopeControl::end() const
{
	return writer.tell();
}
uint64_t ScopeControl::size() const
{
	return end() - start;
}
