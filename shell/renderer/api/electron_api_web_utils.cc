// Copyright (c) 2023 Salesforce, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/renderer/api/electron_api_web_utils.h"

#include "dawn/webgpu.h"
#include "dawn/webgpu_cpp.h"
#include "shell/common/gin_helper/dictionary.h"
#include "shell/common/gin_helper/error_thrower.h"
#include "shell/common/node_includes.h"
#include "third_party/blink/public/web/web_blob.h"
#include "third_party/blink/renderer/bindings/modules/v8/v8_gpu_device.h"
#include "third_party/blink/renderer/bindings/modules/v8/v8_gpu_texture.h"
#include "third_party/blink/renderer/modules/webgpu/gpu_device.h"
#include "third_party/blink/renderer/modules/webgpu/gpu_texture.h"

namespace electron::api::web_utils {

std::string GetPathForFile(v8::Isolate* isolate, v8::Local<v8::Value> file) {
  blink::WebBlob blob = blink::WebBlob::FromV8Value(isolate, file);
  if (blob.IsNull()) {
    gin_helper::ErrorThrower(isolate).ThrowTypeError(
        "getPathForFile expected to receive a File object but one was not "
        "provided");
    return "";
  }
  return blob.Path();
}

void ImportSharedTextureForGpuDevice(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  auto* isolate = info.GetIsolate();
  auto v8_device = info.This();
  auto v8_tex_info = info[0].As<v8::Object>();

  blink::GPUDevice* device =
      blink::V8GPUDevice::ToWrappableUnsafe(isolate, v8_device);

  if (device == nullptr) {
    return;
  }

  gin::Dictionary dict(isolate, v8_tex_info);

  std::string sharedTextureHandle;
  dict.Get("sharedTextureHandle", &sharedTextureHandle);

  auto* handle = reinterpret_cast<void*>(std::stoull(sharedTextureHandle));

  blink::GPUTexture* sharedTexture = device->importSharedTexture(handle);

  info.GetReturnValue().SetNonEmpty(sharedTexture->Wrap(
      blink::ScriptState::From(v8_device->GetCreationContextChecked())));
}

}  // namespace electron::api::web_utils

namespace {

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();
  gin_helper::Dictionary dict(isolate, exports);
  dict.SetMethod("getPathForFile", &electron::api::web_utils::GetPathForFile);
  dict.Set(
      "importSharedTextureForGpuDeviceInternal",
      v8::FunctionTemplate::New(
          isolate, &electron::api::web_utils::ImportSharedTextureForGpuDevice)
          ->GetFunction(context)
          .ToLocalChecked());
}

}  // namespace

NODE_LINKED_BINDING_CONTEXT_AWARE(electron_renderer_web_utils, Initialize)
