#pragma once

#include "DME/Renderer/Buffer.h"

namespace DME
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		void Bind() const override;
		void UnBind() const override;

		void SetData(const void* data, uint32_t size) override;
		
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }


	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;


	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void UnBind() const;

		virtual uint32_t GetCount() const { return m_Count; }

	private:

		uint32_t m_RendererID;
		uint32_t m_Count;


	};
}