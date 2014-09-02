/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TO_STRING_ARRAY_W_H_included
#define TO_STRING_ARRAY_W_H_included

#include "jni.h"
#include "ScopedLocalRef.h"

#include <string>
#include <vector>

jobjectArray newStringArray(JNIEnv* env, size_t count);

template <typename Counter, typename Getter>
jobjectArray toStringArrayW(JNIEnv* env, Counter* counter, Getter* getter) {
    size_t count = (*counter)();
    jobjectArray result = newStringArray(env, count);
    if (result == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < count; ++i) {
        const std::wstring& cur_str = (*getter)(i);
        ScopedLocalRef<jstring> s(env, env->NewString(cur_str.c_str(), cur_str.length()));
        if (env->ExceptionCheck()) {
            return NULL;
        }
        env->SetObjectArrayElement(result, i, s.get());
        if (env->ExceptionCheck()) {
            return NULL;
        }
    }
    return result;
}

struct VectorCounterW {
    const std::vector<std::wstring>& strings;
    VectorCounterW(const std::vector<std::wstring>& strings) : strings(strings) {}
    size_t operator()() {
        return strings.size();
    }
};
struct VectorGetterW {
    const std::vector<std::wstring>& strings;
    VectorGetterW(const std::vector<std::wstring>& strings) : strings(strings) {}
    const std::wstring& operator()(size_t i) {
        return strings[i];
    }
};

inline jobjectArray toStringArrayW(JNIEnv* env, const std::vector<std::wstring>& strings) {
    VectorCounterW counter(strings);
    VectorGetterW getter(strings);
    return toStringArrayW<VectorCounterW, VectorGetterW>(env, &counter, &getter);
}

//JNIEXPORT jobjectArray toStringArray(JNIEnv* env, const char* const* strings);

#endif  // TO_STRING_ARRAY_W_H_included
