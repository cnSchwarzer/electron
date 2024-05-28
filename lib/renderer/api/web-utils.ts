const binding = process._linkedBinding('electron_renderer_web_utils');

export const getPathForFile = binding.getPathForFile;
export const importSharedTextureForGpuDevice = (gpuDevice: any, textureInfo: any) => {
    const internal: (textureInfo: any) => any = binding.importSharedTextureForGpuDeviceInternal
    return internal.bind(gpuDevice)(textureInfo)
};
