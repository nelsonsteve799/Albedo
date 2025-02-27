#pragma once

#include "Albedo/Events/Event.h"
#include "Albedo/Core/Core.h"
#include "Albedo/Core/Timestep.h"

namespace Albedo {

	class Albedo_API Layer
	{
	public:
		Layer(const std::string& debugName = "Layer");
		virtual ~Layer();

		virtual void OnAttach(){}
		virtual void OnDetach(){}
		virtual void OnUpdate(Timestep ts){}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e){}

		inline const std::string GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}