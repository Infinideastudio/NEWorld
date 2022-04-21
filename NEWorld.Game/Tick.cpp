#include "Tick.h"
#include <unordered_map>
#include <kls/temp/STL.h>
#include <kls/coroutine/Operation.h>

std::shared_ptr<TickPipeline> CreateTickPipeline(std::vector<std::shared_ptr<TickComponent>> c) {
	struct ComponentHolder;

	struct ComponentConstructionInfo {
		int ActualDependencyCount{};
		ComponentHolder* FutureHolder{};
		std::vector<ComponentHolder*> ComponentsToRelease{};
	};

	struct ComponentHolder {
		const int ActualDependencyCount{};
		std::shared_ptr<TickComponent> Component{};
		std::vector<ComponentHolder*> ComponentsToRelease{};
		std::atomic_int AwaitingDependencies{ 0 };

		constexpr ComponentHolder() noexcept = default;

		ComponentHolder(ComponentConstructionInfo&& construct, std::shared_ptr<TickComponent> c) noexcept :
			ActualDependencyCount(construct.ActualDependencyCount), Component(std::move(c)),
			ComponentsToRelease(std::move(construct.ComponentsToRelease)) {}

		ComponentHolder(ComponentHolder&& o) noexcept :
			ActualDependencyCount(o.ActualDependencyCount), Component(std::move(o.Component)),
			ComponentsToRelease(std::move(o.ComponentsToRelease)) {}

        kls::coroutine::ValueAsync<void> Launch() {
			co_await kls::coroutine::Redispatch();
			co_await Component->Evaluate();
            kls::temp::vector<kls::coroutine::ValueAsync<void>> childern{};
			childern.reserve(ComponentsToRelease.size()); // reserve the max size
			// decrease the counter for all released component and spawn if necessary
			for (auto next : ComponentsToRelease) {
				if (next->AwaitingDependencies.fetch_sub(1) == 1) childern.push_back(next->Launch());
			}
			co_await kls::coroutine::await_all(std::move(childern));
		}
	};

	class Pipeline : public TickPipeline {
	public:
		explicit Pipeline(std::vector<std::shared_ptr<TickComponent>> components) {
			std::unordered_map<TickComponent*, ComponentConstructionInfo> info{};
			// construct the sorting base
			mComponents.reserve(components.size()); // this is done to ensure the references remains valid
			for (auto& c : components) {
				info.insert_or_assign(c.get(), ComponentConstructionInfo{ 0, &mComponents.emplace_back(), {} });
			}
			// construct dependency graph
			for (auto& c : components) {
				for (auto& b : c->GetToEvalBefore()) {
					info[c.get()].ActualDependencyCount++;
					info[b].ComponentsToRelease.push_back(info[c.get()].FutureHolder);
				}
				for (auto& a : c->GetToEvalAfter()) {
					info[c.get()].ComponentsToRelease.push_back(info[a].FutureHolder);
					info[a].ActualDependencyCount++;
				}
			}
			// consolidates the component graph information
			for (auto& c : components) {
				const auto ptr = c.get();
				auto& construct = info[ptr];
				auto& holder = *construct.FutureHolder;
				// we have to reuse the address for constructing the new holder
				std::destroy_at(&holder);
				std::construct_at(&holder, std::move(construct), std::move(c));
				if (holder.ActualDependencyCount == 0) mComponentRoots.push_back(&holder);
			}
		}

        kls::coroutine::ValueAsync<void> Evaluate() override {
			// reset all component counters
			for (auto& c : mComponents) c.AwaitingDependencies.store(c.ActualDependencyCount);
            kls::temp::vector<kls::coroutine::ValueAsync<void>> childern{};
			childern.reserve(mComponents.size()); // reserve the exact size
			// launch all the roots and await for the result
			for (auto root : mComponentRoots) childern.push_back(root->Launch());
			co_await kls::coroutine::await_all(std::move(childern));
		}
	private:
		std::vector<ComponentHolder> mComponents{};
		std::vector<ComponentHolder*> mComponentRoots{};

	};

	return std::make_shared<Pipeline>(std::move(c));
}
