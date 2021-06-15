#pragma once 

#include <sc2api/sc2_unit.h>
#include <array>

#include <sc2api/sc2_interfaces.h>

namespace sc2
{
class ObservationInterface;
}

namespace sc2::utils
{

constexpr bool is_building_type(sc2::UNIT_TYPEID type)
{
    switch (type)
    {
    // Protoss
    case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
    case UNIT_TYPEID::PROTOSS_ASSIMILATORRICH:
    case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
    case UNIT_TYPEID::PROTOSS_DARKSHRINE:
    case UNIT_TYPEID::PROTOSS_FLEETBEACON:
    case UNIT_TYPEID::PROTOSS_FORGE:
    case UNIT_TYPEID::PROTOSS_GATEWAY:
    case UNIT_TYPEID::PROTOSS_NEXUS:
    case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
    case UNIT_TYPEID::PROTOSS_PYLON:
    case UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED:
    case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
    case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
    case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
    case UNIT_TYPEID::PROTOSS_STARGATE:
    case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
    case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
    case UNIT_TYPEID::PROTOSS_WARPGATE:
        return true;
    }
    return false;
}

struct BuildingTraits
{
    sc2::Race race;
    int mineral_cost = 0; //mineral cost of the item
    int gas_cost = 0; //gas cost of the item
    int supply_cost = 0; //supply cost of the item
    int build_time = 0; //build time of the item
    sc2::AbilityID build_ability = 0; //the ability that creates this item
    sc2::AbilityID warp_ability = 0; //the ability that creates this item via warp-in
    std::vector<sc2::UnitTypeID> required_units; //owning ONE of these is required to make
    std::vector<sc2::UPGRADE_ID> required_upgrades; //having ALL of these is required to make
    int tile_width = 0;
};

using TechTree = std::unordered_map<sc2::UNIT_TYPEID, BuildingTraits>;

TechTree make_tech_tree(const sc2::ObservationInterface& obs);

/*constexpr*/ ABILITY_ID command(UNIT_TYPEID unit);
/*constexpr*/ ABILITY_ID command(UPGRADE_ID unit);

/*constexpr*/ UNIT_TYPEID producer(sc2::ABILITY_ID command);
/*constexpr*/ UNIT_TYPEID producer(sc2::UNIT_TYPEID unit);
/*constexpr*/ UNIT_TYPEID producer(sc2::UPGRADE_ID unit);

inline bool can_afford(sc2::UNIT_TYPEID item, sc2::utils::TechTree& tree, const sc2::ObservationInterface& obs)
{
    const auto minerals = obs.GetMinerals();
    const auto vespene = obs.GetVespene();
    const auto unit_traits = tree[item];
    return minerals >= unit_traits.mineral_cost && vespene >= unit_traits.gas_cost;
}

template<typename T, size_t MaxSize>
struct FixedVector
{
    static const int MAX_SIZE = MaxSize;
    constexpr FixedVector(size_t size = 0) : m_size(size) { }
    constexpr const T* begin() const { return &m_data[0]; }
    constexpr const T* end() const { return &m_data[m_size]; }
    constexpr T& operator[](size_t index) { return m_data[index]; }
    constexpr void push_back(const T& val)
    {
        if (m_size >= MaxSize)
        {
            throw std::range_error("FixedVector::push_back out of range");
        }
        m_data[m_size++] = val;
    }
    constexpr size_t size() const { return m_size; }
private:
    std::array<T, MaxSize> m_data;
    size_t m_size = 0;
};

using Footprint = FixedVector<Point2DI, 25>;

template<int W, int H>
constexpr Footprint
make_footprint(std::string_view pattern)
{
    static_assert(W * H <= Footprint::MAX_SIZE);

    auto center = Point2DI { -1, -1 };
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            if (pattern[size_t(y*W) + x] == 'c')
            {
                center.x = x;
                center.y = y;
            }
        }
    }

    auto res = Footprint(W * H);
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            const auto delta = pattern[size_t(y*W) + x] == ' '
                ? Point2DI{ 0,0 } : Point2DI{ x - center.x, center.y - y };
            res[size_t(y * W) + x] = delta;
        }
    }
    return res;
}

constexpr std::array < std::pair<sc2::UNIT_TYPEID, Footprint>, 8> get_all_footprints()
{
    constexpr std::array < std::pair<sc2::UNIT_TYPEID, Footprint>, 8> footprints =
    {
          std::make_pair(UNIT_TYPEID::PROTOSS_NEXUS, make_footprint<5,5>("#####"
                                                                         "#####"
                                                                         "##c##"
                                                                         "#####"
                                                                         "#####"))
        , std::make_pair(UNIT_TYPEID::PROTOSS_FORGE, make_footprint<3, 3>("###"
                                                                          "#c#"
                                                                          "###"))
        , std::make_pair(UNIT_TYPEID::PROTOSS_ASSIMILATOR, make_footprint<3,3>("###"
                                                                               "#c#"
                                                                               "###"))
        , std::make_pair(UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, make_footprint<3,3>("###"
                                                                                     "#c#"
                                                                                     "###"))
        , std::make_pair(UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, make_footprint<3,3>("###"
                                                                                     "#c#"
                                                                                     "###"))
        , std::make_pair(UNIT_TYPEID::PROTOSS_PYLON, make_footprint<2,2>("#c"
                                                                         "##"))
        , std::make_pair(UNIT_TYPEID::NEUTRAL_MINERALFIELD, make_footprint<2,1>("#c"))
        , std::make_pair(UNIT_TYPEID::NEUTRAL_MINERALFIELD750, make_footprint<2,1>("#c"))
    };
    return footprints;
}

