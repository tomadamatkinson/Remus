#include <scene_graph/scene_graph.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Create a Node", "[scene_graph]")
{
	remus::SceneGraph scene_graph;

	auto node = scene_graph.create_node();

	REQUIRE(node.is_valid());
}

struct Data
{
	int i = 0;
};

TEST_CASE("Add a component", "[scene_graph]")
{
	remus::SceneGraph scene_graph;

	auto node = scene_graph.create_node();

	node.add_component<Data>();

	REQUIRE(node.has_component<Data>());
}

TEST_CASE("Emplace a component", "[scene_graph]")
{
	remus::SceneGraph scene_graph;

	auto node = scene_graph.create_node();

	node.emplace_component<Data>(42);

	REQUIRE(node.has_component<Data>());
	REQUIRE(node.get_component<Data>().i == 42);
}

TEST_CASE("Remove a component", "[scene_graph]")
{
	remus::SceneGraph scene_graph;

	auto node = scene_graph.create_node();

	node.add_component<Data>();
	node.remove_component<Data>();

	REQUIRE(!node.has_component<Data>());
}
