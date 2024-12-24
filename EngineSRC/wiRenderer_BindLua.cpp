#include "wiRenderer_BindLua.h"
#include "wiRenderer.h"
#include "wiHelper.h"
#include "wiScene.h"
#include "wiScene_BindLua.h"
#include "wiMath_BindLua.h"
#include "wiTexture_BindLua.h"
#include "wiEmittedParticle.h"
#include "wiHairParticle.h"
#include "wiPrimitive_BindLua.h"
#include "wiEventHandler.h"
#include "wiVoxelGrid_BindLua.h"
#include "wiPathQuery_BindLua.h"
#include "wiTrailRenderer_BindLua.h"

using namespace lb::ecs;
using namespace lb::graphics;
using namespace lb::scene;
using namespace lb::lua::scene;
using namespace lb::lua::primitive;

namespace lb::lua::renderer
{
	int SetGamma(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::lua::SSetString(L, "SetGamma() no longer supported!");
		}
		else
		{
			lb::lua::SError(L, "SetGamma(float) not enough arguments!");
		}
		return 0;
	}
	int SetGameSpeed(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetGameSpeed(lb::lua::SGetFloat(L, 1));
		}
		else
		{
			lb::lua::SError(L,"SetGameSpeed(float) not enough arguments!");
		}
		return 0;
	}
	int GetGameSpeed(lua_State* L)
	{
		lb::lua::SSetFloat(L, lb::renderer::GetGameSpeed());
		return 1;
	}
	int IsRaytracingSupported(lua_State* L)
	{
		lb::lua::SSetBool(L, lb::graphics::GetDevice()->CheckCapability(GraphicsDeviceCapability::RAYTRACING));
		return 1;
	}

	int SetShadowProps2D(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetShadowProps2D(lb::lua::SGetInt(L, 1));
		}
		else
			lb::lua::SError(L, "SetShadowProps2D(int max_resolution) not enough arguments!");
		return 0;
	}
	int SetShadowPropsCube(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetShadowPropsCube(lb::lua::SGetInt(L, 1));
		}
		else
			lb::lua::SError(L, "SetShadowPropsCube(int max_resolution) not enough arguments!");
		return 0;
	}
	int SetDebugPartitionTreeEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetToDrawDebugPartitionTree(lb::lua::SGetBool(L, 1));
		}
		return 0;
	}
	int SetDebugBoxesEnabled(lua_State* L)
	{
		lb::lua::SError(L, "SetDebugBoxesEnabled is obsolete! Use SetDebugPartitionTreeEnabled(bool value) instead to draw a partition tree!");
		return 0;
	}
	int SetDebugBonesEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetToDrawDebugBoneLines(lb::lua::SGetBool(L, 1));
		}
		return 0;
	}
	int SetDebugEmittersEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetToDrawDebugEmitters(lb::lua::SGetBool(L, 1));
		}
		return 0;
	}
	int SetDebugEnvProbesEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetToDrawDebugEnvProbes(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetDebugEnvProbesEnabled(bool enabled) not enough arguments!");
		}
		return 0;
	}
	int SetDebugForceFieldsEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetToDrawDebugForceFields(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetDebugForceFieldsEnabled(bool enabled) not enough arguments!");
		}
		return 0;
	}
	int SetDebugCamerasEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetToDrawDebugCameras(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetDebugCamerasEnabled(bool enabled) not enough arguments!");
		}
		return 0;
	}
	int SetDebugCollidersEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetToDrawDebugColliders(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetDebugCollidersEnabled(bool enabled) not enough arguments!");
		}
		return 0;
	}
	int SetGridHelperEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetToDrawGridHelper(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetGridHelperEnabled(bool enabled) not enough arguments!");
		}
		return 0;
	}
	int SetDDGIDebugEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetDDGIDebugEnabled(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetDDGIDebugEnabled(bool enabled) not enough arguments!");
		}
		return 0;
	}
	int SetVSyncEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::eventhandler::SetVSync(lb::lua::SGetBool(L, 1));
		}
		return 0;
	}
	int SetResolution(lua_State* L)
	{
		lb::lua::SError(L, "SetResolution() is deprecated, now it's handled by window events!");
		return 0;
	}
	int SetDebugLightCulling(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetDebugLightCulling(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetDebugLightCulling(bool enabled) not enough arguments!");
		}
		return 0;
	}
	int SetOcclusionCullingEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetOcclusionCullingEnabled(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetOcclusionCullingEnabled(bool enabled) not enough arguments!");
		}
		return 0;
	}
	int SetTemporalAAEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetTemporalAAEnabled(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetTemporalAAEnabled(bool enabled) not enough arguments!");
		}
		return 0;
	}
	int SetRaytracedShadowsEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetRaytracedShadowsEnabled(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetRaytracedShadowsEnabled(bool enabled) not enough arguments!");
		}
		return 0;
	}
	int SetMeshShaderAllowed(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetMeshShaderAllowed(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetMeshShaderAllowed(bool enabled) not enough arguments!");
		}
		return 0;
	}
	int SetMeshletOcclusionCullingEnabled(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			lb::renderer::SetMeshletOcclusionCullingEnabled(lb::lua::SGetBool(L, 1));
		}
		else
		{
			lb::lua::SError(L, "SetMeshletOcclusionCullingEnabled(bool enabled) not enough arguments!");
		}
		return 0;
	}

	int DrawLine(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 1)
		{
			Vector_BindLua* a = Luna<Vector_BindLua>::lightcheck(L, 1);
			Vector_BindLua* b = Luna<Vector_BindLua>::lightcheck(L, 2);
			if (a && b)
			{
				lb::renderer::RenderableLine line;
				XMStoreFloat3(&line.start, XMLoadFloat4(&a->data));
				XMStoreFloat3(&line.end, XMLoadFloat4(&b->data));
				bool depth = false;
				if (argc > 2)
				{
					Vector_BindLua* c = Luna<Vector_BindLua>::lightcheck(L, 3);
					if (c)
					{
						XMStoreFloat4(&line.color_start, XMLoadFloat4(&c->data));
						XMStoreFloat4(&line.color_end, XMLoadFloat4(&c->data));
					}
					else
						lb::lua::SError(L, "DrawLine(Vector origin,end, opt Vector color, opt bool depth = false) one or more arguments are not vectors!");

					if (argc > 3)
					{
						depth = lb::lua::SGetBool(L, 4);
					}
				}
				lb::renderer::DrawLine(line, depth);
			}
			else
				lb::lua::SError(L, "DrawLine(Vector origin,end, opt Vector color, opt bool depth = false) one or more arguments are not vectors!");
		}
		else
			lb::lua::SError(L, "DrawLine(Vector origin,end, opt Vector color, opt bool depth = false) not enough arguments!");

		return 0;
	}
	int DrawPoint(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Vector_BindLua* a = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (a)
			{
				lb::renderer::RenderablePoint point;
				XMStoreFloat3(&point.position, XMLoadFloat4(&a->data));
				bool depth = false;
				if (argc > 1)
				{
					point.size = lb::lua::SGetFloat(L, 2);

					if (argc > 2)
					{
						Vector_BindLua* color = Luna<Vector_BindLua>::lightcheck(L, 3);
						if (color)
						{
							point.color = color->data;
						}

						if (argc > 3)
						{
							depth = lb::lua::SGetBool(L, 4);
						}
					}
				}
				lb::renderer::DrawPoint(point, depth);
			}
			else
				lb::lua::SError(L, "DrawPoint(Vector origin, opt float size, opt Vector color, opt bool depth = false) first argument must be a Vector type!");
		}
		else
			lb::lua::SError(L, "DrawPoint(Vector origin, opt float size, opt Vector color, opt bool depth = false) not enough arguments!");

		return 0;
	}
	int DrawBox(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Matrix_BindLua* m = Luna<Matrix_BindLua>::lightcheck(L, 1);
			if (m)
			{
				if (argc > 1)
				{
					Vector_BindLua* color = Luna<Vector_BindLua>::lightcheck(L, 2);
					if (color)
					{
						bool depth = true;
						if (argc > 2)
						{
							depth = lb::lua::SGetBool(L, 3);
						}
						lb::renderer::DrawBox(m->data, color->data, depth);
						return 0;
					}
				}

				lb::renderer::DrawBox(m->data);
			}
			else
				lb::lua::SError(L, "DrawBox(Matrix boxMatrix, opt Vector color, opt bool depth = true) first argument must be a Matrix type!");
		}
		else
			lb::lua::SError(L, "DrawBox(Matrix boxMatrix, opt Vector color, opt bool depth = true) not enough arguments!");

		return 0;
	}
	int DrawSphere(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Sphere_BindLua* sphere = Luna<Sphere_BindLua>::lightcheck(L, 1);
			if (sphere)
			{
				if (argc > 1)
				{
					Vector_BindLua* color = Luna<Vector_BindLua>::lightcheck(L, 2);
					if (color)
					{
						bool depth = true;
						if (argc > 2)
						{
							depth = lb::lua::SGetBool(L, 3);
						}
						lb::renderer::DrawSphere(sphere->sphere, color->data, depth);
						return 0;
					}
				}

				lb::renderer::DrawSphere(sphere->sphere);
			}
			else
				lb::lua::SError(L, "DrawSphere(Sphere sphere, opt Vector color, opt bool depth = true) first argument must be a Matrix type!");
		}
		else
			lb::lua::SError(L, "DrawSphere(Sphere sphere, opt Vector color, opt bool depth = true) not enough arguments!");

		return 0;
	}
	int DrawCapsule(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			Capsule_BindLua* capsule = Luna<Capsule_BindLua>::lightcheck(L, 1);
			if (capsule)
			{
				if (argc > 1)
				{
					Vector_BindLua* color = Luna<Vector_BindLua>::lightcheck(L, 2);
					if (color)
					{
						bool depth = true;
						if (argc > 2)
						{
							depth = lb::lua::SGetBool(L, 3);
						}
						lb::renderer::DrawCapsule(capsule->capsule, color->data, depth);
						return 0;
					}
				}

				lb::renderer::DrawCapsule(capsule->capsule);
			}
			else
				lb::lua::SError(L, "DrawCapsule(Capsule capsule, opt Vector color, opt bool depth = true) first argument must be a Matrix type!");
		}
		else
			lb::lua::SError(L, "DrawCapsule(Capsule capsule, opt Vector color, opt bool depth = true) not enough arguments!");

		return 0;
	}
	int DrawDebugText(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			std::string text = lb::lua::SGetString(L, 1);
			lb::renderer::DebugTextParams params;
			if (argc > 1)
			{
				Vector_BindLua* position = Luna<Vector_BindLua>::lightcheck(L, 2);
				if (position != nullptr)
				{
					params.position.x = position->data.x;
					params.position.y = position->data.y;
					params.position.z = position->data.z;

					if (argc > 2)
					{
						Vector_BindLua* color = Luna<Vector_BindLua>::lightcheck(L, 3);
						if (color != nullptr)
						{
							params.color = color->data;

							if (argc > 3)
							{
								params.scaling = lb::lua::SGetFloat(L, 4);

								if (argc > 4)
								{
									params.flags = lb::lua::SGetInt(L, 5);
								}
							}
						}
						else
							lb::lua::SError(L, "DrawDebugText(string text, opt Vector position, opt Vector color, opt float scaling, opt int flags) third argument was not a Vector!");
					}
				}
				else
					lb::lua::SError(L, "DrawDebugText(string text, opt Vector position, opt Vector color, opt float scaling, opt int flags) second argument was not a Vector!");

			}
			lb::renderer::DrawDebugText(text.c_str(), params);
		}
		else
			lb::lua::SError(L, "DrawDebugText(string text, opt Vector position, opt Vector color, opt float scaling, opt int flags) not enough arguments!");

		return 0;
	}
	int DrawVoxelGrid(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			VoxelGrid_BindLua* a = Luna<VoxelGrid_BindLua>::lightcheck(L, 1);
			if (a)
			{
				lb::renderer::DrawVoxelGrid(a->voxelgrid);
			}
			else
				lb::lua::SError(L, "DrawVoxelGrid(VoxelGrid voxelgrid) first argument must be a VoxelGrid type!");
		}
		else
			lb::lua::SError(L, "DrawVoxelGrid(VoxelGrid voxelgrid) not enough arguments!");

		return 0;
	}
	int DrawPathQuery(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			PathQuery_BindLua* a = Luna<PathQuery_BindLua>::lightcheck(L, 1);
			if (a)
			{
				lb::renderer::DrawPathQuery(a->pathquery);
			}
			else
				lb::lua::SError(L, "DrawPathQuery(PathQuery pathquery) first argument must be a PathQuery type!");
		}
		else
			lb::lua::SError(L, "DrawPathQuery(PathQuery pathquery) not enough arguments!");

		return 0;
	}
	int DrawTrail(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 0)
		{
			TrailRenderer_BindLua* a = Luna<TrailRenderer_BindLua>::lightcheck(L, 1);
			if (a)
			{
				lb::renderer::DrawTrail(&a->trail);
			}
			else
				lb::lua::SError(L, "DrawTrail(TrailRenderer trail) first argument must be a TrailRenderer type!");
		}
		else
			lb::lua::SError(L, "DrawTrail(TrailRenderer trail) not enough arguments!");

		return 0;
	}

	class PaintTextureParams_BindLua
	{
	public:
		lb::renderer::PaintTextureParams params;

		PaintTextureParams_BindLua(const lb::renderer::PaintTextureParams& params) : params(params) {}
		PaintTextureParams_BindLua(lua_State* L) {}

		int SetEditTexture(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetEditTexture(Texture tex): not enough arguments!");
				return 0;
			}
			Texture_BindLua* tex = Luna<Texture_BindLua>::lightcheck(L, 1);
			if (tex == nullptr)
			{
				lb::lua::SError(L, "SetEditTexture(Texture tex): argument is not a Texture!");
				return 0;
			}
			if (tex->resource.IsValid())
			{
				params.editTex = tex->resource.GetTexture();
			}
			return 0;
		}
		int SetBrushTexture(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetBrushTexture(Texture tex): not enough arguments!");
				return 0;
			}
			Texture_BindLua* tex = Luna<Texture_BindLua>::lightcheck(L, 1);
			if (tex == nullptr)
			{
				lb::lua::SError(L, "SetBrushTexture(Texture tex): argument is not a Texture!");
				return 0;
			}
			if (tex->resource.IsValid())
			{
				params.brushTex = tex->resource.GetTexture();
			}
			return 0;
		}
		int SetRevealTexture(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetRevealTexture(Texture tex): not enough arguments!");
				return 0;
			}
			Texture_BindLua* tex = Luna<Texture_BindLua>::lightcheck(L, 1);
			if (tex == nullptr)
			{
				lb::lua::SError(L, "SetRevealTexture(Texture tex): argument is not a Texture!");
				return 0;
			}
			if (tex->resource.IsValid())
			{
				params.revealTex = tex->resource.GetTexture();
			}
			return 0;
		}
		int SetCenterPixel(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetCenterPixel(Vector value): not enough arguments!");
				return 0;
			}
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec == nullptr)
			{
				lb::lua::SError(L, "SetCenterPixel(Vector value): argument is not a Vector!");
				return 0;
			}
			params.push.xPaintBrushCenter.x = uint32_t(vec->data.x);
			params.push.xPaintBrushCenter.y = uint32_t(vec->data.y);
			return 0;
		}
		int SetBrushColor(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetBrushColor(Vector value): not enough arguments!");
				return 0;
			}
			Vector_BindLua* vec = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (vec == nullptr)
			{
				lb::lua::SError(L, "SetBrushColor(Vector value): argument is not a Vector!");
				return 0;
			}
			params.push.xPaintBrushColor = lb::Color::fromFloat4(vec->data);
			return 0;
		}
		int SetBrushRadius(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetBrushRadius(int value): not enough arguments!");
				return 0;
			}
			params.push.xPaintBrushRadius = lb::lua::SGetInt(L, 1);
			return 0;
		}
		int SetBrushAmount(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetBrushAmount(float value): not enough arguments!");
				return 0;
			}
			params.push.xPaintBrushAmount = lb::lua::SGetFloat(L, 1);
			return 0;
		}
		int SetBrushSmoothness(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetBrushSmoothness(float value): not enough arguments!");
				return 0;
			}
			params.push.xPaintBrushSmoothness = lb::lua::SGetFloat(L, 1);
			return 0;
		}
		int SetBrushRotation(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetBrushRotation(float value): not enough arguments!");
				return 0;
			}
			params.push.xPaintBrushRotation = lb::lua::SGetFloat(L, 1);
			return 0;
		}
		int SetBrushShape(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetBrushShape(float value): not enough arguments!");
				return 0;
			}
			params.push.xPaintBrushShape = lb::lua::SGetInt(L, 1);
			return 0;
		}

		inline static constexpr char className[] = "PaintTextureParams";
		inline static constexpr Luna<PaintTextureParams_BindLua>::FunctionType methods[] = {
			lunamethod(PaintTextureParams_BindLua, SetEditTexture),
			lunamethod(PaintTextureParams_BindLua, SetBrushTexture),
			lunamethod(PaintTextureParams_BindLua, SetRevealTexture),
			lunamethod(PaintTextureParams_BindLua, SetBrushColor),
			lunamethod(PaintTextureParams_BindLua, SetCenterPixel),
			lunamethod(PaintTextureParams_BindLua, SetBrushRadius),
			lunamethod(PaintTextureParams_BindLua, SetBrushAmount),
			lunamethod(PaintTextureParams_BindLua, SetBrushSmoothness),
			lunamethod(PaintTextureParams_BindLua, SetBrushRotation),
			lunamethod(PaintTextureParams_BindLua, SetBrushShape),
			{ nullptr, nullptr }
		};
		inline static constexpr Luna<PaintTextureParams_BindLua>::PropertyType properties[] = {
			{ nullptr, nullptr }
		};
	};

	int PaintIntoTexture(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "PaintIntoTexture(PaintTextureParams params): not enough arguments!");
			return 0;
		}
		PaintTextureParams_BindLua* params = Luna<PaintTextureParams_BindLua>::lightcheck(L, 1);
		if (params == nullptr)
		{
			lb::lua::SError(L, "PaintIntoTexture(PaintTextureParams params): argument is not a PaintTextureParams!");
			return 0;
		}
		lb::renderer::PaintIntoTexture(params->params);
		return 0;
	}
	int CreatePaintableTexture(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 2)
		{
			lb::lua::SError(L, "CreatePaintableTexture(int width,height, opt int mips = 0, opt Vector initialColor = Vector()): not enough arguments!");
			return 0;
		}
		uint32_t width = (uint32_t)lb::lua::SGetInt(L, 1);
		uint32_t height = (uint32_t)lb::lua::SGetInt(L, 2);
		uint32_t mips = 0;
		lb::Color color = lb::Color::Transparent();
		if (argc > 2)
		{
			mips = (uint32_t)lb::lua::SGetInt(L, 3);
			if (argc > 3)
			{
				Vector_BindLua* v = Luna<Vector_BindLua>::lightcheck(L, 4);
				if (v != nullptr)
				{
					color = lb::Color::fromFloat4(v->data);
				}
			}
		}
		Luna<Texture_BindLua>::push(L, lb::renderer::CreatePaintableTexture(width, height, mips, color));
		return 1;
	}


	class PaintDecalParams_BindLua
	{
	public:
		lb::renderer::PaintDecalParams params;

		PaintDecalParams_BindLua(const lb::renderer::PaintDecalParams& params) : params(params) {}
		PaintDecalParams_BindLua(lua_State* L) {}

		int SetInTexture(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetInTexture(Texture tex): not enough arguments!");
				return 0;
			}
			Texture_BindLua* tex = Luna<Texture_BindLua>::lightcheck(L, 1);
			if (tex == nullptr)
			{
				lb::lua::SError(L, "SetInTexture(Texture tex): argument is not a Texture!");
				return 0;
			}
			if (tex->resource.IsValid())
			{
				params.in_texture = tex->resource.GetTexture();
			}
			return 0;
		}
		int SetOutTexture(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetOutTexture(Texture tex): not enough arguments!");
				return 0;
			}
			Texture_BindLua* tex = Luna<Texture_BindLua>::lightcheck(L, 1);
			if (tex == nullptr)
			{
				lb::lua::SError(L, "SetOutTexture(Texture tex): argument is not a Texture!");
				return 0;
			}
			if (tex->resource.IsValid())
			{
				params.out_texture = tex->resource.GetTexture();
			}
			return 0;
		}
		int SetMatrix(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetMatrix(Matrix mat): not enough arguments!");
				return 0;
			}
			Matrix_BindLua* mat = Luna<Matrix_BindLua>::lightcheck(L, 1);
			if (mat == nullptr)
			{
				lb::lua::SError(L, "SetMatrix(Texture mat): argument is not a Texture!");
				return 0;
			}
			params.decalMatrix = mat->data;
			return 0;
		}
		int SetObject(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetObject(Entity entity): not enough arguments!");
				return 0;
			}
			params.objectEntity = (lb::ecs::Entity)lb::lua::SGetLongLong(L, 1);
			return 0;
		}
		int SetSlopeBlendPower(lua_State* L)
		{
			int argc = lb::lua::SGetArgCount(L);
			if (argc < 1)
			{
				lb::lua::SError(L, "SetSlopeBlendPower(float power): not enough arguments!");
				return 0;
			}
			params.slopeBlendPower = lb::lua::SGetFloat(L, 1);
			return 0;
		}

		inline static constexpr char className[] = "PaintDecalParams";
		inline static constexpr Luna<PaintDecalParams_BindLua>::FunctionType methods[] = {
			lunamethod(PaintDecalParams_BindLua, SetInTexture),
			lunamethod(PaintDecalParams_BindLua, SetOutTexture),
			lunamethod(PaintDecalParams_BindLua, SetMatrix),
			lunamethod(PaintDecalParams_BindLua, SetObject),
			lunamethod(PaintDecalParams_BindLua, SetSlopeBlendPower),
			{ nullptr, nullptr }
		};
		inline static constexpr Luna<PaintDecalParams_BindLua>::PropertyType properties[] = {
			{ nullptr, nullptr }
		};
	};
	int PaintDecalIntoObjectSpaceTexture(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc < 1)
		{
			lb::lua::SError(L, "PaintDecalIntoObjectSpaceTexture(PaintDecalParams params): not enough arguments!");
			return 0;
		}
		PaintDecalParams_BindLua* params = Luna<PaintDecalParams_BindLua>::lightcheck(L, 1);
		if (params == nullptr)
		{
			lb::lua::SError(L, "PaintDecalIntoObjectSpaceTexture(PaintDecalParams params): argument is not a PaintDecalParams!");
			return 0;
		}
		lb::renderer::PaintDecalIntoObjectSpaceTexture(params->params);
		return 0;
	}

	int PutWaterRipple(lua_State* L)
	{
		int argc = lb::lua::SGetArgCount(L);
		if (argc > 1)
		{
			std::string name = lb::lua::SGetString(L, 1);
			Vector_BindLua* v = Luna<Vector_BindLua>::lightcheck(L, 2);
			if (v)
			{
				GetGlobalScene()->PutWaterRipple(name, v->GetFloat3());
			}
			else
				lb::lua::SError(L, "PutWaterRipple(string imagename, Vector position) argument is not a Vector!");
		}
		else if (argc > 0)
		{
			Vector_BindLua* v = Luna<Vector_BindLua>::lightcheck(L, 1);
			if (v)
			{
				GetGlobalScene()->PutWaterRipple(v->GetFloat3());
			}
			else
				lb::lua::SError(L, "PutWaterRipple(Vector position) argument is not a Vector!");
		}
		else
		{
			lb::lua::SError(L, "PutWaterRipple(Vector position) not enough arguments!");
			lb::lua::SError(L, "PutWaterRipple(string imagename, Vector position) not enough arguments!");
		}
		return 0;
	}

	int ClearWorld(lua_State* L)
	{
		Scene_BindLua* scene = Luna<Scene_BindLua>::lightcheck(L, 1);
		if (scene == nullptr)
		{
			lb::renderer::ClearWorld(*GetGlobalScene());
		}
		else
		{
			lb::renderer::ClearWorld(*scene->scene);
		}
		return 0;
	}
	int ReloadShaders(lua_State* L)
	{
		lb::renderer::ReloadShaders();
		return 0;
	}

	void Bind()
	{
		static bool initialized = false;
		if (!initialized)
		{
			initialized = true;

			Luna<PaintTextureParams_BindLua>::Register(lb::lua::GetLuaState());
			Luna<PaintDecalParams_BindLua>::Register(lb::lua::GetLuaState());

			lb::lua::RegisterFunc("SetGamma", SetGamma);
			lb::lua::RegisterFunc("SetGameSpeed", SetGameSpeed);
			lb::lua::RegisterFunc("GetGameSpeed", GetGameSpeed);
			lb::lua::RegisterFunc("IsRaytracingSupported", IsRaytracingSupported);

			lb::lua::RegisterFunc("SetShadowProps2D", SetShadowProps2D);
			lb::lua::RegisterFunc("SetShadowPropsCube", SetShadowPropsCube);
			lb::lua::RegisterFunc("SetDebugBoxesEnabled", SetDebugBoxesEnabled);
			lb::lua::RegisterFunc("SetDebugPartitionTreeEnabled", SetDebugPartitionTreeEnabled);
			lb::lua::RegisterFunc("SetDebugBonesEnabled", SetDebugBonesEnabled);
			lb::lua::RegisterFunc("SetDebugEmittersEnabled", SetDebugEmittersEnabled);
			lb::lua::RegisterFunc("SetDebugEnvProbesEnabled", SetDebugEnvProbesEnabled);
			lb::lua::RegisterFunc("SetDebugForceFieldsEnabled", SetDebugForceFieldsEnabled);
			lb::lua::RegisterFunc("SetDebugCamerasEnabled", SetDebugCamerasEnabled);
			lb::lua::RegisterFunc("SetDebugCollidersEnabled", SetDebugCollidersEnabled);
			lb::lua::RegisterFunc("SetGridHelperEnabled", SetGridHelperEnabled);
			lb::lua::RegisterFunc("SetDDGIDebugEnabled", SetDDGIDebugEnabled);
			lb::lua::RegisterFunc("SetVSyncEnabled", SetVSyncEnabled);
			lb::lua::RegisterFunc("SetResolution", SetResolution);
			lb::lua::RegisterFunc("SetDebugLightCulling", SetDebugLightCulling);
			lb::lua::RegisterFunc("SetOcclusionCullingEnabled", SetOcclusionCullingEnabled);
			lb::lua::RegisterFunc("SetTemporalAAEnabled", SetTemporalAAEnabled);
			lb::lua::RegisterFunc("SetRaytracedShadowsEnabled", SetRaytracedShadowsEnabled);
			lb::lua::RegisterFunc("SetMeshShaderAllowed", SetMeshShaderAllowed);
			lb::lua::RegisterFunc("SetMeshletOcclusionCullingEnabled", SetMeshletOcclusionCullingEnabled);

			lb::lua::RegisterFunc("DrawLine", DrawLine);
			lb::lua::RegisterFunc("DrawPoint", DrawPoint);
			lb::lua::RegisterFunc("DrawBox", DrawBox);
			lb::lua::RegisterFunc("DrawSphere", DrawSphere);
			lb::lua::RegisterFunc("DrawCapsule", DrawCapsule);
			lb::lua::RegisterFunc("DrawDebugText", DrawDebugText);
			lb::lua::RegisterFunc("DrawVoxelGrid", DrawVoxelGrid);
			lb::lua::RegisterFunc("DrawPathQuery", DrawPathQuery);
			lb::lua::RegisterFunc("DrawTrail", DrawTrail);

			lb::lua::RegisterFunc("PaintIntoTexture", PaintIntoTexture);
			lb::lua::RegisterFunc("CreatePaintableTexture", CreatePaintableTexture);
			lb::lua::RegisterFunc("PaintDecalIntoObjectSpaceTexture", PaintDecalIntoObjectSpaceTexture);

			lb::lua::RegisterFunc("PutWaterRipple", PutWaterRipple);

			lb::lua::RegisterFunc("ClearWorld", ClearWorld);
			lb::lua::RegisterFunc("ReloadShaders", ReloadShaders);

			lb::lua::RunText(R"(
GetScreenWidth = function() return main.GetCanvas().GetLogicalWidth() end
GetScreenHeight = function() return main.GetCanvas().GetLogicalHeight() end

DEBUG_TEXT_DEPTH_TEST = 1
DEBUG_TEXT_CAMERA_FACING = 2
DEBUG_TEXT_CAMERA_SCALING = 4
)");

		}
	}
};
