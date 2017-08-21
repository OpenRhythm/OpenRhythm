// Copyright (c) 2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.
#ifndef SOUNDTOUCH_C_H
#define SOUNDTOUCH_C_H

typedef void *ST_INSTANCE;

// Might cause issues with underlying SoundTouch header? Not sure yet.
#ifdef SOUNDTOUCH_INTEGER_SAMPLES
typedef short sampletype_t;
#ifdef SOUNDTOUCH_FLOAT_SAMPLES
#error "Cannot use both Float and Integer sample types."
#endif
#else
typedef float sampletype_t;
#endif  // SOUNDTOUCH_INTEGER_SAMPLES

#ifdef _WIN32
#ifdef DLL_EXPORTS
#define SOUNDTOUCH_C_API __declspec(dllexport)
#else
#define SOUNDTOUCH_C_API __declspec(dllimport)
#endif
#else
#define SOUNDTOUCH_C_API
#endif // _WIN32


#ifdef __cplusplus
#define CDECL
extern "C" {
#else
#ifdef _WIN32
#define CDECL __cdecl
#else
#define CDECL
#endif
#endif // __cplusplus

typedef enum
{
	USE_AA_FILTER = 0,
	AA_FILTER_LENGTH,
	USE_QUICKSEEK,
	SEQUENCE_MS,
	SEEKWINDOW_MS,
	OVERLAP_MS,
	NOMINAL_INPUT_SEQUENCE,
	NOMINAL_OUTPUT_SEQUENCE,
	INITIAL_LATENCY
} SoundTouch_Setting;

SOUNDTOUCH_C_API ST_INSTANCE CDECL soundtouch_create();
SOUNDTOUCH_C_API void CDECL soundtouch_destroy(ST_INSTANCE st);

SOUNDTOUCH_C_API const char* CDECL soundtouch_getVersionString(ST_INSTANCE st);
SOUNDTOUCH_C_API const unsigned int CDECL soundtouch_getVersionId(ST_INSTANCE st);

SOUNDTOUCH_C_API void CDECL soundtouch_setRate(ST_INSTANCE st, double newRate);
SOUNDTOUCH_C_API void CDECL soundtouch_setTempo(ST_INSTANCE st, double newTempo);
SOUNDTOUCH_C_API void CDECL soundtouch_setRateChange(ST_INSTANCE st, double newRate);
SOUNDTOUCH_C_API void CDECL soundtouch_setTempoChange(ST_INSTANCE st, double newTempo);
SOUNDTOUCH_C_API void CDECL soundtouch_setPitch(ST_INSTANCE st, double newPitch);
SOUNDTOUCH_C_API void CDECL soundtouch_setPitchOctaves(ST_INSTANCE st, double newPitch);
SOUNDTOUCH_C_API void CDECL soundtouch_setPitchSemiTones(ST_INSTANCE st, double newPitch);
SOUNDTOUCH_C_API void CDECL soundtouch_setPitchSemiTonesInt(ST_INSTANCE st, int newPitch);
SOUNDTOUCH_C_API void CDECL soundtouch_setChannels(ST_INSTANCE st, unsigned int numChannels);
SOUNDTOUCH_C_API void CDECL soundtouch_setSampleRate(ST_INSTANCE st, unsigned int srate);
SOUNDTOUCH_C_API double CDECL soundtouch_getInputOutputSampleRatio(ST_INSTANCE st);
SOUNDTOUCH_C_API void CDECL soundtouch_flush(ST_INSTANCE st);
SOUNDTOUCH_C_API void CDECL soundtouch_putSamples(ST_INSTANCE st, const sampletype_t *samples, unsigned int numSamples);
SOUNDTOUCH_C_API unsigned int CDECL soundtouch_receiveSamples(ST_INSTANCE st, sampletype_t *output, unsigned int maxSamples);
SOUNDTOUCH_C_API void CDECL soundtouch_clear(ST_INSTANCE st);
SOUNDTOUCH_C_API bool CDECL soundtouch_setSetting(ST_INSTANCE st, SoundTouch_Setting setting, int value);
SOUNDTOUCH_C_API int CDECL soundtouch_getSetting(ST_INSTANCE st, SoundTouch_Setting setting);
SOUNDTOUCH_C_API unsigned int CDECL soundtouch_numUnprocessedSamples(ST_INSTANCE st);
SOUNDTOUCH_C_API unsigned int CDECL soundtouch_numSamples(ST_INSTANCE st);
SOUNDTOUCH_C_API int CDECL soundtouch_isEmpty(ST_INSTANCE st);

#ifdef __cplusplus

}

#else


#endif /* #ifndef SOUNDTOUCH_C_H */