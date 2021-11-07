#pragma once

#include <functional>

#include "RenderFunctions.h"
#include "Hooks.h"
#include "D3DCommon.h"

namespace D3D
{
	class RenderNametags;
	class RenderBones;

	class FbRenderer
	{
	public:
		void* RenderLighting(void* Arg0, struct LightingRenderViews* Arg1, bool Arg2, void* Arg3);
		void* RenderMerge(void* Arg0, void* Arg2, void* Arg3, Vector4f Arg4);
	};

	class Renderer
	{
		friend class FbRenderer;

		struct RenderPassList : public std::deque<std::shared_ptr<RenderPass>>
		{
			using std::deque<std::shared_ptr<RenderPass>>::deque;
			SpinLock Lock;
		};

	public:
		Renderer();

		template<class T, class... TArgs>
		std::weak_ptr<T> AddPass(TArgs&&... Args);

		bool RemovePass(const RenderPass* Pass);
		void RenderPostProcess(ID3D11RenderTargetView* Rtv, GameRenderer* Renderer);

		const GlobalShaderConstants& GShaderConstants() const;
		GlobalShaderConstants& GShaderConstants();

	protected:
		void RenderMerge(GameRenderer* Renderer);
		void RenderFinal(GameRenderer* Renderer, Dx11RenderTarget* Rt, Dx11Texture** Textures, Dx11Texture* Depth);

	private:
		void ResourceSync(ID3D11Device* Dev, IntPoint Vp);
		void UpdateConstants(GameRenderer* Renderer);

	private:
		RenderPassList Passes[RenderPass::NumTypes];
		ComPtr<ID3D11Buffer> ConstantBuffer;
		SceneDepth DepthCopy;
		IntPoint CurrentVp;
		GlobalShaderConstants ShaderConstants;
	};
}

#include "D3DRenderer.inl"