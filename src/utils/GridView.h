#pragma once

#include <utils/Grid.h>
#include <sc2api/sc2_map_info.h>

namespace sc2::utils
{

class GridView
{
public:
    GridView(const ImageData& data);

    auto operator[](const Point2DI& point) const -> bool;

    auto data() const { return m_data.data(); };

    int getWidth() const { return m_width; };
    int getHeight() const { return m_height; };

private:
    const std::string& m_data;
    int m_width;
    int m_height;
};

}