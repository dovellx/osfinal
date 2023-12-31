void myprint(char* msg, int len);

int add_two_num(int a, int b)
{
	myprint("The sum of two numbers is ", 26);
        char c[5];
        c[0] = a + b + '0';
        c[1] = '\n'; 
    myprint(c, 2);
	return 0;
}
