#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

class FileWriter {
	std::string sourceFileName;
	std::string destinationFileName;
	std::ifstream sourceFilePointer;
	std::ofstream destinationFilePointer;

public:
	explicit FileWriter(const std::string _sourceFileName, const std::string _destinationFileName);
	const std::string ReadString();
	void WriteString(const std::string bufer);
	void CopyTextFromFile();
	void CloseFiles();
	~FileWriter();
};
