struct color
{
	color() { r = g = b = a = 1.f; };
	color(float R, float G, float B, float A) : r(R), g(G), b(B), a(A) {};
	color(int R, int G, int B, int A) : r(R / 255.f), g(G / 255.f), b(B / 255.f), a(A / 255.f) {};
	float r, g, b, a;
};