#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace remus
{
enum class AttributeType
{
	POSITION,
	NORMAL,
	TANGENT,
	TEXCOORD_0,
	TEXCOORD_1,
	COLOR_0,
	JOINTS_0,
	WEIGHTS_0
};

inline std::string to_string(AttributeType type)
{
#define CASE(x)            \
	case AttributeType::x: \
		return #x;

	switch (type)
	{
		CASE(POSITION)
		CASE(NORMAL)
		CASE(TANGENT)
		CASE(TEXCOORD_0)
		CASE(TEXCOORD_1)
		CASE(COLOR_0)
		CASE(JOINTS_0)
		CASE(WEIGHTS_0)
		default:
			return "Unknown";
	}

#undef CASE
}

using AttributesMap = std::unordered_map<AttributeType, std::vector<uint8_t>>;

enum class PrimitiveTopology
{
	TRIANGLES,
	LINES,
	POINTS
};

inline std::string to_string(PrimitiveTopology topology)
{
#define CASE(x)                \
	case PrimitiveTopology::x: \
		return #x;

	switch (topology)
	{
		CASE(TRIANGLES)
		CASE(LINES)
		CASE(POINTS)
		default:
			return "Unknown";
	}

#undef CASE
}

struct StaticMesh
{
	PrimitiveTopology topology;

	size_t               indices_count;
	std::vector<uint8_t> indices;

	AttributesMap attributes;
};
}        // namespace remus
