internal int Clamp(int min, int val, int max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}