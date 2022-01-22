#pragma once

#include <vector>
#include <Coro/Coro.h>
#include <System/PmrBase.h>

struct TickComponent: PmrBase {
	virtual std::vector<TickComponent*> GetToEvalAfter() = 0;
	virtual std::vector<TickComponent*> GetToEvalBefore() = 0;
	virtual ValueAsync<void> Evaluate() = 0;
};

struct TickPipeline: PmrBase {
	virtual ValueAsync<void> Evaluate() = 0;
};

std::shared_ptr<TickPipeline> CreateTickPipeline(std::vector<std::shared_ptr<TickComponent>>);
