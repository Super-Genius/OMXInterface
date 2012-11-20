#include <jni.h>


#include <binder/ProcessState.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/MediaBufferGroup.h>
#include <media/stagefright/MediaDebug.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/OMXClient.h>
#include <media/stagefright/OMXCodec.h>
#include <media/stagefright/MediaSource.h>
#include <utils/List.h>
#include <new>
#include "OMXInterface.h"
#include <stdio.h>
#include <math.h>

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

using namespace android;

class MemAudioSource : public MediaSource
{
private:
	void *m_SrcData;
	size_t m_SrcSize;
	size_t m_CurOffset;
	MediaBuffer *m_MediaBuffer;
	int32_t m_SamplingRate;
	int32_t m_ChannelCount;
	sp<MetaData> m_SourceFormat;

public:
   MemAudioSource(void *srcData, size_t srcSize, int32_t channelCount, int32_t samplingRate) :
	   m_SrcData(srcData),
	   m_SrcSize(srcSize),
	   m_ChannelCount(channelCount),
	   m_SamplingRate(samplingRate)
   {
	   m_CurOffset = 0;
	   m_MediaBuffer = new MediaBuffer(srcData, srcSize);
	   m_SourceFormat->setCString(kKeyMIMEType, MEDIA_MIMETYPE_AUDIO_AAC);
	   m_SourceFormat->setInt32(kKeySampleRate, m_SamplingRate);
	   m_SourceFormat->setInt32(kKeyChannelCount, m_ChannelCount);
	   m_SourceFormat->setInt32(kKeyMaxInputSize, m_SrcSize);
	   m_SourceFormat->setCString(kKeyDecoderComponent, "MemAudioSource");

   }

   virtual status_t read(
        MediaBuffer **buffer, const MediaSource::ReadOptions *options)
   {
	   *buffer = m_MediaBuffer;
	   return OK;
   }


    virtual sp<MetaData> getFormat() {
        return m_SourceFormat;
    }

    virtual status_t start(MetaData *params) {
        return OK;
    }

    virtual status_t stop() {
        return OK;
    }

};

OMXInterface *OMXInterface::_instance = NULL;

OMXInterface *OMXInterface::GetInstance()
{
	if (_instance == NULL)
	{
		_instance = new OMXInterface();
	}

	return _instance;
}

int OMXInterface::EncodeDecodeAudio(const char *mimeType, void *sourceData, size_t sourceSize, int32_t srcChannelCount, int32_t srcSampleRate, void **destData, size_t *destSize, bool encode)
{
    sp<MetaData> meta, outFormat;
    void *retBuffer = NULL;
    int retVal = OK;

    meta = new MetaData;
    if (meta == NULL)
    {
        return ENOMEM;
    }

    meta->setCString(kKeyMIMEType, mimeType);
    meta->setInt32(kKeyChannelCount, srcChannelCount);
    meta->setInt32(kKeySampleRate, srcSampleRate);

    OMXClient *m_Client = new OMXClient;
    sp<MediaSource> m_SourceAudio = new MemAudioSource(sourceData, sourceSize, srcChannelCount, srcSampleRate);
    sp<MediaSource> m_Decoder = OMXCodec::Create(m_Client->interface(), meta,
    		encode, m_SourceAudio, NULL);

    if (m_Decoder->start() ==  OK)
    {
        outFormat = m_Decoder->getFormat();

        MediaBuffer *outMediaBuffer = NULL;
        if (m_Decoder->read(&outMediaBuffer) == OK)
        {
        	size_t outSize = outMediaBuffer->size();
        	*destData = (void *)new char[outSize];
        	memcpy(*destData, outMediaBuffer->data(), outSize);
        	if (destSize != NULL)
        	{
        		*destSize = outSize;
        	}
        }
        else
        {
        	retVal = -2;
        }

    	m_Decoder->stop();
    }
    else
    {
    	retVal = -1;
    }

	m_Client->disconnect();

	return retVal;

}

void OMXInterface::MixSamples(void *src1Data, size_t source1Size, float mix1Value, void *src2Data, size_t source2Size, float mix2Value, int32_t nChannels, void *destData)
{
	uint16_t *src1 = (uint16_t *)src1Data;
	uint16_t *src2 = (uint16_t *)src2Data;
	uint16_t *dest = (uint16_t *)destData;
	size_t minSize = MIN(source1Size, source2Size);

	for (size_t i=0; i< minSize; i++)
	{
		for (int32_t channel=0; channel < nChannels; channel++)
		{
			float val1, val2;
			val1 = (float)(*src1++);
			val2 = (float)(*src2++);
			float mix = (mix1Value * val1) + (mix2Value * val2);
			mix = floor(mix + .5f);

			if (mix < 0.0f)
			{
				mix = 0.0f;
			}

			if (mix > 65535.0f)
			{
				mix = 65535.0f;
			}

			*dest++ = (uint16_t)mix;
		}
	}
}

