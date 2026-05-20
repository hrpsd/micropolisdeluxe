// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2022 - 2024 Leeor Dicker
// Copyright © 2025 - 2026 Sylvain Nowé
//
// Portions Copyright © 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#include "main.h"
#include "w_tk.h"
#include "gameOptions.h"
#include "w_sound.h"

#include <string>
#include <map>

#include "SDL_include.h"
#include SDL_INCLUDE_MIXER


typedef std::map<std::string, Mix_Chunk*> ChunksMap;

ChunksMap soundsChunks;


/* Sound routines */


static bool SoundInitialized = false;
static bool UserSoundOn = true;
int Dozing = 0;
bool musicIsPlaying = false;


bool userSoundOn()
{
    return UserSoundOn;
}

void userSoundOn(bool val)
{
	UserSoundOn = val;
}



void InitializeSound()
{
    SoundInitialized = true;

	Mix_OpenAudio(22050, AUDIO_U16SYS, 1, 4096);

    if (gameOptions.mPlayMusic)
    {
        PlayMusic();
    }
}

void PlayMusic() {
    Mix_Music* music = Mix_LoadMUS((resourcesPath + "/sounds/micropolis.ogg").c_str());

    Mix_VolumeMusic(12);
    Mix_PlayMusic(music, -1);

    musicIsPlaying = true;
}

void StopMusic()
{
    Mix_HaltMusic();

    musicIsPlaying = false;
}

void ShutDownSound()
{
    if (SoundInitialized)
    {
        // unload sound samples here
    }
}


void MakeSound(const std::string& channel, const std::string& id)
{
    if (!UserSoundOn)
    {
        return;
    }
    if (!SoundInitialized)
    {
        InitializeSound();
    }

    Eval("UIMakeSound \"" + channel + "\" \"" + id + "\"");
	
	ChunksMap::iterator pos = soundsChunks.find(id);
	
	if (pos == soundsChunks.end()) {
		printf("Loading SFX %s\n", id.c_str());
		std::string _id = id;
		for (auto& c : _id)
		{
		   c = tolower(c);
		}
		
		Mix_Chunk *wave = Mix_LoadWAV((resourcesPath + "/sounds/" + _id + ".wav").c_str());
		
		soundsChunks[id] = wave;
	}

	if (Mix_PlayChannel(-1, soundsChunks[id], 0) == -1)
	{
		printf("Waves sound could not be played!\n"
			   "SDL_Error: %s\n", SDL_GetError());
		//Mix_FreeChunk(soundsChunks[id]);
		//exit(1);
	}
}


void MakeSoundOn(SimView* view, const char* channel, const char* id)
{
    char buf[256]{};

    if (!UserSoundOn)
    {
        return;
    }
    if (!SoundInitialized)
    {
        InitializeSound();
    }

    //sprintf(buf, "UIMakeSoundOn %s \"%s\" \"%s\"", /*Tk_PathName(view->tkwin)*/ "window-path", channel, id);
	printf("UIMakeSoundOn \"%s\" \"%s\"\n", channel, id);
    Eval(buf);
	
	MakeSound(channel, id);
}


void SoundOff()
{
    if (!SoundInitialized)
    {
        InitializeSound();
    }
    Eval("UISoundOff");
    Dozing = 0;
}


void DoStartSound(const std::string& channel, const std::string& id)
{
    Eval("UIStartSound " + channel + " " + id);
}


void DoStopSound(const char* id)
{
    Eval("UIStopSound " + std::string(id));
}


void StartBulldozer()
{
    if (!UserSoundOn)
    {
        return;
    }
    if (!SoundInitialized)
    {
        InitializeSound();
    }
    if (!Dozing)
    {
        DoStartSound("edit", "1");
        Dozing = 1;
    }
}


void StopBulldozer()
{
    if ((!UserSoundOn) || (!SoundInitialized))
    {
        return;
    }

    DoStopSound("1");
    Dozing = 0;
}
