void no_op(const int& b)
{
    // this does nothing
    return;
}

int main()
{
    // local variables, stored on stack
    int a;
    int b[10];

    // local variables, still on stack
    no_op(a);

    // dyanmic memory alloc; on stack. More costly but can outlive function scope and 
    // be used to store larger data structures
    int* ptr = new int[2];
    // running valgrind here shows 8 bytes lost since the pointer was not automatically deallocated
    
}