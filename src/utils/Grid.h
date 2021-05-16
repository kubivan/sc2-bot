#pragma once

#include <sc2api/sc2_common.h>

#include <functional>
#include <vector>
#include <type_traits>

namespace sc2
{

template <class TValue, int BPP> 
struct GridBase
{
	using ValueType = TValue;
	static const int bpp = BPP;

	Rect2DI getArea() const
	{
		return m_area;
	}
protected:
	GridBase(Rect2DI area)
		: m_area(std::move(area))
	{
	}

	Rect2DI m_area;
};

template<class T>
class Grid : public GridBase<T, 8>
{
public:
	Grid(const Rect2DI& area)
		: GridBase(area)
		, m_area(area)
		, m_grid(area.Width() * area.Height())
	{
	}
	
	const T& operator[](const Point2DI& point) const
	{
		const auto x = m_area.from.x + point.x;
		const auto y = m_area.from.y + point.y;
		return m_grid[y*m_area.Width() + x ];
	}

	T& operator[](const Point2DI& point)
	{
		return const_cast<T&>(static_cast<const Grid<T>&>(*this).operator [](point));
	}

private:
	Rect2DI m_area;
	std::vector<typename GridBase::ValueType> m_grid;
};

}

