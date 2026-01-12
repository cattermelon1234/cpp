#include <algorithm>
#include <iostream>
#include <list>

template <typename T> std::list<T> sequential_quicksort(std::list<T> input) {
  // if input is empty, return
  if (input.empty()) {
    return input;
  }

  std::list<T> result;
  // choose pivot as first element of input
  result.splice(result.begin(), input, input.begin());
  T const &pivot = *result.begin();

  // partition around pivot
  auto divide_point = std::partition(input.begin(), input.end(),
                                     [&](T const &t) { return t < pivot; });

  // splice list and recursively sequential_quicksort
  std::list<T> lower_half;
  lower_half.splice(lower_half.begin(), input, input.begin(), divide_point);
  auto new_lower = sequential_quicksort(std::move(lower_half));
  auto new_higher = sequential_quicksort(std::move(input));

  // move the sorted halves to the right location
  result.splice(result.end(), new_higher);
  result.splice(result.begin(), new_lower);

  return result;
}

template <typename T> void print_list(std::list<T> const &l) {
  for (auto const &x : l)
    std::cout << x << " ";
  std::cout << "\n";
}

// ---- main test ----
int main() {
  std::list<int> data = {7, 2, 9, 4, 1, 5, 3, 8, 6};

  std::cout << "Before: ";
  print_list(data);

  auto sorted = sequential_quicksort(data);

  std::cout << "After:  ";
  print_list(sorted);
}
