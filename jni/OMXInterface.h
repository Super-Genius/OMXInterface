


#ifndef _OMXINTERFACE_H_
#define _OMXINTERFACE_H_

class OMXInterface
{
private:
	OMXInterface() { };
	static OMXInterface *_instance;
public:
	static OMXInterface *GetInstance();

	int EncodeDecodeAudio(const char *mimeType, void *sourceData, size_t sourceSize, int32_t srcChannelCount, int32_t srcSampleRate, void **destData, size_t *destSize, bool encode = false);

	void MixSamples(void *src1Data, size_t source1Size, float mix1Value, void *src2Data, size_t source2Size, float mix2Value, int32_t nChannels, void *destData);

};

#ifdef DEBUG
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, __FILE__, __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, __FILE__, __VA_ARGS__)
#else
#define LOG_INFO(...)
#define LOG_ERROR(...)
#endif

#endif
