#pragma once
#include <fmod_studio.hpp>
#include <fmod.hpp>
#include "../Cars/Porsche_911_Carrera_S/Porsche911.h"
#include <string>
#include <vector>
#include <random>

enum class EngineState;

class FMODManager {
public:
	void InitAudio();
	void EngineSounds();
	void TransmissionSounds();
	void PlayGearUp();
	void PlayGearDown();
	void PlayMenuMusic();
	void PlayMenuClick();
	void PlayMenuConfirm();
	void PlayMenuCancel();
	void PlayMenuStart();
	void StopMenuMusic();
	void TyreSounds();
	void LoadMenuSounds();
	void WindSounds();
	void SetVehicleAudioEnabled(bool enabled);
	void ShutdownAudio();
	void Update(EngineState state, float rpm, float throttle, float speedKmh, float avgSlipRatio, float avgSlipAngle);
	void SetVehicleAudioDefinition(const AudioDefinition& audioDef);


	void LoadJukebox();
	void UpdateJukebox();
	void PlayRandomTrack();
	void StopJukebox();


	std::vector<std::string> m_jukeboxTracks;
	std::mt19937 m_rng;
	int m_currentTrackIndex = -1;
private:

	FMOD::System* system = nullptr;
	FMOD::Sound* idleIn = nullptr;
	FMOD::Sound* lowOnIn = nullptr;
	FMOD::Sound* midOnIn = nullptr;
	FMOD::Sound* highOnIn = nullptr;
	FMOD::Sound* topFullIn = nullptr;
	FMOD::Sound* limiterIn = nullptr;
	FMOD::Sound* transmission = nullptr;
	FMOD::Sound* gearUp = nullptr;
	FMOD::Sound* gearDown = nullptr;
	FMOD::Sound* tyreRolling = nullptr;
	FMOD::Sound* tyreSkid = nullptr;
	FMOD::Sound* wind = nullptr;
	FMOD::Sound* menuClick = nullptr;
	FMOD::Sound* menuConfirm = nullptr;
	FMOD::Sound* menuCancel = nullptr;
	FMOD::Sound* menuStart = nullptr;
	FMOD::Sound* menuMusic = nullptr;
	FMOD::Sound* m_currentMusic = nullptr;

	FMOD::Channel* idleChannel = nullptr;
	FMOD::Channel* lowChannel = nullptr;
	FMOD::Channel* midChannel = nullptr;
	FMOD::Channel* highChannel = nullptr;
	FMOD::Channel* topChannel = nullptr;
	FMOD::Channel* limiterChannel = nullptr;
	FMOD::Channel* transmissionChannel = nullptr;
	FMOD::Channel* gearUpChannel = nullptr;
	FMOD::Channel* gearDownChannel = nullptr;
	FMOD::Channel* tyreRollingChannel = nullptr;
	FMOD::Channel* tyreSkidChannel = nullptr;
	FMOD::Channel* windChannel = nullptr;
	FMOD::Channel* menuClickChannel = nullptr;
	FMOD::Channel* menuConfirmChannel = nullptr;
	FMOD::Channel* menuCancelChannel = nullptr;
	FMOD::Channel* menuStartChannel = nullptr;
	FMOD::Channel* menuMusicChannel = nullptr;
	FMOD::Channel* musicChannel = nullptr;





	bool vehicleAudioEnabled = false;

	AudioDefinition m_audio;

	void LoadVehicleSounds();

	std::string MakePath(const std::string& file) const;

};


