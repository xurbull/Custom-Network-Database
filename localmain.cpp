#include "directory.hpp"

using namespace std;

int main() {
	Directory myDatabase;
	Parser myParser;

	string input;

	// main loop
	while (true) {
		cout << "127.0.0.1> ";

		if (getline(cin, input)) {

			// parser
			vector<string> args = myParser.parse(input);

			if (args.empty())
				continue;

			// directory
			string result = myDatabase.execute(args);

			// 4. print
			cout << result << "\n";

		}
		else {
			cout << "Error reading input\n";
			continue;
		}

		// debug print
		myDatabase.debugPrint();
	}

	return 0;
} 