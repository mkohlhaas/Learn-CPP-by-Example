#include <iostream>
#include <vector>

using std::cout, std::vector;

void
generate_triangle ()
{
  vector data{ 1 };
  for (auto number : data)
    {
      cout << number << ' ';
    }
  cout << '\n';
}

int
main ()
{
  generate_triangle ();
}
