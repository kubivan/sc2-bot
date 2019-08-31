
#pragma once

#include <EventListener.h>

#include <sc2api/sc2_common.h>
#include <set>
#include <glm/vec2.hpp>

class SC2;

class CannonRush : public EventListener
{
public:
	~CannonRush();
	CannonRush(SC2& api);

	void step() override;

	void unitCreated(const sc2::Unit* unit) override;

	void buildingConstructionComplete(const sc2::Unit* unit) override;

	void unitDestroyed(const sc2::Unit*) override;

	bool has_forge() const
	{
		return m_forge_count;
	}

	const sc2::Unit* get_free_probe() const;

private:
	const size_t rushers_count = 2;
	class Rusher
	{
	public:
		static const auto vision = 8;
		enum class State { Idle, Scouting, Wander, Rushing };
		Rusher(const CannonRush* parent, const sc2::Unit* rusher, SC2& sc2);

		void step();
		const sc2::Unit* unit() const;
	private:
		void rush();
		void scout();
		void set_state(State newstate);

		const CannonRush* m_parent;
		const sc2::Unit* m_unit;
		SC2& m_sc2;
		State m_state = State::Idle;

		sc2::Point3D m_heading;

		sc2::Point2D m_target;
		std::vector<const sc2::Unit*> m_closest_targets;
	};

	void assign_rushers();

	SC2& m_sc2;

	std::vector<std::unique_ptr<Rusher>> m_rushers;
	size_t m_forge_count = 0;
};
