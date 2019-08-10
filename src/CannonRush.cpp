#include "CannonRush.h"

#include <Utils.h>
#include <iostream>

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
    m_closest_enemies = api->obs->GetUnits([&](const sc2::Unit& u) {
        return u.alliance == sc2::Unit::Enemy; 
    });
    switch (state)
    {
    case Rusher::State::Idle:
        api->actions->UnitCommand(rusher, sc2::ABILITY_ID::STOP);

        target = enemy_base_location(*api);
        state = State::Scouting;
        break;

    case Rusher::State::Scouting:
        if (!m_closest_enemies.empty())
        {
            state = State::Rushing;
            break;
        }
        if (!rusher->orders.empty())
        {
            break;
        }
        if (sc2::DistanceSquared2D(target, sc2::Point2D{ rusher->pos }) > 5*5 )
        {
            api->actions->UnitCommand(rusher, sc2::ABILITY_ID::MOVE, target);
        }
        else
        {
            state = Rusher::State::Rushing;
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

void CannonRush::Rusher::rush() const 
{
    if (!rusher->orders.empty())
    {
        return;
    }
    if (m_closest_enemies.empty())
    {
        state = State::Scouting;
        return;
    }
    const auto forges = api->obs->GetUnits(sc2::IsUnit(sc2::UNIT_TYPEID::PROTOSS_FORGE));
    if (forges.empty() || forges.front()->build_progress != 1.f)
    {
        api->actions->UnitCommand(rusher, sc2::ABILITY_ID::MOVE, rand_point_near(rusher->pos, 5.), true);
    }
    else
    {
        const auto enemy = m_closest_enemies.front();
        const auto near_pylons = api->obs->GetUnits([&](auto& u) {
            return u.unit_type == sc2::UNIT_TYPEID::PROTOSS_PYLON
                && sc2::DistanceSquared2D(enemy->pos, u.pos) < 4.f * 4.f;
        });
        if (near_pylons.size() < 2)
        {
            api->actions->UnitCommand(rusher, sc2::ABILITY_ID::BUILD_PYLON, rand_point_near(enemy->pos, 4.));
        }
        else
        {
            build_near(*api, rusher, near_pylons.front()->pos, 4.f, sc2::ABILITY_ID::BUILD_PHOTONCANNON);
        }
    }
}

void CannonRush::unitCreated(const sc2::Unit* unit)
{
}

void CannonRush::buildingConstructionComplete(const sc2::Unit* unit)
{
}

void CannonRush::unitDestroyed(const sc2::Unit* unit)
{
    std::cout << "Unit destroyed" << std::endl;
    if (m_rushers.erase(unit))
    {
        std::cout << "Rusher destroyed. " << std::endl;
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
    : rusher(rusher)
    , api(api)
{
}

bool CannonRush::Rusher::operator<(const Rusher & other) const
{
    return rusher < other.rusher;
}
