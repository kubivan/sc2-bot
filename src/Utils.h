#pragma once

#include <sc2api/sc2_common.h>
#include <sc2api/sc2_map_info.h>
#include <sc2api/sc2_unit.h>

class API;

void dump_pahting_grid(const sc2::ImageData& pathing_grid, const std::string& fname);

sc2::Point2D enemy_base_location(const API& api);

sc2::Point2D rand_point_near( const sc2::Point2D& center
                            , float radius
                            );

sc2::Point2D build_near(const API& api
    , const sc2::Unit* probe
    , const sc2::Point2D& center
    , float radius
    , sc2::ABILITY_ID building
    , bool queued = false);
