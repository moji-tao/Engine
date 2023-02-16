#pragma once
#include <memory>
#include "EngineRuntime/include/renderdoc_app.h"
#include "EngineRuntime/include/Framework/Interface/IRuntimeModule.h"
#include "EngineRuntime/include/Framework/Interface/ISingleton.h"
#include "EngineRuntime/include/Function/Window/WindowUI.h"
#include "EngineRuntime/include/Function/Render/RHI.h"
#include "EngineRuntime/include/Function/Render/ImGuiDevice.h"
#include "EngineRuntime/include/Function/Render/RenderPipeline.h"
#include "EngineRuntime/include/Function/Render/RenderResource.h"

namespace Engine
{
	class RenderSystem : public ISingleton<RenderSystem>
	{
	public:
		RenderSystem();

		virtual ~RenderSystem();

	public:
		virtual bool Initialize(InitConfig* info);

		virtual bool Tick(float deltaTile, bool isEditorMode);

		virtual void Finalize();

		ImGuiDevice* InitializeUIRenderBackend(WindowUI* windowUI);

		RenderResource* GetRenderResource();

		void InitializeUIRenderBackendEnd();

		void FinalizeUIRenderBackend();

		void RenderViewResize(int width, int height);

		void SetEditorRenderCamera(const RenderCamera& camera);

		Vector2 GetRenderViewSize();

		RenderCamera* GetRenderCamera();

	public:
		size_t mFrameCount = 0;

		RENDERDOC_API_1_1_2* rdoc_api = nullptr;//API接口

	private:
		std::unique_ptr<RHI> mRHI = nullptr;

		std::unique_ptr<RenderCamera> mRenderCamera = nullptr;

		const RenderCamera* mEditorRenderCamera = nullptr;

		std::unique_ptr<RenderPipeline> mRenderPipeline = nullptr;

		std::unique_ptr<RenderResource> mRenderResource = nullptr;

		std::unique_ptr<ImGuiDevice> mImGuiDevice = nullptr;

	private:
		Vector2 mRenderView;

	public:
		bool mEnableTAA = false;
		bool mEnableSSAO = true;
		bool mEnableAmbientLighting = false;
	};
}
