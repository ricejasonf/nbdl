#include<iostream>
#include<string>

struct Cow {};
struct Dog {};

void speak(const int x, Cow)
{
	std::cout << "moooOOOO" << std::endl;
}
void speak(const int x, Dog)
{
	std::cout << "woof" << std::endl;
}

int main()
{
	speak(5, Cow{});
	speak(5, Dog{});
}
