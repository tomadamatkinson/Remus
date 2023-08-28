#pragma once

#include <memory>
#include <typeindex>
#include <typeinfo>

#include <entt/entt.hpp>

#include "node.hpp"

namespace remus
{
class Node;
class SceneGraph;
using SceneGraphPtr = std::shared_ptr<SceneGraph>;

// Perform a process on a set of entities held in the scene graph.
class System
{
  public:
	virtual ~System()                                                     = default;
	virtual void update(entt::registry &registry, float delta_time) const = 0;
};

/* The scene graph is the root of the entity-component system.
 * It holds the registry and all the systems.
 * It is responsible for updating the systems.
 * It is responsible for creating nodes.
 */
class SceneGraph final
{
  public:
	friend class remus::Node;

	SceneGraph()  = default;
	~SceneGraph() = default;

	SceneNodeRef create_node();

	/*
	 * Add a system to the scene graph.
	 * Returns true if the system was added successfully.
	 */
	template <typename T, typename... Args>
	bool add_system(Args &&...args)
	{
		return add_system(typeid(T), std::make_shared<T>(args...));
	}

	template <typename T>
	bool remove_system()
	{
		return systems.erase(typeid(T)) > 0;
	}

	template <typename T>
	bool has_system()
	{
		return systems.find(typeid(T)) != systems.end();
	}

	void update(float delta_time);

	void print_scene_heirarchy(size_t spacing = 4) const;

	entt::registry &registry()
	{
		return _registry;
	}

  private:
	entt::registry                                     _registry;
	std::map<std::type_index, std::shared_ptr<System>> systems;

	std::vector<std::shared_ptr<SceneNode>> nodes;

	bool add_system(const std::type_info &type_info, std::shared_ptr<System> &&system);

	// used by print_scene_heirarchy()
	void print_node(SceneNode &node, int depth, size_t spacing) const;
};
}        // namespace remus
