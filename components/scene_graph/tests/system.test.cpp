#include <scene_graph/scene_graph.hpp>

#include <catch2/catch_test_macros.hpp>

struct SystemData
{
	int i = 0;
};

class IncrementSystem final : public remus::System
{
  public:
	virtual ~IncrementSystem() override = default;
	IncrementSystem()                   = default;
	IncrementSystem(int increment_amount) :
	    increment_amount(increment_amount)
	{}

	virtual void update(entt::registry &registry, float delta_time) const override
	{
		auto view = registry.view<SystemData>();
		for (auto entity : view)
		{
			auto &data = view.get<SystemData>(entity);
			data.i += increment_amount;
		}
	}

	int increment_amount = 1;
};

TEST_CASE("Add a system", "[scene_graph]")
{
	remus::SceneGraph scene_graph;

	// Add a system to the scene graph. This should succeed.
	REQUIRE(scene_graph.add_system<IncrementSystem>() == true);

	// Add a system to the scene graph again. This should fail.
	// The system already exists.
	REQUIRE(scene_graph.add_system<IncrementSystem>() == false);
}

TEST_CASE("Remove a system", "[scene_graph]")
{
	remus::SceneGraph scene_graph;

	// Add a system to the scene graph. This should succeed.
	REQUIRE(scene_graph.add_system<IncrementSystem>() == true);

	// Remove the system from the scene graph. This should succeed.
	REQUIRE(scene_graph.remove_system<IncrementSystem>() == true);

	// Remove the system from the scene graph again. This should fail.
	// The system does not exist.
	REQUIRE(scene_graph.remove_system<IncrementSystem>() == false);
}

TEST_CASE("Has system", "[scene_graph]")
{
	remus::SceneGraph scene_graph;

	// Add a system to the scene graph. This should succeed.
	REQUIRE(scene_graph.add_system<IncrementSystem>() == true);

	// Check if the system exists. This should succeed.
	REQUIRE(scene_graph.has_system<IncrementSystem>() == true);

	// Remove the system from the scene graph. This should succeed.
	REQUIRE(scene_graph.remove_system<IncrementSystem>() == true);

	// Check if the system exists. This should fail.
	REQUIRE(scene_graph.has_system<IncrementSystem>() == false);
}

TEST_CASE("Update a system", "[scene_graph]")
{
	remus::SceneGraph scene_graph;

	scene_graph.add_system<IncrementSystem>();

	auto node = scene_graph.create_node();

	node.add_component<SystemData>({0});

	for (int i = 0; i < 10; i++)
	{
		scene_graph.update(0.0f);
		REQUIRE(node.get_component<SystemData>().i == i + 1);
	}
}

TEST_CASE("Add system with arguments", "[scene_graph]")
{
	remus::SceneGraph scene_graph;

	// Add a system to the scene graph. This should succeed.
	REQUIRE(scene_graph.add_system<IncrementSystem>(2) == true);

	// Add a system to the scene graph again. This should fail.
	// The system already exists.
	REQUIRE(scene_graph.add_system<IncrementSystem>(2) == false);
}

TEST_CASE("Update a system with arguments", "[scene_graph]")
{
	remus::SceneGraph scene_graph;

	int increment_amount = 2;

	scene_graph.add_system<IncrementSystem>(increment_amount);

	auto node = scene_graph.create_node();

	node.emplace_component<SystemData>();

	for (int i = 0; i < 10; i++)
	{
		scene_graph.update(0.0f);
		REQUIRE(node.get_component<SystemData>().i == (i + 1) * increment_amount);
	}
}
