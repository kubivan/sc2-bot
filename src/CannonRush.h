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
        bool has_forge() const;
        void rush() const;
        void set_state(State newstate) const;

        const sc2::Unit* m_rusher;
        mutable State m_state = State::Idle;
        const API* m_api;
        mutable std::vector<const sc2::Unit*> m_closest_targets;
        mutable sc2::Point2D m_target;
    };

    void assign_rushers();
    const sc2::Unit* get_free_probe();

    const API& m_api;

    std::set<Rusher> m_rushers;
};
