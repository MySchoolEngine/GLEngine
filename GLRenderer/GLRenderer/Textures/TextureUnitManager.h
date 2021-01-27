#pragma once


namespace GLEngine::GLRenderer::Textures {

class C_Texture;

class C_TextureUnitManger {
public:
	C_TextureUnitManger(C_TextureUnitManger const&) = delete;
	void						operator=(C_TextureUnitManger const&) = delete;
	static C_TextureUnitManger& Instance();

	void					   BindTextureToUnit(const C_Texture& texture, unsigned int unit);
	[[nodiscard]] bool		   UnitHasTexture(unsigned int unit) const;
	[[nodiscard]] bool		   IsTextureBindInUnit(const C_Texture& texture, unsigned int unit) const;
	[[nodiscard]] bool		   IsTextureBindInAny(const C_Texture& texture) const;
	[[nodiscard]] unsigned int GetTextureUnit(const C_Texture& texture) const; //< returns -1 if !IsTextureBindInAny(texture)

	// todo image should also store other infos than simple boolean
	void					   BindImageToUnit(const C_Texture& image, unsigned int unit, E_OpenGLAccess access);
	[[nodiscard]] bool		   UnitHasImage(unsigned int unit) const;
	[[nodiscard]] bool		   IsImageBindInUnit(const C_Texture& image, unsigned int unit) const;
	[[nodiscard]] bool		   IsImageBindInAny(const C_Texture& image) const;
	[[nodiscard]] unsigned int GetImageUnit(const C_Texture& image) const; //< returns -1 if !IsTextureBindInAny(texture)

private:
	// map from texturing unit to texture
	std::map<unsigned int, GLuint> m_TextureUnits;
	std::map<unsigned int, GLuint> m_ImageUnits;
	C_TextureUnitManger();
};


} // namespace GLEngine::GLRenderer::Textures