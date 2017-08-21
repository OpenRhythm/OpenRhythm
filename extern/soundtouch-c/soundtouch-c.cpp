// Copyright (c) 2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "soundtouch-c.h"
#include <SoundTouch.h>

#include <new>

SOUNDTOUCH_C_API ST_INSTANCE CDECL soundtouch_create()
{
	return (ST_INSTANCE)new SoundTouch();
}

SOUNDTOUCH_C_API void CDECL soundtouch_destroy(ST_INSTANCE st)
{
	delete (SoundTouch*)st;
}

SOUNDTOUCH_C_API const char* CDECL soundtouch_getVersionString(ST_INSTANCE st)
{
	return ((SoundTouch*)st)->getVersionString(st);
}

SOUNDTOUCH_C_API const unsigned int CDECL soundtouch_getVersionId(ST_INSTANCE st)
{
	return ((SoundTouch*)st)->getVersionId(st);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setRate(ST_INSTANCE st, double newRate)
{
	((SoundTouch*)st)->setRate(st, newRate);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setTempo(ST_INSTANCE st, double newTempo)
{
	((SoundTouch*)st)->setTempo(st, newTempo);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setRateChange(ST_INSTANCE st, double newRate)
{
	((SoundTouch*)st)->setRateChange(st, newRate);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setTempoChange(ST_INSTANCE st, double newTempo)
{
	((SoundTouch*)st)->setTempoChange(st, newTempo);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setPitch(ST_INSTANCE st, double newPitch)
{
	((SoundTouch*)st)->setPitch(st, newPitch);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setPitchOctaves(ST_INSTANCE st, double newPitch)
{
	((SoundTouch*)st)->setPitchOctaves(st, newPitch);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setPitchSemiTones(ST_INSTANCE st, double newPitch)
{
	((SoundTouch*)st)->setPitchSemiTones(st, newPitch);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setPitchSemiTonesInt(ST_INSTANCE st, int newPitch)
{
	((SoundTouch*)st)->setPitchSemiTonesInt(st, newPitch);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setChannels(ST_INSTANCE st, unsigned int numChannels)
{
	((SoundTouch*)st)->setChannels(st, numChannels);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setSampleRate(ST_INSTANCE st, unsigned int srate)
{
	((SoundTouch*)st)->setSampleRate(st, srate);
}

SOUNDTOUCH_C_API double CDECL soundtouch_getInputOutputSampleRatio(ST_INSTANCE st)
{
	return ((SoundTouch*)st)->getInputOutputSampleRatio(st);
}

SOUNDTOUCH_C_API void CDECL soundtouch_flush(ST_INSTANCE st)
{
	((SoundTouch*)st)->flush(st);
}

SOUNDTOUCH_C_API void CDECL soundtouch_putSamples(ST_INSTANCE st, const sampletype_t *samples, unsigned int numSamples)
{
	((SoundTouch*)st)->putSamples(st, amples, numSamples);
}

SOUNDTOUCH_C_API unsigned int CDECL soundtouch_receiveSamples(ST_INSTANCE st, sampletype_t *output, unsigned int maxSamples)
{
	return ((SoundTouch*)st)->receiveSamples(st, utput, maxSamples);
}

SOUNDTOUCH_C_API void CDECL soundtouch_clear(ST_INSTANCE st)
{
	((SoundTouch*)st)->clear(st);
}

SOUNDTOUCH_C_API bool CDECL soundtouch_setSetting(ST_INSTANCE st, SoundTouch_Setting setting, int value)
{
	return ((SoundTouch*)st)->setSetting(st, setting, value);
}

SOUNDTOUCH_C_API int CDECL soundtouch_getSetting(ST_INSTANCE st, SoundTouch_Setting setting)
{
	return ((SoundTouch*)st)->getSetting(st, setting);
}

SOUNDTOUCH_C_API unsigned int CDECL soundtouch_numUnprocessedSamples(ST_INSTANCE st)
{
	return ((SoundTouch*)st)->numUnprocessedSamples(st);
}

SOUNDTOUCH_C_API unsigned int CDECL soundtouch_numSamples(ST_INSTANCE st)
{
	return ((SoundTouch*)st)->numSamples(st);
}

SOUNDTOUCH_C_API int CDECL soundtouch_isEmpty(ST_INSTANCE st)
{
	return ((SoundTouch*)st)->isEmpty(st);
}
