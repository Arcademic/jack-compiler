#include "tokenizer.cpp"
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

static const string INPUT_TYPE = ".jack";
static const string OUTPUT_TYPE = ".vm";

bool init = false;

int main(int argc, char *argv[])
{
    fs::path path = argv[1];
    if (path.string().back() == '/') {
        path = path.string().substr(0, path.string().size() - 1);
    }
    
    string output_file_path = path.parent_path().string();
    string output_file_name = path.stem().string();

    cout << output_file_path << endl;
    cout << output_file_name << endl;

    return 0;
}