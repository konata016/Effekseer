
#include <stdint.h>
#include <array>

namespace Effekseer
{
namespace Backend
{

class VertexBuffer
{
};

class IndexBuffer
{
};

class VertexLayout
{
};

class UniformBuffer
{
};

class PipelineState
{
	// VertexLayout
	// ...
};

class Texture
{
	// type
	// format
	// size
};

class Shader
{
	// How to...
	// Unity, UE4, from key

	// Require shader register
};

class ComputeBuffer
{
	// TODO...
};

enum class TextureWrapType
{
	Clamp,
	Repeat,
};

enum class TextureSamplingType
{
	Linear,
	Nearest,
};

struct DrawParameter
{
public:
	VertexBuffer* VertexBufferPtr = nullptr;
	IndexBuffer* IndexBufferPtr = nullptr;
	PipelineState* PipelineStatePtr = nullptr;
	
	std::array<Texture*, 8> TexturePtrs;
	std::array<TextureWrapType, 8> TextureWrapTypes;
	std::array<TextureSamplingType, 8> TextureSamplingTypes;

	int32_t PrimitiveCount = 0;
	int32_t InstanceCount = 0;
};

class GraphicsDevice
{
	/**
		@brief	Create VertexBuffer
		@param	size	the size of buffer
		@param	initialData	the initial data of buffer. If it is null, not initialized.
		@param	isDynamic	whether is the buffer dynamic? (for DirectX9, 11 or OpenGL)
	*/
	virtual VertexBuffer* CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic)
	{
		return nullptr;
	}

	virtual IndexBuffer* CreateIndexBuffer(int32_t size, const void* initialData, int32_t stride)
	{
		return nullptr;
	}

	virtual bool UpdateVertexBuffer(VertexBuffer* buffer, int32_t size, int32_t offset, const void* data)
	{
		return false;
	}

	virtual bool UpdateIndexBuffer(IndexBuffer* buffer, int32_t size, int32_t offset, const void* data)
	{
		return false;
	}

	virtual VertexLayout* CreateVertexLayout()
	{
		return nullptr;
	}

	virtual UniformBuffer* CreateUniformBuffer()
	{
		return nullptr;
	}

	virtual UniformBuffer* CreatePipelineState()
	{
		return nullptr;
	}

	virtual Texture* CreateTexture()
	{
		return nullptr;
	}

	virtual Texture* CreateShader()
	{
		return nullptr;
	}

	virtual Texture* CreateComputeBuffer()
	{
		return nullptr;
	}

	// RenderPass?

	virtual void Draw(DrawParameter& drawParam)
	{
		return nullptr;
	}
};

} // namespace Backend
} // namespace Effekseer