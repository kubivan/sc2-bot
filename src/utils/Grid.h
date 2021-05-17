#pragma once

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_map_info.h>

#include <functional>
#include <vector>
#include <deque>
#include <type_traits>

namespace sc2::utils
{

template<class T>
class Grid
{
public:
    Grid(int width, int height)
        : m_width(width)
        , m_height(height)
        , m_grid(width * height)
    {
    }

    Grid(const ImageData& data)
        : Grid(data.width, data.height)
    {

    }

    const auto&
    operator[](const Point2DI& point) const
    {
        return m_grid[point.y * m_width + point.x];
    }

    auto&
    operator[](const Point2DI& point)
    {
        return const_cast<T&>(static_cast<const Grid<T>&>(*this).operator [](point));
    }

    int getWidth() const { return m_width; };
    int getHeight() const { return m_height; };

private:
    int m_width;
    int m_height;
    std::deque<T> m_grid;
};

}

