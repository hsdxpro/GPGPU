constant int resX = 640;
constant int resY = 480;

class Raytracer
{
public:
	uchar3 GetColor() { return (uchar3)(255,0,0); }
};

kernel void hello(global uchar* out)
{
	size_t x = get_global_id(0) * 3;
	size_t y = get_global_id(1) * 3;

	Raytracer rt;
	uchar r = rt.GetColor().x;
	uchar g = rt.GetColor().y;
	uchar b = rt.GetColor().z;

	out[x + resX * y + 0] = r;
	out[x + resX * y + 1] = g;
	out[x + resX * y + 2] = b;
}