#include "SC2.h"


SC2::SC2(sc2::Agent& agent)
	: m_obs(*agent.Observation())
	, m_actions(*agent.Actions())
	, m_query(*agent.Query())
	, m_debug(*agent.Debug())
{
}

const sc2::ObservationInterface& SC2::obs() const
{
	return m_obs;
}

sc2::ActionInterface& SC2::act()
{
	return m_actions;
}

sc2::QueryInterface& SC2::query()
{
	return m_query;
}

sc2::DebugInterface& SC2::debug()
{
	return m_debug;
}
