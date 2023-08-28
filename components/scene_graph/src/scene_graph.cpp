#include "scene_graph.hpp"

#include <common/logging.hpp>

namespace remus
{
SceneNodeRef SceneGraph::create_node()
{
	auto node = std::make_shared<SceneNode>(_registry);
	nodes.push_back(node);
	return SceneNodeRef(std::weak_ptr<SceneNode>{node});
}

void SceneGraph::update(float delta_time)
{
	// Update all world matrices and transforms
	for (auto &node : nodes)
	{
		node->emplace_component<WorldMatrix>(node->get_world_matrix());
		node->add_component(node->transform);
	}

	// Update all systems
	for (auto &system : systems)
	{
		system.second->update(_registry, delta_time);
	}
}

void SceneGraph::print_node(SceneNode &node, int depth, size_t spacing) const
{
	if (depth == 0)
	{
		LOGI("Scene heirarchy: {}", node.name);
	}

	std::string indent(depth * spacing, ' ');
	LOGI("{}| {}", indent, node.name);
	for (auto &child : node.children)
	{
		print_node(*child, depth + 1, spacing);
	}
}

void SceneGraph::print_scene_heirarchy(size_t spacing) const
{
	std::vector<std::shared_ptr<SceneNode>> roots;

	for (auto &node : nodes)
	{
		if (!node->parent)
		{
			roots.push_back(node);
		}
	}

	for (auto &root : roots)
	{
		print_node(*root, 0, spacing);
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
