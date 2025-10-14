int main()
{
    int x = 5;
    int y = 10;
    const int* ptr_const_int = &x;

    // *ptr_const_int = 10; error
    ptr_const_int = &y; // fine

    int* const const_ptr = &x;
    *const_ptr = 20;

    const int* const const_const_ptr = &y;
    // this one is fully const
}