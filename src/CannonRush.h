#pragma once

#include <EventListener.h>

#include <sc2api/sc2_common.h>
#include <set>

class API;

class CannonRush : public EventListener
{
public: 
    ~CannonRush();
    CannonRush(const API& api);

    void step() override;

    void unitCreated(const sc2::Unit* unit) override;

    void buildingConstructionComplete(const sc2::Unit* unit) override;

    void unitDestroyed(const sc2::Unit*) override;

private:
    const size_t rushers_count = 1;
    struct Rusher
    {
        enum class State {Idle, Scouting, Rushing};
        Rusher( const sc2::Unit* rusher);

        bool operator<(const Rusher& other) const;
        void step(const API& api) const;

        const sc2::Unit* rusher;
        mutable State state = State::Idle;
    };

    void assign_rushers();
    const sc2::Unit* get_free_probe();

    const API& m_api;

    std::set<Rusher> m_rushers;
};
