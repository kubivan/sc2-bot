#include "CannonRush.h"

#include <Utils.h>
#include <iostream>
#include <sstream>
#include <numeric>


constexpr float pylon_radius = 6.5f;
constexpr float pylon_radius_squared = pylon_radius*pylon_radius;

bool is_building(const sc2::Unit& u)
{
    return  u.unit_type != sc2::UNIT_TYPEID::PROTOSS_ADEPT
        && u.unit_type != sc2::UNIT_TYPEID::PROTOSS_PROBE
        && u.unit_type != sc2::UNIT_TYPEID::PROTOSS_ZEALOT
        && u.unit_type != sc2::UNIT_TYPEID::PROTOSS_STALKER;
}

sc2::Units get_enemy_buildings(const API& api)
{
    return api.obs->GetUnits([&api](const sc2::Unit& u)
    {
        return is_building(u);
    });
}

CannonRush::~CannonRush()
{
}

CannonRush::CannonRush(const API& api)
    : m_api(api)
{
}

void CannonRush::step()
{
    assign_rushers();

    for (const auto& rusher : m_rushers)
    {
        rusher.step();
    }
}

void CannonRush::Rusher::step() const
{
    if (m_closest_targets.empty())
    {
        m_closest_targets = m_api->obs->GetUnits([&](const sc2::Unit& u) {
            return u.alliance == sc2::Unit::Enemy && is_building(u);
        });
    }
    switch (m_state)
    {
    case Rusher::State::Idle:
        m_api->actions->UnitCommand(m_rusher, sc2::ABILITY_ID::STOP);

        m_target = enemy_base_location(*m_api);
        return set_state(State::Scouting);
        break;

    case Rusher::State::Scouting:
        if (!m_rusher->orders.empty())
        {
            break;
        }
        if (sc2::DistanceSquared2D(m_target, sc2::Point2D{ m_rusher->pos }) > 5*5 )
        {
            m_api->actions->UnitCommand(m_rusher, sc2::ABILITY_ID::MOVE, m_target);
        }
        else if (has_forge() && m_api->obs->GetMinerals() > 150)
        {
            return set_state(Rusher::State::Rushing);
        }
        else
        {
            m_api->actions->UnitCommand(m_rusher, sc2::ABILITY_ID::MOVE, rand_point_near(m_rusher->pos, 5.), true);
        }
        break;
    case Rusher::State::Rushing:
    {
        rush();
        break;
    }
    default:
        assert(false);
        break;
    }
}

bool CannonRush::Rusher::has_forge() const
{
    const auto forges = m_api->obs->GetUnits(sc2::IsUnit(sc2::UNIT_TYPEID::PROTOSS_FORGE));
    return !forges.empty() && forges.front()->build_progress == 1.f;
}

void CannonRush::Rusher::rush() const
{
    if (!m_rusher->orders.empty())
    {
        return;
    }
    if (m_api->obs->GetMinerals() < 150)
    {
        return set_state(State::Scouting);
    }
    if (m_closest_targets.empty())
    {
        return set_state(State::Scouting);
    }

    m_target = m_closest_targets.back()->pos;
    const auto near_pylons = m_api->obs->GetUnits([&](auto& u) {
        return u.unit_type == sc2::UNIT_TYPEID::PROTOSS_PYLON
            && sc2::DistanceSquared2D(m_target, u.pos) < pylon_radius_squared;
    });
    if (near_pylons.size() < 2)
    {
        return m_api->actions->UnitCommand(m_rusher, sc2::ABILITY_ID::BUILD_PYLON, rand_point_near(m_target, 4.));
    }
    if (!std::any_of(near_pylons.begin(),
        near_pylons.end(),
        [](const sc2::Unit* u) {return u->build_progress == 1.f; }))
    {
        //no pylons built
       return m_api->actions->UnitCommand(m_rusher, sc2::ABILITY_ID::MOVE, rand_point_near(m_rusher->pos, 5.), true);
    }

    const auto photons = m_api->obs->GetUnits([&](auto& u) {
        return u.unit_type == sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON
            && sc2::DistanceSquared2D(m_target, u.pos) < pylon_radius_squared;
    });
    if (photons.empty())
    {
        build_near(*m_api, m_rusher, near_pylons.front()->pos, 4.f, sc2::ABILITY_ID::BUILD_PHOTONCANNON);
        return;
    }
    m_closest_targets.pop_back();
}

void CannonRush::Rusher::set_state(State newstate) const
{
    std::stringstream ss;
    ss << "Change state from " << (int)m_state << " to " << (int)newstate;
    m_api->actions->SendChat(ss.str());
    m_state = newstate;
}

void CannonRush::unitCreated(const sc2::Unit* unit)
{
}

void CannonRush::buildingConstructionComplete(const sc2::Unit* unit)
{
}

void CannonRush::unitDestroyed(const sc2::Unit* unit)
{
    if (m_rushers.erase(unit))
    {
        m_api.actions->SendChat("Rusher destroyed!");
    }
}

void CannonRush::assign_rushers()
{
    while (m_rushers.size() != rushers_count)
    {
        m_rushers.emplace(get_free_probe(), &m_api);
    }
}

const sc2::Unit* CannonRush::get_free_probe()
{
    return m_api.obs->GetUnits([this](const sc2::Unit& unit)
    {
        return unit.unit_type == sc2::UNIT_TYPEID::PROTOSS_PROBE 
              && !this->m_rushers.count(&unit);
    }).front();
}

CannonRush::Rusher::Rusher(const sc2::Unit* rusher, const API* api)
    : m_rusher(rusher)
    , m_api(api)
{
}

bool CannonRush::Rusher::operator<(const Rusher & other) const
{
    return m_rusher < other.m_rusher;
}
