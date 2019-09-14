#include "GridView.h"

using namespace sc2

GridView::GridView(const ImageData& data)
	: m_data(data.data.data())
	, m_bpp(data.bits_per_pixel)
	, m_area({ 0, 0 }, { data.width, data.height })
{
}

const char& operator[](const Point2DI& point) const
{
	if (this->bpp == 1)
	{
		div_t idx = div(point.x + point.y * m_area.Width(), 8);
		return (m_data[idx.quot] >> (7 - idx.rem)) & 1;
	}
	assert(m_bpp == 8);

	return m_data[point.y * m_area.Width() + point.y];
}

bool GridView::getBit(int x, int y) const
{
	return getBit({ x, y });
}
