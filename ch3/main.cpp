// if you are using fmt instead, swap the last include for the next two lines and change std::format to fmt::format
// #define FMT_HEADER_ONLY
// #include <fmt/core.h>

#include <cassert>
#include <format>
#include <functional>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <vector>

using std::cin, std::cout;
using namespace std;

// Listing 3.1
unsigned
some_const_number ()
{
  return 42;
}

// Listing 3.2
unsigned
input ()
{
  unsigned number; // try a negative number!
  while (!(cin >> number))
    {
      cin.clear ();
      cin.ignore (std::numeric_limits<std::streamsize>::max (), '\n');
      cout << "Please enter a number.\n>";
    }
  return number;
}

// Listing 3.3
void
guess_number (unsigned number)
{
  cout << "Guess the number.\n>";
  unsigned guess = input ();
  while (guess != number)
    {
      cout << guess << " is wrong. Try again\n>";
      guess = input ();
    }
  cout << "Well done.\n";
}

// Listing 3.4 Taking optional input
// Allow a way to give up
optional<unsigned>
read_number (std::istream &in)
{
  unsigned result{};
  if (in >> result)
    {
      return result;
    }
  in.clear ();
  in.ignore (numeric_limits<streamsize>::max (), '\n');
  return {};
}

// Listing 3.5 Allow giving up
void
guess_number_or_give_up (int number)
{
  cout << "Guess the number.\n>";
  optional<int> guess;
  while ((guess = read_number (cin)))
    {
      if (guess.value () == number)
        {
          cout << "Well done.\n";
          return;
        }
      cout << guess.value () << " is wrong. Try again\n>";
    }
  cout << "The number was " << number << "\n";
}

// Listing 3.9
unsigned
some_random_number ()
{
  random_device              rd;
  mt19937                    mt (rd ());
  uniform_int_distribution<> dist (0, 100);
  return dist (mt);
}

// Listing 3.11 Function to check if a number is prime
constexpr bool
is_prime (int n)
{
  // https://en.wikipedia.org/wiki/Primality_test
  if (n == 2 || n == 3)
    {
      return true;
    }

  if (n <= 1 || n % 2 == 0 || n % 3 == 0)
    {
      return false;
    }

  for (int i = 5; i * i <= n; i += 6)
    {
      if (n % i == 0 || n % (i + 2) == 0)
        {
          return false;
        }
    }

  return true;
}

// Listing 3.14, which started as listing 3.13, a function to indicate which digits are correct
// * means correct in the right place
// ^ means correct in the wrong place
// . means wrong
string
check_which_digits_correct (unsigned number, unsigned guess)
{
  auto        ns = format ("{:0>5}", (number));
  auto        gs = format ("{:0>5}", (guess));
  std::string matches (5, '.');
  // which for guesses match...
  for (size_t i = 0, stop = gs.length (); i < stop; ++i)
    {
      char guess_char = gs[i];
      if (i < ns.length () && guess_char == ns[i])
        {
          matches[i] = '*';
          ns[i]      = '*'; // don't reuse this digit
        }
    }
  // now for guesses that don't match...
  for (size_t i = 0, stop = gs.length (); i < stop; ++i)
    {
      char guess_char = gs[i];
      if (i < ns.length () && matches[i] != '*')
        {
          if (size_t idx = ns.find (guess_char, 0); idx != std::string::npos)
            {
              idx        = ns.find (guess_char, idx);
              matches[i] = '^';
              ns[idx]    = '^';
            }
        }
    }
  return matches;
}

void
check_properties ()
{
  static_assert (is_prime (2));
  static_assert (is_prime (7321));
  static_assert (is_prime (56897));
  static_assert (is_prime (41521));
  auto got = check_which_digits_correct (12347, 11779);
  assert (got == "*.^..");
  got = check_which_digits_correct (12345, 23451);
  assert (got == "^^^^^");
  got = check_which_digits_correct (12345, 12345);
  assert (got == "*****");
  got = check_which_digits_correct (48533, 12345);
  assert (got == "..^^^");
  got = check_which_digits_correct (98041, 41141);
  assert (got == "...**");
  assert (is_prime (17231));
  got = check_which_digits_correct (1723, 17231);
  assert (got == "^^^^.");
  unsigned number = 78737;
  got             = check_which_digits_correct (number, 87739);
  assert (got == "^^**.");
}

// Listing 3.15 A much better number guessing game
void
guess_number_with_clues (unsigned number, auto message)
{
  cout << "Guess the number.\n>";
  optional<unsigned> guess;
  while ((guess = read_number (std::cin)))
    {
      if (guess.value () == number)
        {
          cout << "Well done.\n";
          return;
        }
      cout << message (number, guess.value ());
      cout << '>';
    }
  cout << format ("The number was {}\n", number);
}

// Listing 3.12 Generating a prime number
int
some_prime_number ()
{
  random_device                 rd;
  mt19937                       mt{ rd () };
  uniform_int_distribution<int> dist{ 0, 99999 };
  int                           n{};
  while (!is_prime (n))
    {
      n = dist (mt);
    }
  return n;
}

// Listing 3.19 Using all the clues
void
guess_number_with_more_clues (int number, auto messages)
{
  cout << "Guess the number.\n>";
  optional<int> guess;
  while ((guess = read_number (std::cin)))
    {
      if (guess.value () == number)
        {
          cout << "Well done.";
          return;
        }
      cout << format ("{:0>5} is wrong. Try again\n", guess.value ());
      for (auto message : messages)
        {
          auto clue = message (guess.value ());
          if (clue.length ())
            {
              cout << clue;
              break;
            }
        }
    }
  cout << format ("The number was {:0>5}\n", (number));
}

int
main ()
{
  check_properties ();

  // guess a number without a clue
  guess_number (some_const_number ());

  // alow a non-number to indicate defeat
  guess_number_or_give_up (some_const_number ());

  // Listing 3.10
  // Guess any number with a clue:
  guess_number_with_clues (some_random_number (), [] (int number, int guess) {
    return format ("Your guess was too {}\n", (guess < number ? "small" : "big"));
  });

  // Listing 3.20
  // Guess a prime number:
  auto check_prime = [] (int guess) { return string ((is_prime (guess)) ? "" : "Not prime\n"); };

  auto check_length = [] (int guess) { return string ((guess < 100000) ? "" : "Too long\n"); };

  const int number       = some_prime_number ();
  auto      check_digits = [number] (int guess) { return format ("{}\n", check_which_digits_correct (number, guess)); };
  vector<function<string (int)> > messages{ check_length, check_prime, check_digits };
  guess_number_with_more_clues (number, messages);
}
