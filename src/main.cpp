#include <sc2api/sc2_api.h>

#include <sc2utils/sc2_manage_process.h>
#include <Kubot.h>

int main(int argc, char* argv[]) {
	sc2::Coordinator coordinator;
	coordinator.LoadSettings(argc, argv);

	Kubot bot;
	coordinator.SetParticipants({
		sc2::CreateParticipant(sc2::Race::Protoss, &bot),
		sc2::CreateComputer(sc2::Race::Protoss, sc2::Difficulty::HardVeryHard)
		});

	coordinator.LaunchStarcraft();
	coordinator.StartGame(sc2::kMapBelShirVestigeLE);

	while (coordinator.Update()) 
	{
		sc2::SleepFor(15);
	}

	return 0;
}
