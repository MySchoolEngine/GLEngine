#version 430
layout(std430, binding = 3) buffer terrainStats
{
	float m_max;
	float m_min;
	float m_avg;
}
stats;

// highly unoptimal
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(binding = 0, rgba32f) readonly uniform image2D terrainPatch;


//=================================================================================
void main()
{
	ivec2 size	= imageSize(terrainPatch);
	stats.m_max = 0;
	stats.m_min = imageLoad(terrainPatch, ivec2(0, 0)).x;
	stats.m_avg = 0;
	float sum	= 0;
	for (int i = 0; i < size.x; ++i)
	{
		for (int j = 0; j < size.y; ++j)
		{
			float value = imageLoad(terrainPatch, ivec2(i, j)).x;
			sum += value;
			stats.m_min = min(stats.m_min, value);
			stats.m_max = max(stats.m_max, value);
		}
	}

	stats.m_avg = sum / (size.x * size.y);
}