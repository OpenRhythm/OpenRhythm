// Copyright (c) 2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.
#ifndef SOUNDTOUCH_C_H
#define SOUNDTOUCH_C_H

typedef void *ST_INSTANCE;

#ifdef SOUNDTOUCH_C_INTEGER_SAMPLES
typedef short sampletype_t;
#ifdef SOUNDTOUCH_C_FLOAT_SAMPLES
#error "Cannot use both Float and Integer sample types."
#endif
#else
typedef float sampletype_t;
#endif  // SOUNDTOUCH_C_INTEGER_SAMPLES

#ifdef _WIN32
#ifdef DLL_EXPORTS
#define SOUNDTOUCH_C_API __declspec(dllexport)
#else
#define SOUNDTOUCH_C_API __declspec(dllimport)
#endif
#else
#define SOUNDTOUCH_C_API __attribute__((visibility("default")))
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
	STC_USE_AA_FILTER = 0,
	STC_AA_FILTER_LENGTH,
	STC_USE_QUICKSEEK,
	STC_SEQUENCE_MS,
	STC_SEEKWINDOW_MS,
	STC_OVERLAP_MS,
	STC_NOMINAL_INPUT_SEQUENCE,
	STC_NOMINAL_OUTPUT_SEQUENCE,
	STC_INITIAL_LATENCY
} SoundTouchC_Setting;

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
SOUNDTOUCH_C_API bool CDECL soundtouch_setSetting(ST_INSTANCE st, SoundTouchC_Setting setting, int value);
SOUNDTOUCH_C_API int CDECL soundtouch_getSetting(ST_INSTANCE st, SoundTouchC_Setting setting);
SOUNDTOUCH_C_API unsigned int CDECL soundtouch_numUnprocessedSamples(ST_INSTANCE st);
SOUNDTOUCH_C_API unsigned int CDECL soundtouch_numSamples(ST_INSTANCE st);
SOUNDTOUCH_C_API int CDECL soundtouch_isEmpty(ST_INSTANCE st);

#ifdef __cplusplus

}

#endif


#endif /* #ifndef SOUNDTOUCH_C_H */
