#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <regex>
#include <stdexcept>

struct ASTNode {
	int exitCode;
};

ASTNode parseSource(const std::string& source) {
	std::regex pattern(R"(quit\s*\(\s*(\d+)\s*\)\s*;)");
	std::smatch match;
	if (std::regex_search(source, match, pattern)) {
		ASTNode node;
		node.exitCode = stoi(match[1].str());
		return node;
	} else {
		throw std::runtime_error("Syntax error: Unrecognized statement.");
	}
}

std::string generateAsm(const ASTNode& node) {
	std::stringstream ss;
	ss << ".section .text\n"
	   << ".global _start\n"
	   << "_start:\n"
	   << "    mov $60, %rax\n"
	   << "    mov $" << node.exitCode << ", %rdi\n"
	   << "    syscall\n";
	return ss.str();
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <file.wf> [--clean]" << std::endl;
        return 1;
	}

	std::string filename = argv[1];
	if (filename.size() < 3 || filename.substr(filename.size() - 3) != ".wf") {
		std::cerr << "Error: Source file must have a .wf extension" << std::endl;
		return 1;
	}

	std::ifstream infile(filename);
	if (!infile) {
		std::cerr << "Error: Cannot open source file" << std::endl;
		return 1;
	}

	std::stringstream buffer;
	buffer << infile.rdbuf();
    std::string source = buffer.str();
    
	ASTNode ast;
	try {
		ast = parseSource(source);
	} catch (const std::exception& e) {
		std::cerr << "Parse error: " << e.what() << std::endl;
		return 1;
	}

	std::string asmCode = generateAsm(ast);
	std::string asmFilename = "temp.s";
	std::ofstream asmFile(asmFilename);
	if (!asmFile) {
		std::cerr << "Error: Unable to write assembly file " << asmFilename << std::endl;
		return 1;
	}
	asmFile << asmCode;
    asmFile.close();

	std::string objectFilename = "temp.o";
	std::string outputFilename = "output";
	std::string command = "as -o " + objectFilename + " " + asmFilename;
	if (system(command.c_str()) != 0) {
		std::cerr << "Error: Failed to compile assembly" << std::endl;
        return 1;
	}

	command = "ld -o " + outputFilename + " " + objectFilename;
	if (system(command.c_str()) != 0) {
		std::cerr << "Error: Failed to link object file" << std::endl;
        return 1;
	}

	std::cout << "Comilation finished. Executable: " << outputFilename << std::endl;

    if (argc >= 3) {
		std::string option = argv[2];
		if (option == "--clean") {
			remove(asmFilename.c_str());
			remove(objectFilename.c_str());
		}
	}

	return 0;
}
