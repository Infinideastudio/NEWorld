#pragma once

#include <vector>
#include <kls/Object.h>
#include <kls/coroutine/Async.h>

struct TickComponent: kls::PmrBase {
	virtual std::vector<TickComponent*> GetToEvalAfter() = 0;
	virtual std::vector<TickComponent*> GetToEvalBefore() = 0;
	virtual kls::coroutine::ValueAsync<void> Evaluate() = 0;
};

struct TickPipeline: kls::PmrBase {
	virtual kls::coroutine::ValueAsync<void> Evaluate() = 0;
};

std::shared_ptr<TickPipeline> CreateTickPipeline(std::vector<std::shared_ptr<TickComponent>>);
