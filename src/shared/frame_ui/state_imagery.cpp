// Copyright (C) 2020, Robin Klimonow. All rights reserved.

#include "state_imagery.h"

#include "base/macros.h"

#include <utility>
#include <algorithm>


namespace mmo
{
	StateImagery::StateImagery(std::string name)
		: m_name(std::move(name))
	{
	}

	void StateImagery::AddLayer(std::shared_ptr<FrameLayer>& layer)
	{
		m_layers.emplace_back(layer);
	}

	void StateImagery::RemoveLayer(uint32 index)
	{
		ASSERT(index < m_layers.size());
		m_layers.erase(m_layers.begin() + index);
	}

	void StateImagery::RemoveAllLayers()
	{
		m_layers.clear();
	}

	void StateImagery::Render(Frame & frame) const
	{
		for (const auto& layer : m_layers)
		{
			layer->Render(frame);
		}
	}
}