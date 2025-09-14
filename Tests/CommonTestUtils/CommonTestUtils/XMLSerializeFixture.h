#pragma once

#include <Utils/Serialization/XMLSerialize.h>

#include <pugixml.hpp>

#include <gtest/gtest.h>

#include <string>

class XMLSerializeFixture : public ::testing::Test {
public:
	static std::string ToString(const pugi::xml_document& doc)
	{
		std::stringstream ss;
		doc.save(ss);
		return ss.str();
	}

protected:
	GLEngine::Utils::C_XMLSerializer serializer;
};
