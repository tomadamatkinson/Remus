#include <loaders/models/gltf_loader.hpp>

#include <common/logging.hpp>
#include <scene_graph/components/static_mesh.hpp>
#include <scene_graph/scene_graph.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Load a glTF file", "[scene_graph]")
{
	remus::SceneGraph scene_graph;

	remus::GLtfLoader gltf_loader;

	auto node = gltf_loader.load("./assets/porsche_911/scene.gltf", scene_graph);
	REQUIRE(node.is_valid());
}
