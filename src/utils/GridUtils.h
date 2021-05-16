#include "Grid.h"

#include "GridView.h"
#include <functional>

namespace sc2
{

template<class TA, class TB, class TFunc>
auto zip_with(const TA& a
    , const TB& b
    , TFunc pred)
{
    using VA = typename TA::ValueType;
    using VB = typename TB::ValueType;
    using TRes = std::result_of<TFunc(VA, VB)>::type;
    Grid<TRes> res(a.getArea());
    for (int y = 0; y < a.getArea().Height(); ++y)
    {
        for (int x = 0; x < a.getArea().Width(); ++x)
        {
            const auto pixel = sc2::Point2DI(x, y);
            const bool value = pred(a[pixel], b[pixel]);
            res[pixel] = value;
        }

    }
    return res;
}

template<class T, class TA, class TB>
sc2::Grid<T> mark_with(const TA& a
    , const TB& b
    , T mark)
{
    auto res = sc2::Grid<T>{ a.getArea() };
    for (int y = 0; y < a.getArea().Height(); ++y)
    {
        for (int x = 0; x < a.getArea().Width(); ++x)
        {
            const auto pixel = sc2::Point2DI{ x, y };
            res[pixel] = b[pixel] ? mark : a[pixel];
        }
    }
    return res;
}

}

