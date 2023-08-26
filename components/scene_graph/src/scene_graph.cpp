#include "scene_graph.hpp"

#include <common/logging.hpp>

namespace remus
{
Node SceneGraph::create_node()
{
	auto node    = std::make_shared<InternalNode>();
	node->handle = registry.create();
	node->owner  = this;
	nodes.push_back(node);
	return remus::Node(std::weak_ptr<InternalNode>{node});
}

void SceneGraph::update(float delta_time)
{
	for (auto &system : systems)
	{
		system.second->update(registry, delta_time);
	}
}

bool SceneGraph::add_system(const std::type_info &type_info, std::shared_ptr<System> &&system)
{
	for (auto &system : systems)
	{
		if (system.first == type_info)
		{
			LOGW("System already exists: {}", type_info.name());
			return false;
		}
	}

	systems[std::type_index{type_info}] = system;
	return true;
}
}        // namespace remus
