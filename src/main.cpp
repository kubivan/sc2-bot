#include <sc2api/sc2_api.h>

#include <sc2utils/sc2_manage_process.h>
#include <Kubot.h>

#include <utils/UnitTraits.h>

constexpr auto ramp_pattern =
"//bbb "
"//bbb "
"/ bbb "
"bbb   "
"bbb bb"
"bbb bb";

int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    constexpr auto pattern_test = sc2::utils::BuildingPlacerPattern{ ramp_pattern, 6, 6, 3 };
    constexpr auto placement_test = sc2::utils::make_placer(pattern_test);

    Kubot bot;
    coordinator.SetParticipants({
        sc2::CreateParticipant(sc2::Race::Protoss, &bot)
        ,sc2::CreateComputer(sc2::Race::Protoss, sc2::Difficulty::VeryEasy)
        });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    while (coordinator.Update())
    {
       //sc2::SleepFor(5);
    }

    return 0;
}
