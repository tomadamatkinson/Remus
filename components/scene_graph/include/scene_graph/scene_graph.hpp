#pragma once

#include <memory>
#include <typeindex>
#include <typeinfo>

#include <entt/entt.hpp>

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

	Node create_node();

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

  private:
	entt::registry                                     registry;
	std::map<std::type_index, std::shared_ptr<System>> systems;

	struct InternalNode
	{
		entt::entity handle;
		SceneGraph  *owner;
	};

	std::vector<std::shared_ptr<InternalNode>> nodes;

	bool add_system(const std::type_info &type_info, std::shared_ptr<System> &&system);
};

/* A node is a handle to an entity in the scene graph.
 * It is responsible for adding components to the entity.
 * It is responsible for removing components from the entity.
 * It is responsible for adding children to the entity.
 * It is responsible for rdemoving chilren from the entity.
 * It represents the transformation of the entity.
 */
class Node
{
  public:
	friend class remus::SceneGraph;

	~Node() = default;

	bool is_valid() const
	{
		return !internal.expired();
	}

	operator bool() const
	{
		return is_valid();
	}

	template <typename T, typename... Args>
	T &emplace_component(Args &&...args)
	{
		if (auto ptr = internal.lock())
		{
			return ptr->owner->registry.emplace<T>(ptr->handle, std::forward<Args>(args)...);
		}
		throw std::runtime_error("Node is expired");
	}

	template <typename T>
	T &add_component(const T &component = {})
	{
		if (auto ptr = internal.lock())
		{
			return ptr->owner->registry.emplace<T>(ptr->handle, component);
		}
		throw std::runtime_error("Node is expired");
	}

	template <typename T>
	T &get_component()
	{
		if (auto ptr = internal.lock())
		{
			return ptr->owner->registry.get<T>(ptr->handle);
		}
		throw std::runtime_error("Node is expired");
	}

	template <typename T>
	bool has_component()
	{
		if (auto ptr = internal.lock())
		{
			return ptr->owner->registry.all_of<T>(ptr->handle);
		}
		throw std::runtime_error("Node is expired");
	}

	template <typename T>
	void remove_component()
	{
		if (auto ptr = internal.lock())
		{
			ptr->owner->registry.remove<T>(ptr->handle);
		}
		else
		{
			throw std::runtime_error("Node is expired");
		}
	}

  private:
	Node(std::weak_ptr<remus::SceneGraph::InternalNode> internal) :
	    internal(internal)
	{}

	std::weak_ptr<remus::SceneGraph::InternalNode> internal;
};
}        // namespace remus
