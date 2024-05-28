# webUtils

> A utility layer to interact with Web API objects (Files, Blobs, etc.)

Process: [Renderer](../glossary.md#renderer-process)

## Methods

The `webUtils` module has the following methods:

### `webUtils.getPathForFile(file)`

* `file` File - A web [File](https://developer.mozilla.org/en-US/docs/Web/API/File) object.

Returns `string` - The file system path that this `File` object points to. In the case where the object passed in is not a `File` object an exception is thrown. In the case where the File object passed in was constructed in JS and is not backed by a file on disk an empty string is returned.

This method superceded the previous augmentation to the `File` object with the `path` property.  An example is included below.

```js
// Before
const oldPath = document.querySelector('input').files[0].path

// After
const { webUtils } = require('electron')
const newPath = webUtils.getPathForFile(document.querySelector('input').files[0])
```

### `webUtils.importSharedTextureForGpuDevice(device, textureInfo)`

* `device` any - A [GPUDevice](https://developer.mozilla.org/zh-CN/docs/Web/API/GPUDevice) object.
* `textureInfo` Object - Contains the info about the shared texture to be imported. It is designed to use with textures captured by the [offscreen rendering](../tutorial/offscreen-rendering.md), but any valid external texture from whatever source should also work.
  * `sharedTextureHandle` string - _Windows_ _macOS_ The handle to the shared texture, in integer string format. On Windows is the `HANDLE` return from `[GetSharedHandle](https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiresource-getsharedhandle)`. On macOS is a `IOSurface*`.
  * `planes` Object[] - _Linux_ Each plane's info of the shared texture.
    * `stride` number - The strides and offsets in bytes to be used when accessing the buffers via a memory mapping. One per plane per entry.
    * `offset` number - The strides and offsets in bytes to be used when accessing the buffers via a memory mapping. One per plane per entry.
    * `size` number - Size in bytes of the plane. This is necessary to map the buffers.
    * `fd` number - File descriptor for the underlying memory object (usually dmabuf).
  * `modifier` string - _Linux_ The modifier is retrieved from GBM library and passed to EGL driver. 

Returns `any` - A [GPUTexture](https://developer.mozilla.org/en-US/docs/Web/API/GPUTexture) that represents the imported texture.

This method can import shared external texture handles into WebGPU context.

```js
// Main
const win = new BrowserWindow({
  width: 800,
  height: 600,
  webPreferences: {
    offscreen: true,
    offscreenUseSharedTexture: true
  }
})
const otherWin = new BrowserWindow({ width: 800, height: 800 })

var importingTexture;
win.webContents.on('paint', (event, dirty, image) => {
  // Record the texture at main process for release later.
  importingTexture = event.texture
  // Send only `textureInfo` to renderer processes. `release` function is not clonable.
  otherWin.webContents.send('render', importingTexture.textureInfo)
})

ipcMain.on('import-finished', () => {
  // Release at main process once finished
  importingTexture.release()
})

// Renderer
const adapter = await navigator.gpu.requestAdapter();
const device = await adapter.requestDevice();
ipcRenderer.on('import-texture', (_event, info) => {
  const importedTexture = webUtils.importSharedTextureForGpuDevice(device, info)
  ipcRenderer.send('import-finished')
})
```
