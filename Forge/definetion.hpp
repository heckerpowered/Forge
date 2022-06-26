#pragma once

#include <jni.h>

#define VMCALL native

namespace sdk
{
	using index = __int64;
	JNIEXPORT jclass JNICALL JVM_DefineClass(JNIEnv* env, char const* name, jobject loader, jbyte const* buf, jsize len, jobject pd);
}