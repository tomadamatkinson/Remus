#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

namespace remus
{
struct Image
{
	size_t               width{0};
	size_t               height{0};
	std::vector<uint8_t> data;
};

using ImagePtr = std::shared_ptr<Image>;

struct PBRMaterial
{
	float     metallic_factor{1.0f};
	float     roughness_factor{1.0f};
	float     normal_scale{1.0f};
	float     occlusion_strength{1.0f};
	glm::vec4 emissive_factor{1.0f};
	glm::vec4 base_color_factor{1.0f, 1.0f, 1.0f, 1.0f};
	ImagePtr  base_color_texture{};
	ImagePtr  metallic_roughness_texture{};
	ImagePtr  normal_texture{};
	ImagePtr  occlusion_texture{};
	ImagePtr  emissive_texture{};
};
};        // namespace remus
