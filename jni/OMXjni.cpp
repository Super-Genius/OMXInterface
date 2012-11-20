
#include <stdint.h>
#include <jni.h>
#include "OMXInterface.h"
#include <stdio.h>
#define OK	0


void Java_com_omxinterface_OMXInterface_EncodeDecodeAudio(JNIEnv* jenv, jobject thiz, jstring mimeType, jbyteArray srcData, jint nChannels, jint sampleRate, jbyteArray destData, jboolean encode)
{

	jbyte* srcBuffer = jenv->GetByteArrayElements(srcData, NULL);
	jsize srcLength = jenv->GetArrayLength(srcData);

	jsize destLength;
	void *destBuffer;

	const jchar *sMimeType = jenv->GetStringChars(mimeType, NULL);
	if (OMXInterface::GetInstance()->EncodeDecodeAudio((const char *)sMimeType, (void *)srcBuffer, (size_t)srcLength, nChannels, sampleRate, &destBuffer, (size_t *)&destLength, encode) == OK)
	{
		destData = jenv->NewByteArray(destLength);
		jenv->SetByteArrayRegion(destData, 0, destLength, (const signed char *)destBuffer);
	}
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv *env;
	LOG_INFO("JNI_OnLoad called");
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK)
	{
		LOG_ERROR("Failed to get the environment using GetEnv()");
		return -1;
	}

	return JNI_VERSION_1_6;
}
