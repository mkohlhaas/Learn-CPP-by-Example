#include <chrono>
// If are using gcc 12 or earlier or clang 15 or earlier you need to clone Howard Hinnant's library for the parse method
// and use it's date.h git clone https://github.com/HowardHinnant/date If you get errors with operator<<, you also need
// to use the date.h from this library Also, add using date::operator<<; just before any std::cout <<
// #include <date/date.h>
#include <cassert>
#include <iostream>
#include <optional>
#include <sstream>
#include <thread>

// Listing 4.2 Duration between two time points
void
duration_to_end_of_year ()
{
  using namespace std::chrono;
  using std::cout;

  time_point     now           = system_clock::now ();
  constexpr auto year          = 2022;
  auto           new_years_eve = year_month_day (std::chrono::year (year), month (12), day (31));
  auto           event         = sys_days (new_years_eve);
  duration       dur           = event - now;
  cout << dur << " until event\n";
}

// Listing 4.4 Use duration to move to a coarser representation
void
durations ()
{
  using namespace std::chrono;
  using std::cout;

  auto  nearly_a_day = hours{ 23 };
  days  a_day        = duration_cast<days> (nearly_a_day);
  hours round_trip   = a_day;
  cout << nearly_a_day << " cast to " << a_day << " and cast back to " << round_trip << '\n';
}

// Listing 4.5 Defining a duration
void
defining_a_duration ()
{
  using namespace std::chrono;
  using std::cout;
  using centuries = duration<long long, std::ratio_multiply<years::period, std::hecto> >;
  // A type alias, which we used to write like this:
  //    typedef duration<long long,
  //        std::ratio_multiply<years::period, std::hecto>> centuries;
  centuries two_hundred_years = centuries (2);
  seconds   sec               = two_hundred_years;
  hours     hrs               = two_hundred_years;
  days      day_count         = duration_cast<days> (two_hundred_years);
  cout << "Two centuries is approximately " << day_count << '\n';
}

// Listing 4.6 Writing and using a concept
//  They are called in requirements_and_concepts below, but result in a compliation failure
//  Try uncommenting the calls and see what happens
template <typename T> concept Quacks = requires (T t) { t.Quack (); };

template <typename T>
void
might_be_a_duck (T x)
{
  x.Quack ();
}

template <typename T>
requires Quacks<T> void
         must_be_a_duck (T x)
{
  x.Quack ();
}

void
also_must_be_a_duck (Quacks auto x)
{
  x.Quack ();
}

void
requirements_and_concepts ()
{
  // might_be_a_duck(42); // error left of '.Quack' must have class/struct/union
  // must_be_a_duck(42); //'must_be_a_duck': no matching overloaded function found, could be 'void must_be_a_duck(T)',
  // the associated constraints are not satisfied also_must_be_a_duck(42);
}

// Listing 4.7 How many days until the last day of the year?
void
countdown ()
{
  using namespace std::chrono;
  using std::cout;

  time_point now = system_clock::now ();
  const auto ymd = year_month_day{ floor<days> (now) };

  auto this_year     = ymd.year ();
  auto new_years_eve = this_year / December / 31;

  auto     event = sys_days (new_years_eve);
  duration dur   = event - now;
  cout << duration_cast<days> (dur) << " until event \n";
}

// Listing 4.8 Days until pay day
void
pay_day ()
{
  using namespace std::chrono;
  using std::cout;

  time_point now = system_clock::now ();
  const auto ymd = year_month_day{ floor<days> (now) };

  auto this_year = ymd.year ();

  auto     pay_day = ymd.year () / ymd.month () / Friday[last];
  auto     event   = sys_days (pay_day);
  duration dur     = event - now;
  cout << duration_cast<days> (dur) << " until pay day \n";
}

// Listing 4.9 A testable countdown
constexpr std::chrono::system_clock::duration
          countdown (std::chrono::system_clock::time_point start)
{
  using namespace std::chrono;

  auto days_only = floor<days> (start);

  const auto ymd = year_month_day{ days_only };

  auto this_year     = ymd.year ();
  auto new_years_eve = this_year / December / last;

  auto event = sys_days (new_years_eve);
  return event - start;
}

