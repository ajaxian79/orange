//
// Created by ajaxian on 11/14/23.
//

#ifndef ORANGE_IMAGE_H
#define ORANGE_IMAGE_H

#include <string>
#include <imgui.h>

namespace Orange {

	enum class ImageFormat
	{
		None = 0,
		RGBA,
		RGBA32F
	};

	class Image
	{
	public:
		Image(std::string_view path);
		Image(int width, int height, ImageFormat format, const void* data = nullptr, int data_size = 0);
		~Image();

        ImTextureID GetDescriptorSet() const { return m_DescriptorSet; }

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

        static void* Decode(const void* data, int length, int* outWidth, int* outHeight);
	private:
		int m_Width = 0, m_Height = 0;
        void* m_Data = NULL;

		ImageFormat m_Format = ImageFormat::None;

		size_t m_AlignedSize = 0;

        ImTextureID m_DescriptorSet = (ImTextureID)0;

		std::string m_Filepath;
	};

}

#endif