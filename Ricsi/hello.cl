constant char hw[] = "Hello World\n";

kernel void hello(global char * out)
{
	size_t tid = get_global_id(0);
	out[tid] = hw[tid];
}