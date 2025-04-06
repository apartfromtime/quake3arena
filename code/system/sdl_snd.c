/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "../client/snd_local.h"
#include "../win32/win_local.h"

static SDL_AudioStream* s_stream = NULL;
static long s_bufferSize = 0;
static int s_dmapos = 0;
static bool sound_init = false;

// Callback function for SDL. Writes sound data to SDL when requested.
static void
SDL_SDL3Callback(void* userdata, SDL_AudioStream* stream, int additional_amount,
	int total_amount)
{
	if (additional_amount > 0) {
		Uint8* data = SDL_stack_alloc(Uint8, additional_amount);
		if (data != NULL) {
			int length1;
			int length2;
			int pos = (s_dmapos * (dma.samplebits / 8));

			if (pos >= s_bufferSize) {
				s_dmapos = pos = 0;
			}

			// shouldn't happen
			if (!sound_init) {
				memset(data, '\0', additional_amount);
				return;
			}

			int tobufferend = s_bufferSize - pos;

			if (additional_amount > tobufferend) {
				length1 = tobufferend;
				length2 = additional_amount - length1;
			} else {
				length1 = additional_amount;
				length2 = 0;
			}

			memcpy(data, dma.buffer + pos, length1);

			// Set new position
			if (length2 <= 0) {
				s_dmapos += (length1 / (dma.samplebits / 8));
			} else {
				memcpy(data + length1, dma.buffer, length2);
				s_dmapos = (length2 / (dma.samplebits / 8));
			}

			if (s_dmapos >= s_bufferSize) {
				s_dmapos = 0;
			}

			SDL_PutAudioStreamData(stream, data, additional_amount);
			SDL_stack_free(data);
		}
	}
}

/*
==================
SNDDMA_InitSDL
==================
*/
bool SNDDMA_InitSDL(void)
{
	Com_Printf("Initializing audio subsystem\n");

	if (!SDL_WasInit(SDL_INIT_AUDIO)) {
		if (!SDL_Init(SDL_INIT_AUDIO)) {
			Com_Printf("Couldn't init SDL audio: %s.\n", SDL_GetError());

			return false;
		}
	}

	const char* drivername = SDL_GetCurrentAudioDriver();
	if (drivername == NULL) {
		drivername = "(UNKNOWN)";
	}

	Com_Printf("SDL audio driver is \"%s\".\n", drivername);

	SDL_AudioSpec spec = { 0 };

	if (s_khz->integer == 44) {
		spec.freq = 44100;
	} else if (s_khz->integer == 22) {
		spec.freq = 22050;
	} else {
		spec.freq = 11025;
	}

	spec.format = SDL_AUDIO_S16;
	spec.channels = 2;

	s_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec,
		SDL_SDL3Callback, NULL);

	if (s_stream == NULL) {
		Com_Printf("SDL_OpenAudioDeviceStream() failed: %s\n", SDL_GetError());
		SDL_QuitSubSystem(SDL_INIT_AUDIO);

		return false;
	}

	int samples = 0, bufferSize = 0, val = 0;

	if (spec.freq <= 11025) {
		samples = 256;
	} else if (spec.freq <= 22050) {
		samples = 512;
	} else if (spec.freq <= 44100) {
		samples = 1024;
	} else {
		samples = 2048;
	}

	bufferSize = (samples * spec.channels) * 10;

	if (bufferSize & (bufferSize - 1)) {
		// make it a power of two
		val = 1;
		while (val < bufferSize)
			val <<= 1;

		bufferSize = val;
	}

	s_bufferSize = bufferSize;
	s_dmapos = 0;

	dma.channels = spec.channels;
	dma.samplebits = SDL_AUDIO_BITSIZE(spec.format);
	dma.speed = spec.freq;
	dma.samples = s_bufferSize / SDL_AUDIO_BYTESIZE(spec.format);
	dma.submission_chunk = 1;
	dma.buffer = Z_Malloc(s_bufferSize);

	if (dma.buffer) {
		Com_Memset(dma.buffer, 0, s_bufferSize);
	}

	SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(s_stream));

	return true;
}

/*
==================
SNDDMA_Shutdown
==================
*/
void SNDDMA_Shutdown(void)
{
	Com_DPrintf("Shutting down sound system\n");

	SDL_PauseAudioDevice(SDL_GetAudioStreamDevice(s_stream));
	SDL_DestroyAudioStream(s_stream);
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	Z_Free(dma.buffer);

	s_dmapos = s_bufferSize = 0;
	sound_init = false;
}

/*
==================
SNDDMA_Init

Initialize sdl sound
Returns false if failed
==================
*/
bool SNDDMA_Init(void)
{
	Com_Memset((void*)&dma, 0, sizeof(dma));

	sound_init = false;

	if (!SNDDMA_InitSDL()) {
		return false;
	}

	sound_init = true;

	Com_DPrintf("Completed successfully\n");

	return true;
}

/*
==============
SNDDMA_GetDMAPos

return the current sample position (in mono samples read)
inside the recirculating dma buffer, so the mixing code will know
how many sample are required to fill it up.
===============
*/
int SNDDMA_GetDMAPos(void)
{
	return s_dmapos;
}

/*
=================
SNDDMA_Activate

When we change windows we need to do this
=================
*/
void SNDDMA_Activate(void)
{
	SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(s_stream));
}
