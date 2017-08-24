// Copyright (c) 2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "soundtouch-c.h"
#include <SoundTouch.h>

using soundtouch::SoundTouch;

#include <new>

SOUNDTOUCH_C_API ST_INSTANCE CDECL soundtouch_create()
{
	return (ST_INSTANCE)new SoundTouch;
}

SOUNDTOUCH_C_API void CDECL soundtouch_destroy(ST_INSTANCE st)
{
	delete (SoundTouch*)st;
}

SOUNDTOUCH_C_API const char* CDECL soundtouch_getVersionString(ST_INSTANCE st)
{
//return ((SoundTouch*)st)->getVersionString(st);
	return SoundTouch::getVersionString();
}

SOUNDTOUCH_C_API const unsigned int CDECL soundtouch_getVersionId(ST_INSTANCE st)
{
//return ((SoundTouch*)st)->getVersionId(st);
	return SoundTouch::getVersionId();
}

SOUNDTOUCH_C_API void CDECL soundtouch_setRate(ST_INSTANCE st, double newRate)
{
	((SoundTouch*)st)->setRate(newRate);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setTempo(ST_INSTANCE st, double newTempo)
{
	((SoundTouch*)st)->setTempo(newTempo);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setRateChange(ST_INSTANCE st, double newRate)
{
	((SoundTouch*)st)->setRateChange(newRate);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setTempoChange(ST_INSTANCE st, double newTempo)
{
	((SoundTouch*)st)->setTempoChange(newTempo);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setPitch(ST_INSTANCE st, double newPitch)
{
	((SoundTouch*)st)->setPitch(newPitch);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setPitchOctaves(ST_INSTANCE st, double newPitch)
{
	((SoundTouch*)st)->setPitchOctaves(newPitch);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setPitchSemiTones(ST_INSTANCE st, double newPitch)
{
	((SoundTouch*)st)->setPitchSemiTones(newPitch);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setPitchSemiTonesInt(ST_INSTANCE st, int newPitch)
{
	((SoundTouch*)st)->setPitchSemiTones(newPitch);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setChannels(ST_INSTANCE st, unsigned int numChannels)
{
	((SoundTouch*)st)->setChannels(numChannels);
}

SOUNDTOUCH_C_API void CDECL soundtouch_setSampleRate(ST_INSTANCE st, unsigned int srate)
{
	((SoundTouch*)st)->setSampleRate(srate);
}

SOUNDTOUCH_C_API double CDECL soundtouch_getInputOutputSampleRatio(ST_INSTANCE st)
{
	return ((SoundTouch*)st)->getInputOutputSampleRatio();
}

SOUNDTOUCH_C_API void CDECL soundtouch_flush(ST_INSTANCE st)
{
	((SoundTouch*)st)->flush();
}

SOUNDTOUCH_C_API void CDECL soundtouch_putSamples(ST_INSTANCE st, const sampletype_t *samples, unsigned int numSamples)
{
	((SoundTouch*)st)->putSamples(samples, numSamples);
}

SOUNDTOUCH_C_API unsigned int CDECL soundtouch_receiveSamples(ST_INSTANCE st, sampletype_t *output, unsigned int maxSamples)
{
	return ((SoundTouch*)st)->receiveSamples(output, maxSamples);
}

SOUNDTOUCH_C_API void CDECL soundtouch_clear(ST_INSTANCE st)
{
	((SoundTouch*)st)->clear();
}

SOUNDTOUCH_C_API bool CDECL soundtouch_setSetting(ST_INSTANCE st, SoundTouchC_Setting setting, int value)
{
	return ((SoundTouch*)st)->setSetting(setting, value);
}

SOUNDTOUCH_C_API int CDECL soundtouch_getSetting(ST_INSTANCE st, SoundTouchC_Setting setting)
{
	return ((SoundTouch*)st)->getSetting(setting);
}

SOUNDTOUCH_C_API unsigned int CDECL soundtouch_numUnprocessedSamples(ST_INSTANCE st)
{
	return ((SoundTouch*)st)->numUnprocessedSamples();
}

SOUNDTOUCH_C_API unsigned int CDECL soundtouch_numSamples(ST_INSTANCE st)
{
	return ((SoundTouch*)st)->numSamples();
}

SOUNDTOUCH_C_API int CDECL soundtouch_isEmpty(ST_INSTANCE st)
{
	return ((SoundTouch*)st)->isEmpty();
}

SOUNDTOUCH_C_API unsigned int CDECL numChannels(ST_INSTANCE st)
{
	return ((SoundTouch*)st)->numChannels();
}