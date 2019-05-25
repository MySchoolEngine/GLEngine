#pragma once


namespace GLEngine {
namespace GLRenderer {
namespace Textures {

class C_Texture;

enum class E_Access : char {
	Read,
	Write,
	ReadWrite,
};

class C_TextureUnitManger {
public:
	C_TextureUnitManger(C_TextureUnitManger const&) = delete;
	void operator=(C_TextureUnitManger const&) = delete;
	static C_TextureUnitManger& Instance();

	void BindTextureToUnit(const C_Texture& texture, unsigned int unit);
	bool UnitHasTexture(unsigned int unit) const;
	bool IsTextureBindInUnit(const C_Texture& texture, unsigned int unit) const;
	bool IsTextureBindInAny(const C_Texture& texture) const;
	unsigned int GetTextureUnit(const C_Texture& texture) const; //< returns -1 if !IsTextureBindInAny(texture)

	// todo image should also store other infos than simple boolean
	void BindImageToUnit(const C_Texture& image, unsigned int unit, E_Access access);
	bool UnitHasImage(unsigned int unit) const;
	bool IsImageBindInUnit(const C_Texture& image, unsigned int unit) const;
	bool IsImageBindInAny(const C_Texture& image) const;
	unsigned int GetImageUnit(const C_Texture& image) const; //< returns -1 if !IsTextureBindInAny(texture)

private:
	// map from texturing unit to texture
	std::map<unsigned int, GLuint> m_TextureUnits;
	std::map<unsigned int, GLuint> m_ImageUnits;
	C_TextureUnitManger();
};


}}}