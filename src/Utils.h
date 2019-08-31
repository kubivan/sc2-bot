#pragma once

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_map_info.h>
#include <sc2api/sc2_unit.h>

class SC2;
void dump_pahting_grid(const sc2::ImageData& pathing_grid, const std::string& fname);

sc2::Point2D enemy_base_location(const SC2& api);

sc2::Point2D rand_point_near( const sc2::Point2D& center
                            , float radius
                            );

sc2::Point2D
rand_point_around(const sc2::Point2D& center
	, float radius, float divisor = 1.f);

sc2::Point2D build_near(SC2& api
    , const sc2::Unit* probe
    , const sc2::Point2D& center
    , float radius
    , sc2::ABILITY_ID building
    , bool queued = false);

#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

inline sc2::Point2D seek(sc2::Point2D target_pos, sc2::Point2D seeker_pos)
{
	const auto max_speed = 5.f;
	const auto speed = 3.f;
	auto desired_velocity = target_pos - seeker_pos;
	sc2::Normalize2D(desired_velocity);
	desired_velocity *= max_speed;

	return (desired_velocity /*- m_pVehicle->Velocity()*/);
}

inline sc2::Point2D PointToWorldSpace(const sc2::Point2D& point,
	const glm::vec2& heading,
	const float angle,
	const sc2::Point2D& agent_pos)
{
	glm::mat3 transform;
	transform = glm::rotate(transform, angle);
	transform = glm::translate(transform, { agent_pos.x, agent_pos.y });
	const auto transformed = transform * glm::vec3(point.x, point.y, 0);

	////make a copy of the point
	//sc2::Point2D TransPoint = point;

	////create a transformation matrix
	//C2DMatrix matTransform;

	////rotate
	//matTransform.Rotate(AgentHeading, AgentSide);

	////and translate
	//matTransform.Translate(AgentPosition.x, AgentPosition.y);

	////now transform the vertices
	//matTransform.TransformVector2Ds(TransPoint);

	return {transformed.x, transformed.y};
}

#include <functional>
#include <sc2api/sc2_unit.h>
#include <sc2api/sc2_interfaces.h>

class UnitQuery
{
public:
    friend UnitQuery& operator <<(UnitQuery& q, sc2::Filter filter);
    friend sc2::Units run(UnitQuery&);


    UnitQuery(const sc2::ObservationInterface& obs)
        : m_obs(obs)
    {
    }



private:
    const sc2::ObservationInterface& m_obs;
    std::vector<sc2::Filter> m_filters;
};

inline sc2::Units run(UnitQuery& query)
{
    auto& filters = query.m_filters;
    return query.m_obs.GetUnits([&filters](const auto& u)
    {
        return std::all_of(filters.begin()
            , filters.end()
            , [&u](auto& pred) {return pred(u); });
    });
}

inline UnitQuery& operator <<(UnitQuery& q, sc2::Filter filter)
{
    q.m_filters.push_back(std::move(filter));
    return q;
}

template <class P> UnitQuery& operator<<(UnitQuery& q, P p)
{
    return q.m_filters.push_back([&p](const auto& u) {retunr p(u)});
}

inline sc2::Filter alliance(sc2::Unit::Alliance alliance)
{
    return [&alliance](const auto& u) { return u.alliance == alliance; };
}

inline sc2::Filter type(sc2::UNIT_TYPEID type)
{
    return [&type](const auto& u) { return u.unit_type == type; };
}

//bool all()
//{
//    return true;
//}

//template<typename Arg> inline
//bool all(Arg arg)
//{
//    return 
//}

template<typename... Args> inline
bool all(Args... args)
{


}

//template<typename... args> inline
//bool all(const sc2::unit& u, args... args) 
//{
//    using namespace std::placeholders;
//    return (... && std::bind(args, _1, u)); 
//}

template<typename... Args> inline
sc2::Filter all_filter(Args... args) 
{
    using namespace std::placeholders;
    return[...args](const auto& u)
    {
        return (... && std::bind(args, _1, u)); 
    }
}

inline bool is_some(const sc2::Unit& u)
{
    return true;
}

inline sc2::Units all_ally_pylons(sc2::ObservationInterface& obs)
{
    auto filter = [](const auto& u) { return all(u, is_some, is_some); };

    return obs.GetUnits(filter);
}
