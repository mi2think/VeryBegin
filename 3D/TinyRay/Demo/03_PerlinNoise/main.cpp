/********************************************************************
	created:	2016/08/02
	created:	2:8:2016   15:49
	filename: 	D:\Code\TinyRay\Demo\03_PerlinNoise\main.cpp
	file path:	D:\Code\TinyRay\Demo\03_PerlinNoise
	file base:	main
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test Perlin Noise
*********************************************************************/
#include "core/perlinnoise.h"
#include "core/image.h"
#include "core/rgb.h"
#include "math/rng.h"
#include "math/vector2.h"
using namespace tinyray;

int main()
{
	int imageWidth = 512;
	int imageHeight = 512;
	float* noiseMap = new float[imageWidth * imageHeight]{ 0 };

#if 0
	const char* fileName = "perlin_noise.ppm";
	// generate perlin noise
	PerlinNoise2D noise;
	float frequency = 0.1f;
	for (int j = 0; j < imageHeight; ++j)
	{
		for (int i = 0; i < imageWidth; ++i)
		{
			*(noiseMap + j * imageWidth + i) = (noise.Eval(Vector2f(i, j) * frequency) + 1) * 0.5f;
		}
	}
#elif 0
	const char* fileName = "fractal_pattern.ppm";
	// generate fractal pattern
	PerlinNoise2D noise;
	float frequency = 0.02f;
	float frequencyMult = 1.8f;
	float amplitudeMult = 0.35f;
	int layers = 5;
	float maxNoiseVal = 0;
	for (int j = 0; j < imageHeight; ++j)
	{
		for (int i = 0; i < imageWidth; ++i)
		{
			Vector2f ptNoise = Vector2f(i, j) * frequency;
			float amplitude = 1;
			for (int k = 0; k < layers; ++k)
			{
				noiseMap[j * imageWidth + i] += (noise.Eval(ptNoise) + 1) * 0.5f * amplitude;
				ptNoise *= frequencyMult;
				amplitude *= amplitudeMult;
			}
			if (noiseMap[j * imageWidth + i] > maxNoiseVal)
				maxNoiseVal = noiseMap[j * imageWidth + i];
		}
	}
	for (int i = 0; i < imageWidth * imageHeight; ++i)
	{
		noiseMap[i] /= maxNoiseVal;
	}
#elif 0
	const char* fileName = "turbulence_pattern.ppm";
	// generate turbulence pattern
	PerlinNoise2D noise;
	float frequency = 0.02f;
	float frequencyMult = 1.8f;
	float amplitudeMult = 0.35f;
	int layers = 5;
	float maxNoiseVal = 0;
	for (int j = 0; j < imageHeight; ++j)
	{
		for (int i = 0; i < imageWidth; ++i)
		{
			Vector2f ptNoise = Vector2f(i, j) * frequency;
			float amplitude = 1;
			for (int k = 0; k < layers; ++k)
			{
				//[-1, 1] -> ABS([-1, 1])
				noiseMap[j * imageWidth + i] += ABS(noise.Eval(ptNoise)) * amplitude;
				ptNoise *= frequencyMult;
				amplitude *= amplitudeMult;
			}
			if (noiseMap[j * imageWidth + i] > maxNoiseVal)
				maxNoiseVal = noiseMap[j * imageWidth + i];
		}
	}
	for (int i = 0; i < imageWidth * imageHeight; ++i)
	{
		noiseMap[i] /= maxNoiseVal;
	}
#elif 0
	const char* fileName = "marble_pattern.ppm";
	// generate marble pattern
	PerlinNoise2D noise;
	float frequency = 0.02f;
	float frequencyMult = 1.8f;
	float amplitudeMult = 0.35f;
	int layers = 5;
	for (int j = 0; j < imageHeight; ++j)
	{
		for (int i = 0; i < imageWidth; ++i)
		{
			Vector2f ptNoise = Vector2f(i, j) * frequency;
			float amplitude = 1;
			float noiseValue = 0;
			// compute some fractal noise
			for (int k = 0; k < layers; ++k)
			{
				noiseValue += (noise.Eval(ptNoise) + 1) * 0.5f * amplitude;
				ptNoise *= frequencyMult;
				amplitude *= amplitudeMult;
			}
			// we displace the value i used in sin expression by noiseValue * 100
			noiseMap[j * imageWidth + i] = (sin((i + noiseValue * 100) * 2 * M_PI / 200.0f) + 1) / 2.0f;
		}
	}
#else
	const char* fileName = "wood_pattern.ppm";
	// generate wood pattern
	PerlinNoise2D noise;
	float frequency = 0.01f;
	for (int j = 0; j < imageHeight; ++j)
	{
		for (int i = 0; i < imageWidth; ++i)
		{
			float g = (noise.Eval(Vector2f(i, j) * frequency) + 1) * 0.5f * 10;
			*(noiseMap + j * imageWidth + i) = g - int(g);
		}
	}
#endif

	// output noise map to PPM
	PPM image(imageWidth, imageHeight);
	RGB* pixel = &(image.GetRaster()[0]);
	for (int i = 0; i < imageWidth * imageHeight; ++i)
	{
		float f = noiseMap[i];
		*(pixel + i) = RGB(f, f, f);
	}
	image.SavePPM(fileName);

	delete[] noiseMap;
	return 0;
}
