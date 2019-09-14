#pragma once

#include <utils/Grid.h>
#include <sc2api/sc2_map_info.h>

namespace sc2
{

class GridView : public GridBase<bool, 1>
{
public:
	GridView(const ImageData& data);

	auto operator[](const Point2DI& point) const -> ValueType;

private:
	const std::string& m_data;
};

}

