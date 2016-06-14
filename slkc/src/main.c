#include <slkc/array.h>
#include <stdio.h>


int main(int argc, const char* argv[])
{
	sk_init_memory(SK_SIZE_MB2);
	int* myArray = sk_array(sk_lmalloc(), int, 1, 4);;
	for (int i = 0; i < 200; ++i)
	{
		sk_array_push(myArray, i);
	}
	for (usize i = 0; i < sk_array_length(myArray); ++i)
	{
		printf("%d\n", myArray[i]);
	}
	getchar();
	sk_shutdown_memory();
	return 0;
}