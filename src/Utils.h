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