// Listing 4.12 Reading a date
std::optional<std::chrono::year_month_day>
read_date (std::istream &in)
{
  using namespace std::string_literals;
  using std::cout;

  auto                        format_str = "%Y-%m-%d";
  std::chrono::year_month_day date;
  if (in >> std::chrono::parse (format_str, date))
    {
      return date;
    }
  in.clear ();
  cout << "Invalid format. Expected " << format_str << '\n';
  return {};
}

// Listing 4.13 Countdown to any event
constexpr std::chrono::system_clock::duration
          countdown_to (std::chrono::system_clock::time_point now, std::chrono::year_month_day date)
{
  using namespace std::chrono;
  auto event = sys_days (date);
  return event - now;
}

// Listing 4.14 Countdown in local time
std::chrono::system_clock::duration
countdown_in_local_time (std::chrono::system_clock::time_point now, std::chrono::year_month_day date)
{
  using namespace std::chrono;
  auto sys_event = zoned_time (current_zone (), local_days{ date }).get_sys_time ();
  return sys_event - now;
}

// Listing 4.10 Check the countdown function
void
check_properties ()
{
  using namespace std::chrono;
  constexpr auto new_years_eve = 2022y / December / last;
  constexpr auto one_day_away  = sys_days{ new_years_eve } - 24h;
  constexpr auto result        = countdown (one_day_away);

  static_assert (duration_cast<days> (result) == days{ 1 });

  // An example of another test, not included in the text of the book
  auto now        = sys_days{ 2022y / March / 27 };
  auto difference = duration_cast<hours> (countdown_in_local_time (now, 2022y / March / 28));
  // assert(difference == 23h); // The assert works for the "Europe/London" time zone. Yours might vary
}

int
main ()
{
  using namespace std::chrono;
  using std::cout, std::string, std::cin, std::istringstream, std::optional;

  // Listing 4,2
  duration_to_end_of_year ();

  // Listing 4.3
  durations ();

  // Listing 4.4
  defining_a_duration ();

  // Listing 4.5
  requirements_and_concepts ();

  // Listing 4.6
  countdown ();

  // Listing 4.8
  pay_day ();

  // Listing 4.9
  cout << countdown (system_clock::now ()) << " until event \n"; // calling listing 4.7

  // Listing 4.10
  check_properties ();

  // Listing 4.11 Call the countdown in a loop
  for (int i = 0; i < 5; ++i)
    {
      std::this_thread::sleep_for (5000ms);
      auto dur = countdown (system_clock::now ());
      cout << duration_cast<seconds> (dur) << " until event\n";
    }

  // Listing 4.14 A general purpose countdown
  cout << "Enter a date\n>";
  string str;
  cin >> str;
  istringstream            in (str);
  optional<year_month_day> event_date = read_date (in);
  if (event_date)
    {
      auto dur = countdown_to (system_clock::now (), event_date.value ());
      cout << duration_cast<days> (dur) << " until " << event_date.value () << "\n";

      // Calling the function from listing 4.15
      // WARNING - needs the date library compiled.
      // See Rainer Grimm's website has instructions for compiling
      //   and using the library (https://www.modernescpp.com/index.php/calendar-and-time-zone-in-c-20-time-zones)
      // Or Howard Hinnant's instructions https://howardhinnant.github.io/date/tz.html#Installation
      // You need a curllib and to include the tz.cpp from the src folder in your date clone
      // So the build command will be along the line of
      // clang++ --std=c++20 main..cpp -o ./main..out
      //    /dev/date/src/tz.cpp
      //    -Wall -I/mnt/d/dev/date/include
      //    -I/dev/date/include
      //    -lcurl
      auto local_dur = countdown_in_local_time (system_clock::now (), event_date.value ());
      cout << duration_cast<hours> (local_dur) << " until " << event_date.value () << "in local time \n";
    }
}
