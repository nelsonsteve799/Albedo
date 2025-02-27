#include "AlbedoPreCompiledHeader.h"

#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Albedo {
	Ref<VertexArray> VertexArray::Create() {
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
		}
		return nullptr;
	}
}