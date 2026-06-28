#include "FmodManager.h"
#include <fmod_errors.h>
#include "../Input/Input.h"  
#include "../SharedTypes.h"
#include <cmath>


std::string FMODManager::MakePath(const std::string& file) const
{
    return m_audio.basePath + file;
}

void FMODManager::SetVehicleAudioDefinition(const AudioDefinition& audioDef)
{
    m_audio = audioDef;

    if (!system)
        return;

    LoadVehicleSounds();

    vehicleAudioEnabled = false; // force next Update to apply state
    SetVehicleAudioEnabled(false);

}

void FMODManager::InitAudio()
{
    FMOD_RESULT result;

    result = FMOD::System_Create(&system);
    OutputDebugStringA(FMOD_ErrorString(result));
    OutputDebugStringA("\n");

    result = system->init(512, FMOD_INIT_NORMAL, nullptr);
    OutputDebugStringA(FMOD_ErrorString(result));
    OutputDebugStringA("\n");

}

void FMODManager::LoadVehicleSounds()
{
    EngineSounds();
    TransmissionSounds();
    TyreSounds();
    WindSounds();
}

void FMODManager::LoadMenuSounds()
{
    FMOD_RESULT result;

    result = system->createSound(
        "UI/Sounds/Menu_click.wav",
        FMOD_2D | FMOD_CREATESAMPLE | FMOD_LOOP_OFF,
        nullptr,
        &menuClick);

    OutputDebugStringA(("Menu click: " + std::string(FMOD_ErrorString(result)) + "\n").c_str());

    result = system->createSound(
        "UI/Sounds/Menu_confirm.wav",
        FMOD_2D | FMOD_CREATESAMPLE | FMOD_LOOP_OFF,
        nullptr,
        &menuConfirm);

    OutputDebugStringA(("Menu confirm: " + std::string(FMOD_ErrorString(result)) + "\n").c_str());    
    
    result = system->createSound(
        "UI/Sounds/Menu_cancel.wav",
        FMOD_2D | FMOD_CREATESAMPLE | FMOD_LOOP_OFF,
        nullptr,
        &menuCancel);

    OutputDebugStringA(("Menu cancel: " + std::string(FMOD_ErrorString(result)) + "\n").c_str());    
    
    result = system->createSound(
        "UI/Sounds/Menu_start.wav",
        FMOD_2D | FMOD_CREATESAMPLE | FMOD_LOOP_OFF,
        nullptr,
        &menuStart);

    OutputDebugStringA(("Menu cancel: " + std::string(FMOD_ErrorString(result)) + "\n").c_str());    
    
    result = system->createSound(
        "UI/Sounds/Home.mp3",
        FMOD_LOOP_NORMAL,
        nullptr,
        &menuMusic);

    OutputDebugStringA(("Home: " + std::string(FMOD_ErrorString(result)) + "\n").c_str());
}

void FMODManager::PlayMenuClick()
{
    if (!system || !menuClick)
        return;

    system->playSound(menuClick, nullptr, false, &menuClickChannel);

    if (menuClickChannel)
        menuClickChannel->setVolume(1.0f);
}

void FMODManager::PlayMenuConfirm()
{
    OutputDebugStringA("PLAY MENU CONFIRM\n");

    if (!system)
    {
        OutputDebugStringA("system NULL\n");
        return;
    }

    if (!menuConfirm)
    {
        OutputDebugStringA("menuConfirm NULL\n");
        return;
    }

    FMOD_RESULT result =
        system->playSound(menuConfirm, nullptr, false, &menuConfirmChannel);

    OutputDebugStringA(("playSound: " + std::string(FMOD_ErrorString(result)) + "\n").c_str());

    if (menuConfirmChannel)
    {
        menuConfirmChannel->setVolume(1.0f);
        menuConfirmChannel->setMute(false);
        menuConfirmChannel->setPaused(false);
    }

    system->update();
}

void FMODManager::LoadJukebox()
{
    m_rng.seed(std::random_device{}());

    m_jukeboxTracks =
    {
        "UI/Music/track1.mp3",
        "UI/Music/track2.mp3",
        "UI/Music/track3.mp3",
        "UI/Music/track4.mp3",
        "UI/Music/track5.mp3",
    };
}

