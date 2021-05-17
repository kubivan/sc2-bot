#include "GridView.h"

namespace sc2::utils
{

GridView::GridView(const ImageData& data)
    : m_width(data.width)
    , m_height(data.height)
    , m_data(data.data)
{
}

auto GridView::operator[](const Point2DI& point) const -> bool
{
    div_t idx = div(point.x + point.y * getWidth(), 8);
    auto d = m_data[idx.quot] >> (7 - idx.rem);
    return (m_data[idx.quot] >> (7 - idx.rem)) & 1;
}

}
