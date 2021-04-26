#ifdef GRADRIGO_DLL
#define GRADRIGO_API extern "C" __declspec(dllexport)
#else
#define GRADRIGO_API extern "C" 
#endif

typedef int GRADRIGO_INSTANCE_ID;

// Initialize the Gradrigo in "singleton" mode
GRADRIGO_API GRADRIGO_INSTANCE_ID Initialize(int sample_rate);

// Create a new instance of gradrigo for multi-gradrigo setup
GRADRIGO_API GRADRIGO_INSTANCE_ID NewInstance(int sample_rate);
// Destroy a gradrigo in multi-gradrigo steup (can't be used on singleton gradrigos)
GRADRIGO_API void DestroyInstance(GRADRIGO_INSTANCE_ID id);

GRADRIGO_API int GetBuffer(int num_samples, float *out, GRADRIGO_INSTANCE_ID id = 0);
GRADRIGO_API int ParseString(const char* gradrigo_source_code, GRADRIGO_INSTANCE_ID id = 0);
GRADRIGO_API int StartVoice(const char* box_name, GRADRIGO_INSTANCE_ID id = 0);
GRADRIGO_API void ReleaseVoice(int voice_id, GRADRIGO_INSTANCE_ID id = 0);
GRADRIGO_API void StopVoice(int voice_id, GRADRIGO_INSTANCE_ID id = 0);
GRADRIGO_API void StopAllVoices(GRADRIGO_INSTANCE_ID id = 0);
GRADRIGO_API const char* GetResponseString(int request_id, GRADRIGO_INSTANCE_ID id = 0);
GRADRIGO_API void MidiNoteOn(int note, int velocity, GRADRIGO_INSTANCE_ID id = 0);
GRADRIGO_API void MidiNoteOff(int note, GRADRIGO_INSTANCE_ID id = 0);
GRADRIGO_API void SetVariable(const char *variable_name, float value, GRADRIGO_INSTANCE_ID id = 0);
GRADRIGO_API void ResetEngine(GRADRIGO_INSTANCE_ID id = 0);
GRADRIGO_API const char* ReportBoxesAsJson(GRADRIGO_INSTANCE_ID id = 0);
