#include <Utils/Parsing/MaterialParser.h>

#include <Utils/Parsing/ColorParsing.h>

#include <sstream>

namespace Utils::Parsing {

//=================================================================================
MaterialData C_MaterialParser::ParseMaterialData(const pugi::xml_node& node)
{
	MaterialData data;

	if (auto materialAttr = node.attribute("material"))
	{
		data.m_MaterialName = materialAttr.value();
	}

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
		data.m_ColorMap = colorMap.child_value();
	}


	if (auto normalMap = node.child("normalMap"))
	{
		data.m_NormalMap = normalMap.child_value();
	}

	return data;
}

}

