#include "Dispatch.h"
#include <thread>

namespace {
	auto sessDefault = CreateScalingFIFOExecutor(1, std::thread::hardware_concurrency(), 5000);
}

IExecutor* GetSessionDefault() { return sessDefault.get(); }