void FMODManager::PlayRandomTrack()
{
    if (!system || m_jukeboxTracks.empty())
        return;

    if (m_currentMusic)
    {
        m_currentMusic->release();
        m_currentMusic = nullptr;
    }

    std::uniform_int_distribution<int> dist(
        0,
        static_cast<int>(m_jukeboxTracks.size()) - 1);

    m_currentTrackIndex = dist(m_rng);

    system->createStream(
        m_jukeboxTracks[m_currentTrackIndex].c_str(),
        FMOD_2D | FMOD_DEFAULT,
        nullptr,
        &m_currentMusic);

    system->playSound(
        m_currentMusic,
        nullptr,
        false,
        &musicChannel);

    if (musicChannel)
        musicChannel->setVolume(0.2f);
}


void FMODManager::UpdateJukebox()
{
    if (!system)
        return;

    if (!musicChannel)
    {
        PlayRandomTrack();
        return;
    }

    bool playing = false;
    musicChannel->isPlaying(&playing);

    if (!playing)
    {
        PlayRandomTrack();
    }
}

void FMODManager::PlayMenuStart()
{
    OutputDebugStringA("PLAY MENU Start\n");

    if (!system)
    {
        OutputDebugStringA("system NULL\n");
        return;
    }

    if (!menuStart)
    {
        OutputDebugStringA("menuConfirm NULL\n");
        return;
    }

    FMOD_RESULT result =
        system->playSound(menuStart, nullptr, false, &menuStartChannel);

    OutputDebugStringA(("playSound: " + std::string(FMOD_ErrorString(result)) + "\n").c_str());

    if (menuStartChannel)
    {
        menuStartChannel->setVolume(1.0f);
        menuStartChannel->setMute(false);
        menuStartChannel->setPaused(false);
    }

    system->update();
}

void FMODManager::PlayMenuMusic()
{
    if (!system || !menuMusic)
        return;

    bool isPlaying = false;

    if (menuMusicChannel)
        menuMusicChannel->isPlaying(&isPlaying);

    if (isPlaying)
        return;

    FMOD_RESULT result =
        system->playSound(menuMusic, nullptr, false, &menuMusicChannel);

    OutputDebugStringA(("playSound: " + std::string(FMOD_ErrorString(result)) + "\n").c_str());

    if (menuMusicChannel)
    {
        menuMusicChannel->setVolume(0.6f);
        menuMusicChannel->setMute(false);
        menuMusicChannel->setPaused(false);
    }
}


void FMODManager::StopMenuMusic()
{
    if (menuMusicChannel)
    {
        menuMusicChannel->stop();
        menuMusicChannel = nullptr;
    }
}


void FMODManager::PlayMenuCancel()
{
    OutputDebugStringA("PLAY MENU CONFIRM\n");

    if (!system)
    {
        OutputDebugStringA("system NULL\n");
        return;
    }

    if (!menuCancel)
    {
        OutputDebugStringA("menuConfirm NULL\n");
        return;
    }

    FMOD_RESULT result =
        system->playSound(menuCancel, nullptr, false, &menuCancelChannel);

    OutputDebugStringA(("playSound: " + std::string(FMOD_ErrorString(result)) + "\n").c_str());

    if (menuCancelChannel)
    {
        menuCancelChannel->setVolume(1.0f);
        menuCancelChannel->setMute(false);
        menuCancelChannel->setPaused(false);
    }

    system->update();
}

