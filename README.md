
# Libertas SDK

Libertas SDK is an open-source 3D SDK forked from Wicked Engine 0.71.633. It is just a small 3D SDK for fucking around in and it really isnt ready for a full feature game (and isn't planned to be). If you want to make a commercial game, please use <a href="https://github.com/turanszkij/WickedEngine">Wicked Engine</a> instead.


- [Features](Features.md)<br/>
- [C++ Documentation](Content/Documentation/WickedEngine-Documentation.md)<br/>
- [Lua Documentation](Content/Documentation/ScriptingAPI-Documentation.md)<br/>
- [Wicked Engine](https://github.com/turanszkij/WickedEngine)<br/>


### Platforms:
- Windows 10 or newer

### SDK contains:
- Samples to help you start making an application
- Editor
- Libertas Engine

### How to build: 

To build Libertas SDK for Windows (10 or newer), use the latest version of Visual Studio and the provided `LibertasSDK.sln` solution file.

If you want to develop a C++ application that uses Libertas Engine, you can build the Libertas Engine static library project for the appropriate platform, such as `LibertasEngine_Windows` and link against it. Including the `"Libertas Engine.h"` header will attempt to link the binaries for the appropriate platform, but search directories should be set up beforehand. For example, you can set additional library directories to `$(SolutionDir)BuildOutput\$(Configuration)` by default. For examples, see the `Template`, `Tests`, and `Editor` projects. 

### Lua:

You can use Lua to code whatever you want.
For further details, check the scripting API documentation: [Libertas SDK Scripting API](Content/Documentation/ScriptingAPI-Documentation.md)


### Model import/export:
The native model format is the <b>LILEVEL</b> format. Any application made with Libertas SDK can open this format efficiently.

In addition, the Editor supports importing some common model formats: 
- <b>OBJ</b>
- <b>GLTF 2.0</b>
- <b>VRM</b>
- <b>FBX</b>

The preferred workflow is to import models into the Editor, and save them as <b>LILEVEL</b>, then any application made with Libertas SDK can open them.<br/>

### Graphics APIs:
 The Libertas Engine supports both `DirectX 12` and `Vulkan`, `DirectX 12` being the default. You can specify command line arguments (without any prefix) to switch between render devices or other settings. <b>Current list of arguments: </b>
<table>
  <tr>
	<th>Argument</th>
	<th>Description</th>
  </tr>
  <tr>
	<td>vulkan</td>
	<td>Use the Vulkan rendering device</td>
  </tr>
  <tr>
	<td>debugdevice</td>
	<td>Use debug layer for graphics API validation. Performance will be degraded, but graphics warnings and errors will be written to the "Output" window</td>
  </tr>
  <tr>
	<td>gpuvalidation</td>
	<td>Use GPU Based Validation for graphics. This must be used together with the debugdevice argument. Currently DX12 only.</td>
  </tr>
  <tr>
	<td>gpu_verbose</td>
	<td>Enable verbose GPU validation mode.</td>
  </tr>
  <tr>
	<td>igpu</td>
	<td>Prefer integrated GPU selection for graphics. By default, dedicated GPU selection will be preferred.</td>
  </tr>
  <tr>
	<td>alwaysactive</td>
	<td>The application will not be paused when the window is in the background.</td>
  </tr>
</table>