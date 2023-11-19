//
// Created by ajaxian on 11/14/23.
//

#include "Image.h"

#include "imgui.h"

#include "Application.h"
#include "imgui_impl_opengl3_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#ifdef INCLUDE_GL
#include <GL/gl.h>
#endif

#ifdef INCLUDE_OPENGL
#include <OpenGL/gl.h>
#endif


namespace Orange {

	Image::Image(std::string_view path)
		: m_Filepath(path)
	{
		int width, height, channels;
		uint8_t* data = nullptr;

		if (stbi_is_hdr(m_Filepath.c_str()))
		{
			data = (uint8_t*)stbi_loadf(m_Filepath.c_str(), &width, &height, &channels, 4);
			m_Format = ImageFormat::RGBA32F;
		}
		else
		{
			data = stbi_load(m_Filepath.c_str(), &width, &height, &channels, 4);
			m_Format = ImageFormat::RGBA;
		}

		m_Width = width;
		m_Height = height;

        // Create a OpenGL texture identifier
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//            // This is required on WebGL for non power-of-two textures
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//            // This is required on WebGL for non power-of-two textures

        // Upload pixels into texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

        m_DescriptorSet = image_texture;

	}

	Image::Image(int width, int height, ImageFormat format, const void* data, int data_size)
		: m_Width(width), m_Height(height), m_Format(format)
	{
		if (data) {
            int channels;

            m_Width = width;
            m_Height = height;
            m_Data = malloc(data_size);
            memcpy(m_Data, data, data_size);

            // Create a OpenGL texture identifier
            GLuint image_texture;
            glGenTextures(1, &image_texture);
            glBindTexture(GL_TEXTURE_2D, image_texture);

            // Setup filtering parameters for display
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//            // This is required on WebGL for non power-of-two textures
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//            // This is required on WebGL for non power-of-two textures

            // Upload pixels into texture
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            m_DescriptorSet = image_texture;
        }
	}

	Image::~Image()
	{
        if (m_Data != NULL) {
            free(m_Data);
            m_Data = NULL;
        }
	}

    void* Image::Decode(const void* buffer, int length, int* outWidth, int* outHeight)
    {
        int width, height, channels;
        uint8_t* data = nullptr;
        uint64_t size = 0;

        data = stbi_load_from_memory((const stbi_uc*)buffer, length, &width, &height, &channels, 4);
        size = width * height * 4;

        *outWidth = width;
        *outHeight = height;

        return data;
    }
}
