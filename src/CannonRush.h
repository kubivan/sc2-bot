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
    class Rusher 
    {
    public:
        enum class State {Idle, Scouting, Rushing};
        Rusher( const sc2::Unit* rusher, const API* api = nullptr);
        
        bool operator<(const Rusher& other) const;
        void step() const;
    private:
        void rush() const;

        const sc2::Unit* rusher;
        mutable State state = State::Idle;
        const API* api;
        mutable std::vector<const sc2::Unit*> m_closest_enemies;
        mutable sc2::Point2D target;
    };

    void assign_rushers();
    const sc2::Unit* get_free_probe();

    const API& m_api;

    std::set<Rusher> m_rushers;
};
