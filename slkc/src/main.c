#include <slkc/array.h>
#include <stdio.h>


int main(int argc, const char* argv[])
{
	sk_init_gmalloc();
	int* myArray = sk_array(sk_gmalloc(), int, 1, 4);;
	for (int i = 0; i < 10; ++i)
	{
		sk_array_push(myArray, i);
	}
	for (int i = 0; i < 10; ++i)
	{
		printf("%d\n", myArray[i]);
	}
	getchar();
	return 0;
}