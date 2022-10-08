#include "Renderer.h"
#include "Walnut/Random.h"

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// No resize needed
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{

	float aspectRetio = (float)m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();
	coord.x *= aspectRetio;

	glm::vec3 rayOrigin(0.0f, 0.0f, +2.0f);
	glm::vec3 rayDir(coord.x, coord.y, -1.0f);
	float radius = 0.5f;

	// solve quadratic formula to find t hit distance

	float a = glm::dot(rayDir, rayDir);
	float b = 2.0f * glm::dot(rayOrigin, rayDir);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	// discriminant b^2 -4ac

	float discriminant = (b * b) - (4.0f * a * c);

	if (discriminant >= 0)
	{
		float t1 = (-b + glm::sqrt(discriminant)) / (2 * a);
		float t2 = (-b - glm::sqrt(discriminant)) / (2 * a);

		float t = glm::min(t1, t2);

		glm::vec3 LightDir(-1.f, -1.f, 1.f);
		LightDir = glm::normalize(LightDir);

		glm::vec3 hitCoord;
		hitCoord =  rayOrigin + rayDir * t;
		glm::vec3 faceNormal = hitCoord - glm::vec3(0.0f);
		faceNormal = glm::normalize(faceNormal);


		//uint8_t r = (uint8_t)(255.0f / (t * t));
		//uint8_t g = (uint8_t)(255.0f / (t * t));
		//uint8_t b = (uint8_t)(255.0f / (t * t));

		faceNormal = faceNormal * 0.5f + 0.5f;
		float result = glm::max(glm::dot(faceNormal, -LightDir),0.1f);

		uint8_t r = (uint8_t)(255.0f * faceNormal.x * result);
		uint8_t g = (uint8_t)(255.0f * faceNormal.y * result);
		uint8_t b = (uint8_t)(255.0f * faceNormal.z * result);

		return 0xff000000 | (b << 16) | (g << 8) | (r);
	}
	return 0xff000000;
}


void Renderer::Render()
{
	// Render every pixel on viewPort
	uint32_t width = m_FinalImage->GetWidth();
	uint32_t height = m_FinalImage->GetHeight();
	for (uint32_t  y = 0; y < height; y++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			glm::vec2 coord = {(float)x / (float)width,(float)y / (float)height };

			coord = coord * 2.0f - 1.0f;// -1 to 1
			m_ImageData[x+y* width] = PerPixel(coord);
		}
		
	}

	m_FinalImage->SetData(m_ImageData);

}