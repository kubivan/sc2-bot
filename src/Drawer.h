#pragma once

#include "SC2.h"

class Drawer
{
public:
    Drawer(sc2::DebugInterface& debug, const sc2::ObservationInterface& obs)
        : m_debug(debug)
        , m_obs(obs)
    {
    }

    float terrainHeight(const sc2::Point2D& point) const
    {
        auto& grid = m_obs.GetGameInfo().terrain_height;
        assert(grid.bits_per_pixel > 1);

        sc2::Point2DI pointI(static_cast<int>(point.x), static_cast<int>(point.y));
        if (pointI.x < 0 || pointI.x >= grid.width || pointI.y < 0 || pointI.y >= grid.height)
        {
            return 0.0f;
        }

        assert(grid.data.size() == static_cast<unsigned long>(grid.width * grid.height));
        unsigned char value = grid.data[pointI.x + pointI.y * grid.width];
        return (static_cast<float>(value) - 127.0f) / 8.f;
    }

    void drawLine(float x1, float y1, float x2, float y2, const sc2::Color& color)
    {
        m_callbacks.emplace_back( [&,x1, y1, x2, y2, color]() { m_debug.DebugLineOut(sc2::Point3D(x1, y1, terrainHeight({ x1, y1 }) + 0.2f), sc2::Point3D(x2, y2, terrainHeight({ x2, y2 }) + 0.2f), color); });
    }

    void drawVerticalLine(const sc2::Point2D point, const sc2::Color& color)
    {
        m_callbacks.emplace_back( [&,point, color]() { m_debug.DebugLineOut(sc2::Point3D(point.x, point.y, terrainHeight({ point.x, point.y }) + 0.2f), sc2::Point3D(point.x, point.y, terrainHeight({ point.x, point.y }) + 0.2f + 5), color); });
    }

    void drawLine(const sc2::Point2D& p1, const sc2::Point2D& p2, const sc2::Color& color)
    {
        drawLine(p1.x, p1.y, p2.x, p2.y, color);
    }

    void drawTile(const sc2::Point2DI p, sc2::Color color = sc2::Colors::Green)
    {
        return drawTile(p.x, p.y, color);
    }

    void drawTile(int tileX, int tileY, sc2::Color color = sc2::Colors::Green)
    {
        float px = (float)tileX + 0.1f;
        float py = (float)tileY + 0.1f;
        float d = 0.8f;

        drawLine(px, py, px + d, py, color);
        drawLine(px + d, py, px + d, py + d, color);
        drawLine(px + d, py + d, px, py + d, color);
        drawLine(px, py + d, px, py, color);
    }

    void update()
    {
        for (auto& c : m_callbacks)
        {
            c();
        }
    }
    
private:
    sc2::DebugInterface& m_debug;
    const sc2::ObservationInterface& m_obs;
    std::vector<std::function<void()>> m_callbacks;
};