void FMODManager::EngineSounds()
{
    FMOD_RESULT result;

    std::string idlePath = MakePath(m_audio.idle);
    std::string lowPath = MakePath(m_audio.lowOn);
    std::string midPath = MakePath(m_audio.midOn);
    std::string highPath = MakePath(m_audio.highOn);
    std::string topPath = MakePath(m_audio.topFull);
    std::string limiterPath = MakePath(m_audio.limiter);

    OutputDebugStringA(("Idle path: " + idlePath + "\n").c_str());

    result = system->createSound(idlePath.c_str(), FMOD_LOOP_NORMAL, nullptr, &idleIn);
    OutputDebugStringA(FMOD_ErrorString(result)); OutputDebugStringA("\n");

    result = system->createSound(lowPath.c_str(), FMOD_LOOP_NORMAL, nullptr, &lowOnIn);
    OutputDebugStringA(FMOD_ErrorString(result)); OutputDebugStringA("\n");

    result = system->createSound(midPath.c_str(), FMOD_LOOP_NORMAL, nullptr, &midOnIn);
    OutputDebugStringA(FMOD_ErrorString(result)); OutputDebugStringA("\n");

    result = system->createSound(highPath.c_str(), FMOD_LOOP_NORMAL, nullptr, &highOnIn);
    OutputDebugStringA(FMOD_ErrorString(result)); OutputDebugStringA("\n");

    result = system->createSound(topPath.c_str(), FMOD_LOOP_NORMAL, nullptr, &topFullIn);
    OutputDebugStringA(FMOD_ErrorString(result)); OutputDebugStringA("\n");

    result = system->createSound(limiterPath.c_str(), FMOD_LOOP_NORMAL, nullptr, &limiterIn);
    OutputDebugStringA(FMOD_ErrorString(result)); OutputDebugStringA("\n");

    system->playSound(idleIn, nullptr, true, &idleChannel);
    system->playSound(lowOnIn, nullptr, true, &lowChannel);
    system->playSound(midOnIn, nullptr, true, &midChannel);
    system->playSound(highOnIn, nullptr, true, &highChannel);
    system->playSound(topFullIn, nullptr, true, &topChannel);
    system->playSound(limiterIn, nullptr, true, &limiterChannel);

    if (idleChannel) idleChannel->setVolume(0.0f);
    if (lowChannel) lowChannel->setVolume(0.0f);
    if (midChannel) midChannel->setVolume(0.0f);
    if (highChannel) highChannel->setVolume(0.0f);
    if (topChannel) topChannel->setVolume(0.0f);
    if (limiterChannel) limiterChannel->setVolume(0.0f);

}

void FMODManager::TransmissionSounds()
{
    std::string transmissionPath = MakePath(m_audio.transmission);
    std::string gearUpPath = MakePath(m_audio.gearUp);
    std::string gearDownPath = MakePath(m_audio.gearDown);

    system->createSound(
        transmissionPath.c_str(),
        FMOD_LOOP_NORMAL,
        nullptr,
        &transmission);

    system->createSound(
        gearUpPath.c_str(),
        FMOD_DEFAULT,
        nullptr,
        &gearUp);

    system->createSound(
        gearDownPath.c_str(),
        FMOD_DEFAULT,
        nullptr,
        &gearDown);

    system->playSound(
        transmission,
        nullptr,
        true,
        &transmissionChannel);

    if (transmissionChannel)
        transmissionChannel->setVolume(0.0f);
}

void FMODManager::PlayGearUp()
{
    if (!system || !gearUp)
        return;

    system->playSound(
        gearUp,
        nullptr,
        false,          // play immediately
        &gearUpChannel);

    if (gearUpChannel)
        gearUpChannel->setVolume(1.0f);
}

void FMODManager::PlayGearDown()
{
    if (!system || !gearDown)
        return;

    system->playSound(
        gearDown,
        nullptr,
        false,          // play immediately
        &gearDownChannel);

    if (gearDownChannel)
        gearDownChannel->setVolume(1.0f);
}

void FMODManager::TyreSounds()
{
    std::string rollingPath = MakePath(m_audio.tyreRolling);
    std::string skidPath = MakePath(m_audio.tyreSkid);

    system->createSound(rollingPath.c_str(), FMOD_LOOP_NORMAL, nullptr, &tyreRolling);
    system->createSound(skidPath.c_str(), FMOD_LOOP_NORMAL, nullptr, &tyreSkid);

    system->playSound(tyreRolling, nullptr, true, &tyreRollingChannel);
    system->playSound(tyreSkid, nullptr, true, &tyreSkidChannel);

    if (tyreRollingChannel) tyreRollingChannel->setVolume(0.0f);
    if (tyreSkidChannel) tyreSkidChannel->setVolume(0.0f);

}

