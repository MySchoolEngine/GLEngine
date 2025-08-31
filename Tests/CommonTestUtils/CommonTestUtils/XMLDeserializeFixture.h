#pragma once

#include <Core/Resources/ResourceManager.h>

#include <Utils/Serialization/XMLDeserialize.h>

#include <pugixml.hpp>

#include <gtest/gtest.h>

namespace GLEngine {
class XMLDeserializeFixture : public ::testing::Test {
public:
	XMLDeserializeFixture()
		: deserializer(Core::C_ResourceManager::Instance())
	{
	}

	static pugi::xml_document ConstructDocument(const std::string& content)
	{
		pugi::xml_document doc;
		std::stringstream  ss;
		ss << content;
		const auto status = doc.load(ss);
		EXPECT_EQ(status.status, pugi::xml_parse_status::status_ok);
		return doc;
	}

protected:
	Utils::C_XMLDeserializer deserializer;
};
} // namespace GLEngine