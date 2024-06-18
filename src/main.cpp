#include "tokenizer.cpp"
#include "parser.cpp"
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

static const string INPUT_TYPE = ".jack";
static const string INTERMEDIATE_TYPE = ".xml";
static const string OUTPUT_TYPE = ".vm";

bool init = false;

int main(int argc, char *argv[])
{
    fs::path path = argv[1];
    if (path.string().back() == fs::path::preferred_separator) {
        path = path.parent_path();
    }
    
    if (fs::is_directory(path)) {
        cout << "Input is a directory: " << path << endl;
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == INPUT_TYPE) {
                cout << "Found valid file: " << entry.path().filename() << '\n';

                Tokenizer t(entry.path());
                Parser p(t.tokenize());
            }
            cout << "Done." << '\n';
        }
    } else if (fs::is_regular_file(path) && path.extension() == INPUT_TYPE) {
        cout << "Input is a single file: " << path.filename() << '\n';

        Tokenizer t(path);
        Parser p(t.tokenize());

        cout << "Done." << '\n';
    } else {
        cout << "Invalid argument: " << path << '\n';
        cout << flush;
        return 1;
    }

    cout << flush;
    return 0;
}