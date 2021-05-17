#include "Grid.h"

#include "GridView.h"
#include <functional>

namespace sc2::utils
{

template<typename TPoint>
Point2DI get_tile_pos(const TPoint& pos)
{
    return Point2DI{(int)std::floor(pos.x), (int)std::floor(pos.y)};
}

template<class TA, class TB, class TFunc>
auto zip_with(const TA& a
    , const TB& b
    , TFunc pred)
{
    using VA = std::invoke_result<decltype(&TA::operator[]), const TA*, const Point2DI&>::type;
    using VB = std::invoke_result<decltype(&TB::operator[]), const TB*, const Point2DI&>::type;

    using TRes = std::invoke_result<TFunc, VA, VB>::type;
    Grid<TRes> res(a.getWidth(), a.getHeight());
    for (int y = 0; y < a.getHeight(); ++y)
    {
        for (int x = 0; x < a.getWidth(); ++x)
        {
            const auto pixel = sc2::Point2DI(x, y);
            const bool value = pred(a[pixel], b[pixel]);
            res[pixel] = value;
        }

    }
    return res;
}

template<class T, class TA, class TB>
    Grid<T> mark_with(const TA& a
    , const TB& b
    , T mark)
{
    auto res = Grid<T>{ a.getWidth(), a.getHeight() };
    for (int y = 0; y < a.getHeight(); ++y)
    {
        for (int x = 0; x < a.getWidth(); ++x)
        {
            const auto pixel = sc2::Point2DI{ x, y };
            res[pixel] = b[pixel] ? mark : a[pixel];
        }
    }
    return res;
}

}

