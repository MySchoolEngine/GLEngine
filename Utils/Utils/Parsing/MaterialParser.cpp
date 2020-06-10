#include <Utils/Parsing/MaterialParser.h>

#include <Utils/Parsing/ColorParsing.h>

#include <sstream>

namespace Utils::Parsing {

//=================================================================================
MaterialData C_MaterialParser::ParseMaterialData(const pugi::xml_node& node)
{
	MaterialData data;
	if (auto colorChild = node.child("color"))
	{
		data.m_Color = Utils::Parsing::C_ColorParser::ParseColorRGB(node);
	}

	if (auto roughness = node.child("roughness"))
	{
		std::stringstream ss;
		ss << roughness.child_value();
		float val;
		ss >> val;
		data.m_Roughness = val;
	}

	if (auto roughnessMap = node.child("roughnessMap"))
	{
		data.m_RoughtnessMap = roughnessMap.child_value();
	}

	if (auto colorMap = node.child("colorMap"))
	{
		data.m_RoughtnessMap = colorMap.child_value();
	}


	if (auto normalMap = node.child("normalMap"))
	{
		data.m_RoughtnessMap = normalMap.child_value();
	}

	return data;
}

}

