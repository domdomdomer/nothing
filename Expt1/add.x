/*combine the arguments to be passed to the server side in a structure*/
struct numbers{
	int a;
	int b;
};

program ADD_PROG{
	version ADD_VERS{
	int add(numbers)=1;
	} = 1;
} = 0x4562877;
