#pragma once

#include "Albedo.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Albedo/Cameras/EditorCamera.h"
#include "Albedo/Utils/AssetSystem.h"
// Panels
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneConfigurationPanel.h"

namespace Albedo {
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void OpenDefaultScene(const std::string& path);
		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();
		void OnScenePause();

		void OnDuplicateEntity();

		// UI Panels
		void UI_Toolbar();
	private:
		OrthographicCameraController m_CameraController;
		Ref<Scene>					 m_ActiveScene;
		Ref<Scene>					 m_EditorScene;
		Ref<Scene>					 m_TempScene;
		Ref<Framebuffer>			 m_Framebuffer;

		Ref<Texture2D> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop;

		Ref<EditorCamera>			 m_EditorCamera;
		Entity						 m_HoveredEntity;
		glm::vec2					 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2					 m_ViewportBounds[2];
		bool						 m_PrimaryCamera = true;
		bool						 m_ViewportFocused = false;
		bool						 m_ViewportHovered = false;
		int							 m_GizmoType = -1;
		SceneHierarchyPanel			 m_SceneHierarchyPanel;
		ContentBrowserPanel			 m_ContentBrowserPanel;
		SceneConfigurationPanel			 m_SceneConfigurationPanel;
		std::filesystem::path m_EditorScenePath;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};


		std::string savedScenePath = "TempScene.albedo";

		SceneState m_SceneState = SceneState::Edit;

		friend class SceneHierarchyPanel;

	};
}