template <typename Key, typename Value, std::size_t Size>
struct ConstexprMap {
  std::array<std::pair<Key, Value>, Size> data;
  Value default_value;

  [[nodiscard]] constexpr Value operator[](const Key &key) const
  {
    const auto itr =
        std::find_if(begin(data), end(data),
                     [&key](const auto &v) { return v.first == key; });
    if (itr != end(data))
    {
      return itr->second;
    }
    else
    {
        return default_value;
    }
  }

};

constexpr auto
get_footprint_map()
{
    constexpr std::array < std::pair<sc2::UNIT_TYPEID, Footprint>, 8> footprints =
    {
        std::make_pair(UNIT_TYPEID::PROTOSS_PYLON, make_footprint<2,2>("#c"
                                                                       "##"))
        , std::make_pair(UNIT_TYPEID::PROTOSS_FORGE, make_footprint<3, 3>("###"
                                                                          "#c#"
                                                                          "###"))
        , std::make_pair(UNIT_TYPEID::NEUTRAL_MINERALFIELD, make_footprint<2,1>("#c"))
        , std::make_pair(UNIT_TYPEID::NEUTRAL_MINERALFIELD750, make_footprint<2,1>("#c"))
        , std::make_pair(UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, make_footprint<3,3>("###"
                                                                                     "#c#"
                                                                                     "###"))
        , std::make_pair(UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER, make_footprint<3,3>("###"
                                                                                     "#c#"
                                                                                     "###"))
        , std::make_pair(UNIT_TYPEID::PROTOSS_ASSIMILATOR, make_footprint<3,3>("###"
                                                                               "#c#"
                                                                               "###"))
        , std::make_pair(UNIT_TYPEID::PROTOSS_NEXUS, make_footprint<5,5>("#####"
                                                                         "#####"
                                                                         "##c##"
                                                                         "#####"
                                                                         "#####"))
    };

    constexpr auto footprint_map = ConstexprMap<UNIT_TYPEID, Footprint, 8>{
        footprints, make_footprint<3,3>("###"
                                        "#c#"
                                        "###")
    };

    return footprint_map;
}

inline Footprint
get_footprint(UNIT_TYPEID type)
{
    static constexpr auto map = get_footprint_map();
    return map[type];
}

//5 building should be enough
using PlacerResult = FixedVector<std::pair<Point2DI, Footprint>, 5>;

{

struct BuildingPlacerPattern
{
    static const int MAX_SIZE = 1024;
    constexpr BuildingPlacerPattern(const char* data, int w, int h, int slots_count)
    : m_slots_count(slots_count)
    , m_width(w)
    , m_height(h)
    {
        for (int i = 0; i < w * h; ++i)
        {
            this->data[i] = data[i];
        }
    }

    constexpr char operator[](const Point2DI& pos) const noexcept
    {
        return data[(pos.y + origin.y) * m_width + pos.x + origin.x];
    }

    constexpr int width() const noexcept { return m_width; }
    constexpr int height() const noexcept{ return m_height; }
    constexpr int slots_count() const noexcept{ return m_slots_count; }

private:

    int m_width;
    int m_height;
    std::array<char, MAX_SIZE> data = {}; //TODO: check size
    int m_slots_count;
    Point2DI origin = { 0,0 };

};

constexpr bool can_fit(const BuildingPlacerPattern placer, const Footprint& footprint, const Point2DI& center)
{
    for (const auto& delta : footprint)
    {
        const auto tile = Point2DI{ center.x + delta.x, center.y + delta.y };
        if (tile.x < 0 || tile.x >= placer.width())
            return false;
        if (tile.y < 0 || tile.y >= placer.height())
            return false;
        if (auto pixel = placer[tile]; pixel != 'b')
        {
            return false;
        }
    }

    return true;
}

constexpr std::optional<Point2DI> find_center(const BuildingPlacerPattern& pattern
    , const Footprint& footprint
    , Point2DI start_point
)
{
    auto center = start_point;
    auto w = pattern.width();
    for (; center.y < pattern.height(); center.y++)
    {
        for (; center.x < pattern.width(); center.x++)
        {
            if (can_fit(pattern, footprint, center))
                return center;
        }
        center.x = 0;
    }
    return {};
}

constexpr PlacerResult make_placer(const BuildingPlacerPattern& pattern)
{
    PlacerResult res;
    std::optional<Point2DI> center;
    for (int i = 0; i < pattern.slots_count(); ++i)
    {
        const auto start_point = center.has_value() ? Point2DI{center->x + 1, center->y} : Point2DI{ 0,0 };
        for (const auto&[type, footprint]: get_all_footprints())
        {
            if (!is_building_type(type))
            {
                continue;
            }
            center = find_center(pattern, footprint, start_point);
            if (center)
            {
                res.push_back(std::make_pair(*center, footprint));
                break;
            }
        }
    }
    return res;
}

}
