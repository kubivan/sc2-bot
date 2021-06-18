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
    constexpr const T& operator[](size_t index) const { return m_data[index]; }
    constexpr void push_back(const T& val)
    {
        if (m_size >= MaxSize)
        {
            throw std::range_error("FixedVector::push_back out of range");
        }
        m_data[m_size++] = val;
    }
    constexpr size_t size() const { return m_size; }

    constexpr bool count(const T& val) const
    {
        for (const auto& el : *this)
            if (val == el)
                return true;
        return false;
    }
private:
    std::array<T, MaxSize> m_data = {};
    size_t m_size = 0;
};

struct Footprint : public FixedVector<Point2DI, 25>
{
    int width;
    int height;
}; 

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
    res.width = W;
    res.height = H;
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            const auto delta = pattern[size_t(y*W) + x] == ' '
                ? Point2DI{ 0,0 } : Point2DI{ x - center.x, y - center.y };
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
        , std::make_pair(UNIT_TYPEID::PROTOSS_PYLON, make_footprint<2,2>("##"
                                                                         "#c"))
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
        std::ranges::find_if(data,
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
    constexpr auto footprint_map = ConstexprMap<UNIT_TYPEID, Footprint, 8>{
        get_all_footprints(), make_footprint<3,3>("###"
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

struct BuildingPlacerPattern
{
    static const int MAX_SQUARE = 10*10;
    static const int MAX_SLOTS = 5;
    constexpr BuildingPlacerPattern(const char* data
        , int width
        , int height
        , int slots_count
        , char fitter)
    : m_width(width)
    , m_height(height)
    , m_fitter(fitter)
    {
        assert(slots_count <= MAX_SLOTS);
        assert(width * height <= MAX_SQUARE);
        for (int i = 0; i < width * height; ++i)
        {
            this->m_data[i] = data[i];
        }

        FixedVector<Point2DI, 1024> visited;
        for (int i = 0; i < slots_count; ++i)
        {
            for (const auto&[type, footprint]: get_all_footprints())
            {
                if (!is_building_type(type))
                {
                    continue;
                }
                auto center = find_center(*this, footprint, visited);
                if (center)
                {
                    m_footprints.push_back(std::make_pair(*center, footprint));
                    break;
                }
            }
        }
    }

    constexpr char operator[](const Point2DI& pos) const noexcept
    {
        return m_data[static_cast<size_t>(pos.y) * m_width + pos.x];
    }

    constexpr int width() const noexcept { return m_width; }
    constexpr int height() const noexcept{ return m_height; }
    constexpr char fitter() const noexcept{ return m_fitter; }

    const auto& data() const { return m_data; }
    const auto& footprints() const { return m_footprints; }

private:
    using Visited = FixedVector<Point2DI, 1024>;
    //constexpr std::optional <std::pair<Point2DI, Visited>>
    //find_center(const BuildingPlacerPattern& pattern
    //    , const Footprint& footprint
    //    , const Visited& visited
    //)
    //{
    //    for (int y = 0; y < pattern.height(); y++)
    //    {
    //        for (int x = 0; x < pattern.width(); x++)
    //        {
    //            Point2DI center{ x,y };
    //            if (visited.count(center))
    //            {
    //                continue;
    //            }
    //            if (can_fit(footprint, center))
    //            {
    //                FixedVector<Point2DI, 1024> new_visited = visited;
    //                for (auto t : footprint)
    //                {
    //                    new_visited.push_back(Point2DI{ center.x + t.x, center.y + t.y });
    //                }
    //                return std::make_pair(center, new_visited);
    //            }
    //        }
    //    }
    //    return {};
    //}
    constexpr std::optional <Point2DI>
    find_center(const BuildingPlacerPattern& pattern
        , const Footprint& footprint
        , Visited& visited
    )
    {
        for (int y = 0; y < pattern.height(); y++)
        {
            for (int x = 0; x < pattern.width(); x++)
            {
                Point2DI center{ x,y };
                if (visited.count(center))
                {
                    continue;
                }
                if (can_fit(footprint, center))
                {
                    for (auto t : footprint)
                    {
                        visited.push_back(Point2DI{ center.x + t.x, center.y + t.y });
                    }
                    return center;
                }
            }
        }
        return {};
    }
    constexpr bool can_fit(const Footprint& footprint, const Point2DI& center) const
    {
        for (const auto& delta : footprint)
        {
            const auto tile = Point2DI{ center.x + delta.x, center.y + delta.y };
            if (tile.x < 0 || tile.x >= width())
                return false;
            if (tile.y < 0 || tile.y >= height())
                return false;
            if (auto pixel = this->operator[](tile); pixel != 'b')
            {
                return false;
            }
        }

        return true;
    }

    int m_width;
    int m_height;
    char m_fitter;
    FixedVector<char, MAX_SQUARE> m_data;
    FixedVector<std::pair<Point2DI, Footprint>, 5> m_footprints;
};

}
