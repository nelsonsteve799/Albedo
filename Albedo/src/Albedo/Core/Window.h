#pragma once

#include "AlbedoPreCompiledHeader.h"
#include "Albedo/Events/Event.h"
#include "Core.h"

namespace Albedo {
	struct WindowProps 
	{
		std::string Title;
		uint32_t Width, Height;
		WindowProps(std::string title = "Albedo Window",
			uint32_t width = 1280, uint32_t height = 720)
			:Title(title), Height(height), Width(width) {}
	};

	class Albedo_API Window 
	{
	public:
		using EventCallBackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual void SetEventCallBack(const EventCallBackFn& callback) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps props = WindowProps());
	};
}