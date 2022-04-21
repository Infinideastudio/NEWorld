#include "Dispatch.h"
#include <thread>

namespace {
	auto sessDefault = kls::coroutine::CreateScalingFIFOExecutor(1, std::thread::hardware_concurrency(), 5000);
}

kls::coroutine::IExecutor * GetSessionDefault() { return sessDefault.get(); }
