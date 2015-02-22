#include<string>
#include<Regex.hpp>
#include<emscripten.h>

int main()
{
	const char *phoneRegex = "^[0-9]*$"; 
	int m1 = nbdl::Regex::match(phoneRegex, "5054567894");
	int m2 = nbdl::Regex::match(phoneRegex, "505a");

	EM_ASM_({
		document.write(
			"m1: " + $0 + "\n" +
			"m2: " + $1 + "\n" +
			"done."
		);
	}, m1, m2);
}
