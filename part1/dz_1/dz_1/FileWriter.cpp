#include "FileWriter.h"

FileWriter::FileWriter(const std::string _sourceFileName, const std::string _destinationFileName)
	:sourceFileName(_sourceFileName), 
	destinationFileName(_destinationFileName) {}

const std::string FileWriter::ReadString() {
	//if file for reading is not open
	if (!sourceFilePointer.is_open()) {
		sourceFilePointer.open(sourceFileName);
		if (!sourceFilePointer.is_open()) {
			throw std::runtime_error("Cann't open file for reading");
		}
	}

	std::string bufer;
	getline(sourceFilePointer, bufer);
	return bufer;
}

void FileWriter::WriteString(const std::string bufer) {
	if (!destinationFilePointer.is_open()) {
		destinationFilePointer.open(destinationFileName);
		if (!destinationFilePointer.is_open()) {
			throw std::runtime_error("Cann't open file for writing");
		}
	}
	destinationFilePointer << bufer << std::endl;
}

void FileWriter::CopyTextFromFile() {
	try {
		while (!sourceFilePointer.eof()) {
			std::string lineOfText = ReadString();
			WriteString(lineOfText);
		}
		CloseFiles();
	}
	catch(const std::exception& exception){
		CloseFiles();
		throw;
	}
}

void FileWriter::CloseFiles()
{
	if (sourceFilePointer.is_open()) {
		sourceFilePointer.close();
	}
	if (destinationFilePointer.is_open()) {
		destinationFilePointer.close();
	}
}


FileWriter::~FileWriter()
{
	CloseFiles();
}
