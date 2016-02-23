#ifndef _PERLIN_NOISE_H_
#define _PERLIN_NOISE_H_

#include "../Common.h"

namespace tactical
{ 
	namespace math
	{
		class PerlinNoise2D
		{
		public:
			PerlinNoise2D();
			PerlinNoise2D(const PerlinNoise2D& rhs);
			PerlinNoise2D(double persistance, double frequency, double amplitude, int octaves, int seed);

			double GetHeight(double x, double y) const;

			inline double GetPersistance() const { return m_persistance; }
			inline double GetFrequency()   const { return m_frequency; }
			inline double GetAmplitude()   const { return m_amplitude; }
			inline int    GetOctaves()     const { return m_octaves; }
			inline int    GetSeed()        const { return m_seed; }

			void Set(double persistance, double frequency, double amplitude, int octaves, int seed);

			inline void SetPersistance(double persistance) { m_persistance = persistance; }
			inline void SetFrequency(double frequency)     { m_frequency = frequency; }
			inline void SetAmplitude(double amplitude)     { m_amplitude = amplitude; }
			inline void	SetOctaves(int octaves)            { m_octaves = octaves; }
			inline void SetSeed(int seed)				   { m_seed = seed; }

		private:
			double Total(double i, double j) const;
			double GetValue(double x, double y) const;
			double Interpolate(double x, double y, double a) const;
			double SmoothNoise(double x, double y) const;
			double Noise(int x, int y) const;

			double m_persistance, m_frequency, m_amplitude;
			int m_octaves, m_seed;
		};

		class SimplexNoise
		{
		public:
			SimplexNoise() {}
			SimplexNoise(double persistence, int octaves, int seed);

			float GetNoise1D(float x);
			float GetNoise2D(float x, float y);
			float GetNoise3D(float x, float y, float z);
			float GetNoise4D(float x, float y, float z, float w);

		private:
			float grad(int hash, float x);
			float grad(int hash, float x, float y);
			float grad(int hash, float x, float y, float z);
			float grad(int hash, float x, float y, float z, float w);

			static int p[256];
			static unsigned char simplex[64][4];

			int m_permutation[512];
			int m_permutationMod12[512];

			int m_seed;
			double m_persistance;
		};

	}
}

#endif