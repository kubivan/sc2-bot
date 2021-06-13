#include "CannonRush.h"
#include <SC2.h>

#include <Utils.h>
#include <iostream>
#include <sstream>
#include <numeric>
#include <glm/glm.hpp>
#include <utils/UnitQuery.h>

constexpr float pylon_radius = 6.5f;
constexpr float pylon_radius_squared = pylon_radius * pylon_radius;

using namespace sc2;

CannonRush::~CannonRush()
{
}

CannonRush::CannonRush(SC2& sc2)
    : m_sc2(sc2)
{
}

void
CannonRush::step()
{
    assign_rushers();

    for (auto& rusher : m_rushers)
    {
        rusher->step();
    }
}

void
CannonRush::Rusher::step()
{
    m_heading.x = m_unit->pos.x + cos(m_unit->facing);
    m_heading.y = m_unit->pos.y + sin(m_unit->facing);
    float maxZ = 0.f;
    for (auto& unit : m_sc2.obs().GetUnits())
    {
        maxZ = std::max(unit->pos.z, maxZ);
    }
    m_heading.z = maxZ;
    m_sc2.debug().DebugLineOut(m_unit->pos, m_heading, sc2::Colors::Blue);

    auto str = "(" + std::to_string(m_unit->pos.x) + ",\n" + std::to_string(m_unit->pos.y) + ",\n" + std::to_string(m_unit->facing) + ")";
    m_sc2.debug().DebugTextOut(str, sc2::Point3D(m_unit->pos.x, m_unit->pos.y, maxZ), sc2::Colors::Green, 20);
    auto camera_pos = m_sc2.obs().GetCameraPos() + sc2::Point2D(cos(m_unit->facing), sin(m_unit->facing));

    auto enemies_to_evade = m_sc2.obs().GetUnits(enemy && not_a(building) && in_radius(this->m_unit->pos, 6));
    for (auto& enemy : enemies_to_evade)
    {
        auto heading = enemy->pos + sc2::Point3D{ cos(m_unit->facing), sin(m_unit->facing), 0 };
        //heading.z = maxZ;
        m_sc2.debug().DebugLineOut(enemy->pos, heading, sc2::Colors::Red);
    }

    if (!enemies_to_evade.empty())
    {
        auto enemy = enemies_to_evade.front();
        auto evade_vector = m_unit->pos - enemy->pos;
        sc2::Normalize3D(evade_vector);
        evade_vector *= 2.f;
        auto evade_pos = m_unit->pos + evade_vector;
        if (m_sc2.query().PathingDistance(m_unit->pos, evade_pos) <= 0)
        {
            auto minerals_patch = m_sc2.obs().GetUnits(type(sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD)).front();
            auto rand = rand_point_around(m_unit->pos, 8.);
            evade_pos = { rand.x, rand.y, 0 };
            evade_pos = minerals_patch->pos;
        }
        return m_sc2.act().UnitCommand(m_unit, sc2::ABILITY_ID::MOVE, evade_pos);
    }

    if (m_closest_targets.empty())
    {
        m_closest_targets = m_sc2.obs().GetUnits(enemy && building);
    }
    switch (m_state)
    {
    case Rusher::State::Idle:
        m_sc2.act().UnitCommand(m_unit, sc2::ABILITY_ID::STOP);

        m_target = enemy_base_location(m_sc2);
        return set_state(State::Scouting);
        break;
    case Rusher::State::Scouting:
        return scout();
    case Rusher::State::Rushing:
        return rush();
    default:
        assert(false);
        break;
    }
}

const sc2::Unit*
CannonRush::Rusher::unit() const
{
    return m_unit;
}

void
CannonRush::Rusher::scout()
{
    if (!m_unit->orders.empty())
    {
        return;
    }
    if (sc2::DistanceSquared2D(m_target, sc2::Point2D{ m_unit->pos }) > 10 * 10)
    {
        m_sc2.act().UnitCommand(m_unit, sc2::ABILITY_ID::MOVE, m_target);
    }
    else
    {
        return set_state(Rusher::State::Rushing);
    }
}

void CannonRush::Rusher::rush()
{
    if (!m_unit->orders.empty())
    {
        return;
    }

    if (!m_parent->has_forge())
    {
        return m_sc2.act().UnitCommand(m_unit, sc2::ABILITY_ID::MOVE, rand_point_around(m_unit->pos, 4, 4));
    }

    m_target = m_closest_targets.back()->pos;
    const auto near_pylons = m_sc2.obs().GetUnits(
        type(sc2::UNIT_TYPEID::PROTOSS_PYLON) && in_radius(m_target, pylon_radius));

    if (near_pylons.size() < 2)
    {
        return m_sc2.act().UnitCommand(m_unit, sc2::ABILITY_ID::BUILD_PYLON, rand_point_near(m_target, 4.));
    }
    if (!std::any_of(near_pylons.begin(),
        near_pylons.end(),
        [](const sc2::Unit* u) {return u->build_progress == 1.f; }))
    {
        //no pylons built
        return m_sc2.act().UnitCommand(m_unit, sc2::ABILITY_ID::MOVE, rand_point_around(m_unit->pos, 4.), true);
    }

    const auto photons = m_sc2.obs().GetUnits(
        type(sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON) && in_radius(m_target, pylon_radius));
    if (photons.empty())
    {
        build_near(m_sc2, m_unit, near_pylons.front()->pos, 4.f, sc2::ABILITY_ID::BUILD_PHOTONCANNON);
        m_closest_targets.pop_back();
        return;
    }
}

void CannonRush::Rusher::set_state(State newstate)
{
    m_state = newstate;
}

void CannonRush::unitCreated(const sc2::Unit* unit)
{
}

void CannonRush::buildingConstructionComplete(const sc2::Unit* unit)
{
    if (unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_FORGE)
    {
        m_forge_count++;
    }
}

void CannonRush::unitDestroyed(const sc2::Unit* unit)
{
    const auto found = std::find_if(m_rushers.begin()
        , m_rushers.end()
        , [&unit](const auto& rusher) {
            return rusher->unit()->tag == unit->tag;
        });
    if (found != m_rushers.end())
    {
        m_sc2.act().SendChat("Rusher destroyed");
        m_rushers.erase(found);
    }

    if (unit->unit_type == sc2::UNIT_TYPEID::PROTOSS_FORGE)
    {
        m_forge_count--;
    }
}

void CannonRush::assign_rushers()
{
    while (m_rushers.size() != rushers_count)
    {
        m_rushers.push_back(std::make_unique<Rusher>(this, get_free_probe(), m_sc2));
    }
}

const sc2::Unit* CannonRush::get_free_probe() const
{
    return m_sc2.obs().GetUnits([this](const sc2::Unit& unit)
        {
            if (unit.alliance == sc2::Unit::Enemy)
            {
                return false;
            }
            if (unit.unit_type != sc2::UNIT_TYPEID::PROTOSS_PROBE)
            {
                return false;
            }
            return m_rushers.end() == std::find_if(m_rushers.begin(), m_rushers.end()
                , [&unit](const auto& rusher) {
                    return rusher->unit()->tag == unit.tag;
                });
        }).front();
}

CannonRush::Rusher::Rusher(const CannonRush* parent
    , const sc2::Unit* rusher
    , SC2& sc2)
    : m_parent(parent)
    , m_unit(rusher)
    , m_sc2(sc2)
{
}

