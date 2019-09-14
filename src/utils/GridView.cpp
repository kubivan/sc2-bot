#include "GridView.h"

using namespace sc2;

GridView::GridView(const ImageData& data)
	: GridBase({ { 0, 0 }, { data.width, data.height } })
	, m_data(data.data)
{
	assert(data.bits_per_pixel, bpp);
}

auto GridView::operator[](const Point2DI& point) const -> ValueType
{
	div_t idx = div(point.x + point.y * m_area.Width(), 8);
	return (m_data[idx.quot] >> (7 - idx.rem)) & 1;
}