void FMODManager::WindSounds()
{
    std::string windPath = MakePath(m_audio.wind);

    system->createSound(windPath.c_str(), FMOD_LOOP_NORMAL, nullptr, &wind);
    system->playSound(wind, nullptr, true, &windChannel);

    if (windChannel)
        windChannel->setVolume(0.0f);

}



static float ClampFloat(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static float Bell(float rpm, float center, float width)
{
    float x = std::fabs(rpm - center) / width;
    return ClampFloat(1.0f - x, 0.0f, 1.0f);
}

void FMODManager::SetVehicleAudioEnabled(bool enabled)
{
    if (idleChannel) idleChannel->setPaused(!enabled);
    if (lowChannel) lowChannel->setPaused(!enabled);
    if (midChannel) midChannel->setPaused(!enabled);
    if (highChannel) highChannel->setPaused(!enabled);
    if (topChannel) topChannel->setPaused(!enabled);
    if (limiterChannel) limiterChannel->setPaused(!enabled);
    if (transmissionChannel) transmissionChannel->setPaused(!enabled);
    if (tyreRollingChannel) tyreRollingChannel->setPaused(!enabled);
    if (tyreSkidChannel) tyreSkidChannel->setPaused(!enabled);
    if (windChannel) windChannel->setPaused(!enabled);
}

void FMODManager::Update(EngineState state, float rpm, float throttle, float speedKmh, float avgSlipRatio, float avgSlipAngle)
{
    if (!system)
        return;

    bool gameplay = state == EngineState::GAMEPLAY;

    if (gameplay != vehicleAudioEnabled)
    {
        vehicleAudioEnabled = gameplay;
        SetVehicleAudioEnabled(vehicleAudioEnabled);
    }

    if (!vehicleAudioEnabled)
    {
        system->update();
        return;
    }

    // Pitch each sample around the RPM it was recorded at
    if (idleChannel) idleChannel->setPitch(ClampFloat(rpm / m_audio.idleRPM, 0.5f, 2.0f));
    if (lowChannel)  lowChannel->setPitch(ClampFloat(rpm / m_audio.lowRPM, 0.5f, 2.0f));
    if (midChannel)  midChannel->setPitch(ClampFloat(rpm / m_audio.midRPM, 0.5f, 2.0f));
    if (highChannel) highChannel->setPitch(ClampFloat(rpm / m_audio.highRPM, 0.5f, 2.0f));
    if (topChannel)  topChannel->setPitch(ClampFloat(rpm / m_audio.topRPM, 0.85f, 1.15f));

    // Volume blending by RPM
    float idleVol = Bell(rpm, m_audio.idleRPM, m_audio.idleWidth);
    float lowVol = Bell(rpm, m_audio.lowRPM, m_audio.lowWidth);
    float midVol = Bell(rpm, m_audio.midRPM, m_audio.midWidth);
    float highVol = Bell(rpm, m_audio.highRPM, m_audio.highWidth);
    float topVol = Bell(rpm, m_audio.topRPM, m_audio.topWidth);
    float limiterStart = m_audio.topRPM;
    float limiterAmount =
        ClampFloat(
            (rpm - limiterStart) / 250.0f,
            0.0f,
            1.0f);

    float speedFactor = ClampFloat(speedKmh / 250.0f, 0.0f, 1.0f);
    float rpmFactor = ClampFloat(rpm / 7500.0f, 0.0f, 1.0f);

    float transVolume = 0.12f * speedFactor * rpmFactor;
    float transPitch = ClampFloat(0.75f + rpmFactor * 0.75f, 0.75f, 1.5f);

    float speedTyreFactor = ClampFloat(speedKmh / 180.0f, 0.0f, 1.0f);
    float speedWindFactor = ClampFloat(speedKmh / 260.0f, 0.0f, 1.0f);

    float slipAmount =
        ClampFloat((std::fabs(avgSlipRatio) + std::fabs(avgSlipAngle) * 2.0f) / 1.0f, 0.0f, 1.0f);

    if (tyreRollingChannel)
    {
        tyreRollingChannel->setVolume(0.75f * speedTyreFactor);
        tyreRollingChannel->setPitch(0.75f + speedTyreFactor * 0.5f);
    }

    if (tyreSkidChannel)
    {
        tyreSkidChannel->setVolume(0.4f * slipAmount);
        tyreSkidChannel->setPitch(0.9f + speedTyreFactor * 0.3f);
    }

    if (windChannel)
    {
        float windVolume = 0.8f * speedWindFactor * speedWindFactor;
        float windPitch = 0.75f + speedWindFactor * 0.65f;

        windChannel->setVolume(windVolume);
        windChannel->setPitch(windPitch);
    }


    static bool lastShiftUp = false;
    static bool lastShiftDown = false;

    bool shiftUpPressed = Input::GetCurrentInputs().shiftUp && !lastShiftUp;
    bool shiftDownPressed = Input::GetCurrentInputs().shiftDown && !lastShiftDown;

    if (shiftUpPressed)
        PlayGearUp();    
    
    if (shiftDownPressed)
        PlayGearDown();

    lastShiftUp = Input::GetCurrentInputs().shiftUp;
    lastShiftDown = Input::GetCurrentInputs().shiftDown;

    float onAmount = 1.0f;

    if (idleChannel) idleChannel->setVolume(0.35f * idleVol);
    if (lowChannel)  lowChannel->setVolume(0.60f * lowVol * onAmount);
    if (midChannel)  midChannel->setVolume(0.70f * midVol * onAmount);
    if (highChannel) highChannel->setVolume(0.80f * highVol * onAmount);

    if (limiterChannel)
    {
        limiterChannel->setPitch(1.0f);
        limiterChannel->setVolume(1.5f * limiterAmount);
    }

    if (transmissionChannel)
    {
        transmissionChannel->setVolume(transVolume);
        transmissionChannel->setPitch(transPitch);
    }

    if (topChannel)
        topChannel->setVolume(topVol);

    system->update();

}

void FMODManager::ShutdownAudio()
{
    if (idleIn)
    {
        idleIn->release();
        idleIn = nullptr;
    } 

    if (lowOnIn)
    {
        lowOnIn->release();
        lowOnIn = nullptr;
    } 
    
    if (midOnIn)
    {
        midOnIn->release();
        midOnIn = nullptr;
    }   
    
    if (highOnIn)
    {
        highOnIn->release();
        highOnIn = nullptr;
    }   
    
    if (topFullIn)
    {
        topFullIn->release();
        topFullIn = nullptr;
    }  
    
    if (limiterIn)
    {
        limiterIn->release();
        limiterIn = nullptr;
    }   
    
    if (transmission)
    {
        transmission->release();
        transmission = nullptr;
    }   
    
    if (gearUp)
    {
        gearUp->release();
        gearUp = nullptr;
    }    

    if (gearDown)
    {
        gearDown->release();
        gearDown = nullptr;
    }   
       
    
    if (tyreSkid)
    {
        tyreSkid->release();
        tyreSkid = nullptr;
    }     
    
    if (tyreRolling)
    {
        tyreRolling->release();
        tyreRolling = nullptr;
    }    
    
    if (wind)
    {
        wind->release();
        wind = nullptr;
    }   
    
    if (idleChannel)
    {
        idleChannel = nullptr;
    } 
    
    if (lowChannel)
    {
        lowChannel = nullptr;
    } 
    
    if (midChannel)
    {
        midChannel = nullptr;
    }  
    
    if (highChannel)
    {
        highChannel = nullptr;
    }   
    
    if (topChannel)
    {
        topChannel = nullptr;
    }  
    
    if (limiterChannel)
    {
        limiterChannel = nullptr;
    }

    if (transmissionChannel)
    {
        transmissionChannel = nullptr;
    }   
    
    if (gearUpChannel)
    {
        gearUpChannel = nullptr;
    } 
    
    if (gearDownChannel)
    {
        gearDownChannel = nullptr;
    }
      
    
    if (tyreRollingChannel)
    {
        tyreRollingChannel = nullptr;
    }   
    
    if (tyreSkidChannel)
    {
        tyreSkidChannel = nullptr;
    }   
    
    if (windChannel)
    {
        windChannel = nullptr;
    }

   


    if (system)
    {
        system->close();
        system->release();
        system = nullptr;
    }

}