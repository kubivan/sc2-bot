#pragma once

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_map_info.h>


#include <utils/GridView.h>

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


    Grid(int width, int height, T value = {})
        : m_width(width)
        , m_height(height)
        , m_grid(width * height, value)
    {
    }

    Grid(const ImageData& data)
        : Grid(data.width, data.height)
    {
        auto view = GridView(data);
        for (auto y = 0; y < view.getHeight(); ++y)
        {
            for (auto x = 0; x < view.getWidth(); ++x)
            {
                m_grid[y * view.getWidth() + x] = view[{x,y}];
            }
        }
    }

    const T&
    operator[](const Point2DI& point) const
    {
        return m_grid[point.y * m_width + point.x];
    }

    T&
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

