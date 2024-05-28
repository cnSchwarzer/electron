// Copyright (c) 2023 Salesforce, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_RENDERER_API_ELECTRON_API_WEB_UTILS_H_
#define ELECTRON_SHELL_RENDERER_API_ELECTRON_API_WEB_UTILS_H_

#include "v8/include/v8.h"

namespace electron::api::web_utils {

std::string GetPathForFile(v8::Isolate* isolate, v8::Local<v8::Value> file);
void ImportSharedTextureForGpuDevice(
    const v8::FunctionCallbackInfo<v8::Value>& info);

}  // namespace electron::api::web_utils

#endif  // ELECTRON_SHELL_RENDERER_API_ELECTRON_API_WEB_UTILS_H_
