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
        rusher.step(m_api);
    }
}

void CannonRush::Rusher::step(const API& api) const
{
    const auto enemy_base = enemy_base_location(api);
    const auto distance = sc2::Distance2D(enemy_base, sc2::Point2D{ rusher->pos });
    //std::cout << (state == State::Scouting ? "SCOUTING" : "RUSHING") << " distance " << distance << std::endl;
    switch (state)
    {
    case Rusher::State::Idle:
        api.actions->UnitCommand(rusher, sc2::ABILITY_ID::STOP);
        state = State::Scouting;
        break;

    case Rusher::State::Scouting:
        if (!rusher->orders.empty())
        {
            break;
        }
        if (distance > 5 )
        {
            api.actions->UnitCommand(rusher, sc2::ABILITY_ID::MOVE, enemy_base);
        }
        else
        {
            state = Rusher::State::Rushing;
            step(api);
        }
        break;
    case Rusher::State::Rushing:
    {
        if (!rusher->orders.empty())
        {
            return;
        }
        const auto forges = api.obs->GetUnits(sc2::IsUnit(sc2::UNIT_TYPEID::PROTOSS_FORGE));
        if (forges.empty())
        {
            api.actions->UnitCommand(rusher, sc2::ABILITY_ID::MOVE, rand_point_near(rusher->pos, 5.), true);
        }
        else if(forges.front()->build_progress == 1.f)
        {
            const auto near_pylons = api.obs->GetUnits([&](auto& u) {
                return u.unit_type == sc2::UNIT_TYPEID::PROTOSS_PYLON
                    && sc2::DistanceSquared2D(rusher->pos, u.pos) < 8.f; 
            });
            if (near_pylons.size() < 2)
            {
                api.actions->UnitCommand(rusher, sc2::ABILITY_ID::BUILD_PYLON, rand_point_near(rusher->pos, 5.));
            }
            else
            {
                api.actions->UnitCommand(rusher, sc2::ABILITY_ID::BUILD_PHOTONCANNON, rand_point_near(near_pylons.front()->pos, 5.));
            }
        }

        break;
    }
    default:
        assert(false);
        break;
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
        m_rushers.insert(get_free_probe());
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

CannonRush::Rusher::Rusher(const sc2::Unit* rusher)
    : rusher(rusher)
{
}

bool CannonRush::Rusher::operator<(const Rusher & other) const
{
    return rusher < other.rusher;
}
