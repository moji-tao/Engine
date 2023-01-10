#pragma once
#include "EngineRuntime/include/Function/Render/RenderPipeline.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/RenderTarget.h"

namespace Engine
{
	class D3D12RenderPipeline final : public RenderPipeline
	{
	public:
		D3D12RenderPipeline(D3D12RHI* rhi);

		virtual ~D3D12RenderPipeline();

	public:
		virtual void ForwardRender() override;

		virtual void DeferredRender() override;

		virtual void RenderTargetResize(int width, int height) override;

	private:
		void TestImGuiSceneWindow();

	private:
		D3D12RHI* mRHI = nullptr;

		std::unique_ptr<RenderTarget2D> mSceneRenderTarget = nullptr;

	};	
}
