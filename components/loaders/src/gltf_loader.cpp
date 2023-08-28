#include "gltf_loader.hpp"

#include <common/logging.hpp>

#include <scene_graph/components/material.hpp>
#include <scene_graph/components/static_mesh.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <tiny_gltf.h>

namespace remus
{
inline AttributeType to_attribute_type(const std::string &tiny_gltf_attribute)
{
#define CASE(x)                    \
	if (tiny_gltf_attribute == #x) \
		return AttributeType::x;

	CASE(POSITION)
	CASE(NORMAL)
	CASE(TANGENT)
	CASE(TEXCOORD_0)
	CASE(TEXCOORD_1)
	CASE(COLOR_0)
	CASE(JOINTS_0)
	CASE(WEIGHTS_0)

#undef CASE

	LOG_ASSERT(false, "Unsupported attribute type: {}", tiny_gltf_attribute);
	return AttributeType::POSITION;
}

inline PrimitiveTopology to_primitive_topology(int tiny_gltf_mode)
{
	switch (tiny_gltf_mode)
	{
		case TINYGLTF_MODE_TRIANGLES:
			return PrimitiveTopology::TRIANGLES;
		case TINYGLTF_MODE_LINE:
			return PrimitiveTopology::LINES;
		case TINYGLTF_MODE_POINTS:
			return PrimitiveTopology::POINTS;

		default:
			LOG_ASSERT(false, "Unsupported primitive topology: {}", tiny_gltf_mode);
			return PrimitiveTopology::TRIANGLES;
	}
}

inline std::vector<uint8_t> load_buffer_from_accessor(tinygltf::Model &model, int index)
{
	if (index < 0)
	{
		return {};
	}

	auto &accessor = model.accessors[index];
	auto &view     = model.bufferViews[accessor.bufferView];
	auto &buffer   = model.buffers[view.buffer];

	size_t offset = accessor.byteOffset + view.byteOffset;
	size_t size   = accessor.count * tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type);

	return std::vector<uint8_t>(buffer.data.begin() + offset, buffer.data.begin() + offset + size);
}

SceneNodeRef GLtfLoader::load(const std::string &path, SceneGraph &scene_graph) const
{
	tinygltf::Model    model;
	tinygltf::TinyGLTF loader;
	std::string        error;
	std::string        warning;

	bool ret = loader.LoadASCIIFromFile(&model, &error, &warning, path);

	if (!warning.empty())
	{
		LOGW("GLTF loader: {}", warning);
	}

	if (!error.empty())
	{
		LOGE("GLTF loader: {}", error);
	}

	if (!ret)
	{
		LOGE("GLTF loader: Failed to parse glTF");
		return {};
	}

	std::vector<ImagePtr> images;
	images.reserve(model.images.size());
	for (auto &image : model.images)
	{
		Image image_data;
		image_data.width  = image.width;
		image_data.height = image.height;
		image_data.data   = image.image;
		images.push_back(std::make_shared<Image>(image_data));
	}

	std::vector<SceneNodeRef> nodes;
	nodes.reserve(model.nodes.size());

	for (auto &node : model.nodes)
	{
		SceneNodeRef n = scene_graph.create_node();
		n.set_name(node.name);

		if (node.mesh > -1)
		{
			auto &mesh = model.meshes[node.mesh];

			bool should_create_subnodes = mesh.primitives.size() > 1;

			size_t primitive_index = 0;
			for (auto &primitive : mesh.primitives)
			{
				StaticMesh static_mesh;
				static_mesh.topology = to_primitive_topology(primitive.mode);

				auto &indices_accessor    = model.accessors[primitive.indices];
				static_mesh.indices_count = indices_accessor.count;
				static_mesh.indices       = load_buffer_from_accessor(model, primitive.indices);

				for (auto attributes : primitive.attributes)
				{
					static_mesh.attributes.emplace(to_attribute_type(attributes.first), load_buffer_from_accessor(model, attributes.second));
				}

				if (primitive.material > -1)
				{
					auto &material = model.materials[primitive.material];

					PBRMaterial pbr_material;
					pbr_material.metallic_factor    = material.pbrMetallicRoughness.metallicFactor;
					pbr_material.roughness_factor   = material.pbrMetallicRoughness.roughnessFactor;
					pbr_material.normal_scale       = material.normalTexture.scale;
					pbr_material.occlusion_strength = material.occlusionTexture.strength;
					pbr_material.emissive_factor    = glm::make_vec4(material.emissiveFactor.data());
					pbr_material.base_color_factor  = glm::make_vec4(material.pbrMetallicRoughness.baseColorFactor.data());

					auto lookup_image = [&](int texture_index) -> ImagePtr {
						auto &texture = model.textures[texture_index];
						if (texture.source > -1)
						{
							return images[texture.source];
						}
						return nullptr;
					};

					if (material.pbrMetallicRoughness.baseColorTexture.index > -1)
					{
						pbr_material.base_color_texture = lookup_image(material.pbrMetallicRoughness.baseColorTexture.index);
					}

					if (material.pbrMetallicRoughness.metallicRoughnessTexture.index > -1)
					{
						pbr_material.metallic_roughness_texture = lookup_image(material.pbrMetallicRoughness.metallicRoughnessTexture.index);
					}

					if (material.normalTexture.index > -1)
					{
						pbr_material.normal_texture = lookup_image(material.normalTexture.index);
					}

					if (material.occlusionTexture.index > -1)
					{
						pbr_material.occlusion_texture = lookup_image(material.occlusionTexture.index);
					}

					if (material.emissiveTexture.index > -1)
					{
						pbr_material.emissive_texture = lookup_image(material.emissiveTexture.index);
					}

					n.add_component(pbr_material);
				}

				if (should_create_subnodes)
				{
					auto subnode = scene_graph.create_node();
					subnode.set_name(node.name + "_" + mesh.name + "_primitive_" + std::to_string(primitive_index));
					subnode.add_component(static_mesh);
					subnode.set_parent(n);
				}
				else
				{
					n.add_component(static_mesh);
				}

				primitive_index++;
			}
		}

		auto &transform = n.transform();

		if (node.translation.size() == 3)
		{
			transform.translation = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
		}

		if (node.rotation.size() == 4)
		{
			transform.rotation = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
		}

		if (node.scale.size() == 3)
		{
			transform.scale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
		}

		if (node.matrix.size() == 16)
		{
			// extract the matrix
			glm::mat4 matrix;
			std::transform(node.matrix.begin(), node.matrix.end(), glm::value_ptr(matrix), [](double d) { return static_cast<float>(d); });

			// decompose the matrix
			glm::vec3 skew(0.0f);
			glm::vec4 perspective(0.0f);
			glm::decompose(matrix, transform.scale, transform.rotation, transform.translation, skew, perspective);
		}

		nodes.push_back(n);
	}

	// Relate tree heirarchy
	for (size_t node_index = 0; node_index < model.nodes.size(); node_index++)
	{
		for (auto &child_index : model.nodes[node_index].children)
		{
			nodes[child_index].set_parent(nodes[node_index]);
		}
	}

	// Relate scenes
	SceneNodeRef root{};
	for (auto &scene : model.scenes)
	{
		auto scene_root = scene_graph.create_node();
		scene_root.set_name(scene.name);

		for (auto &node : scene.nodes)
		{
			nodes[node].set_parent(scene_root);
		}

		// take the first loaded scene
		if (!root.is_valid())
		{
			root = scene_root;
		}
	}

	return root;
}
}        // namespace remus
