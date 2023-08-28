#pragma once

#include <entt/entt.hpp>

#include "transform.hpp"

namespace remus
{
class SceneGraph;

class SceneNode
{
  public:
	friend class remus::SceneGraph;

	SceneNode(entt::registry &registry) :
	    entity(registry.create()),
	    registry(&registry)
	{}

	~SceneNode() = default;

	std::string name;

	Transform transform;

	template <typename T, typename... Args>
	T &emplace_component(Args &&...args)
	{
		return registry->emplace_or_replace<T>(entity, std::forward<Args>(args)...);
	}

	template <typename T>
	T &add_component(const T &component = {})
	{
		return registry->emplace_or_replace<T>(entity, component);
	}

	template <typename T>
	T &get_component()
	{
		return registry->get<T>(entity);
	}

	template <typename T>
	bool has_component()
	{
		return registry->all_of<T>(entity);
	}

	template <typename T>
	void remove_component()
	{
		registry->remove<T>(entity);
	}

	void set_parent(SceneNode &node)
	{
		if (parent)
		{
			// remove this node from the parent's children
			parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
		}

		parent = &node;
		node.children.push_back(this);
	}

  private:
	entt::entity    entity;
	entt::registry *registry;

	SceneNode               *parent{nullptr};
	std::vector<SceneNode *> children;

	glm::mat4 get_world_matrix() const
	{
		if (parent)
		{
			return parent->get_world_matrix() * transform.get_matrix();
		}
		return transform.get_matrix();
	}
};

class SceneNodeRef
{
  public:
	SceneNodeRef() = default;

	SceneNodeRef(std::weak_ptr<SceneNode> node) :
	    node(node)
	{}

	~SceneNodeRef() = default;

	void set_name(const std::string &name) const
	{
		if (auto ptr = node.lock())
		{
			ptr->name = name;
			return;
		}
		throw std::runtime_error("Node is expired");
	}

	bool is_valid() const
	{
		return !node.expired();
	}

	Transform &transform()
	{
		if (auto ptr = node.lock())
		{
			return ptr->transform;
		}
		throw std::runtime_error("Node is expired");
	}

	template <typename T, typename... Args>
	T &emplace_component(Args &&...args)
	{
		if (auto ptr = node.lock())
		{
			return ptr->emplace_component<T>(std::forward<Args>(args)...);
		}
		throw std::runtime_error("Node is expired");
	}

	template <typename T>
	T &add_component(const T &component = {})
	{
		if (auto ptr = node.lock())
		{
			return ptr->add_component<T>(component);
		}
		throw std::runtime_error("Node is expired");
	}

	template <typename T>
	T &get_component()
	{
		if (auto ptr = node.lock())
		{
			return ptr->get_component<T>();
		}
		throw std::runtime_error("Node is expired");
	}

	template <typename T>
	bool has_component()
	{
		if (auto ptr = node.lock())
		{
			return ptr->has_component<T>();
		}
		throw std::runtime_error("Node is expired");
	}

	template <typename T>
	void remove_component()
	{
		if (auto ptr = node.lock())
		{
			ptr->remove_component<T>();
			return;
		}
		throw std::runtime_error("Node is expired");
	}

	void set_parent(SceneNodeRef &new_parent)
	{
		auto ptr      = new_parent.node.lock();
		auto this_ptr = node.lock();
		if (ptr && this_ptr)
		{
			this_ptr->set_parent(*ptr);
			return;
		}

		throw std::runtime_error("Node is expired");
	}

  private:
	std::weak_ptr<SceneNode> node;
};
}        // namespace remus
