/**
 * Copyright (c) 2025 Sparr0www820
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
	const std::regex pattern(R"(quit\s*\(\s*(\d+)\s*\)\s*;)");
	if (std::smatch match; std::regex_search(source, match, pattern)) {
		ASTNode node;
		node.exitCode = stoi(match[1].str());
		return node;
	} else {
		throw std::runtime_error("Syntax error: Unrecognized statement.");
	}
}

std::string generateAsm(const ASTNode& node) {
	std::stringstream ss;
	ss << ".section .note.GNU-stack,\"\",@progbits\n"
	   << ".section .text\n"
	   << ".global main\n"
	   << "main:\n"
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

	std::string outputFilename = "output";
	std::string command = "gcc -o " + outputFilename + " " + asmFilename;
	if (system(command.c_str()) != 0) {
		std::cerr << "Error: Failed to compile assembly" << std::endl;
		return 1;
	}

	std::cout << "Compilation finished.\nExecutable: " << outputFilename << std::endl;

	if (argc >= 3) {
		std::string option = argv[2];
		if (option == "--clean") {
			remove(asmFilename.c_str());
		}
	}

	return 0;
}
