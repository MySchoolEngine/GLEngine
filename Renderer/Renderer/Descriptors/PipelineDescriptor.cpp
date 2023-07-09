#include <RendererStdafx.h>

#include <Renderer/Descriptors/PipelineDescriptor.h>

RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;

	rttr::registration::class_<AttributeDescriptor>("AttributeDescriptor")
		.property("binding", &AttributeDescriptor::binding)
		.property("type", &AttributeDescriptor::type);

	rttr::registration::class_<BlendingDescriptor>("BlendingDescriptor")
		.property("enable", &BlendingDescriptor::enable)
		.property("blendColorFunction", &BlendingDescriptor::blendColorFunction)
		.property("srcColorFactor", &BlendingDescriptor::srcColorFactor)
		.property("dstColorFactor", &BlendingDescriptor::dstColorFactor)
		.property("blendAlphaFunction", &BlendingDescriptor::blendAlphaFunction)
		.property("srcAlphaFactor", &BlendingDescriptor::srcAlphaFactor)
		.property("dstAlphaFactor", &BlendingDescriptor::dstAlphaFactor)
		.property("writemask", &BlendingDescriptor::writemask);

	rttr::registration::class_<PipelineDescriptor>("PipelineDescriptor")
		.property("primitiveType", &PipelineDescriptor::primitiveType)
		.property("wireframe", &PipelineDescriptor::wireframe)
		.property("blending", &PipelineDescriptor::blending)
		.property("blendConstant", &PipelineDescriptor::blendConstant)
		.property("bindingCount", &PipelineDescriptor::bindingCount)
		.property("vertexInput", &PipelineDescriptor::vertexInput)
		.property("instanceInput", &PipelineDescriptor::instanceInput)
		.property("shader", &PipelineDescriptor::shader)
		.property("colorAttachementFormat", &PipelineDescriptor::colorAttachementFormat);
}

namespace GLEngine::Renderer {

}
