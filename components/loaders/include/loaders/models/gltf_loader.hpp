#pragma once

#include <scene_graph/scene_graph.hpp>

namespace remus
{
class GLtfLoader
{
  public:
	SceneNodeRef load(const std::string &path, SceneGraph &scene_graph) const;
};
}        // namespace remus
