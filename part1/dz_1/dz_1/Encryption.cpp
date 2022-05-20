#include "Encryption.h"

XorEncriptionAdapter::XorEncriptionAdapter(FileWriter* p){
	//If pointer is null
	if (!p) throw std::runtime_error("Null pointer taked in Xor Adapter");
	p_fwriter=p;
}

void XorEncriptionAdapter::Encrypt(const int key) {
	//Key outside of range 0-255 repeat changes of symbols
	if (key < 0 || key>255) {
		throw std::logic_error("Bad value range for key");
	}

	try {
		std::string lineOfText;
		while (!(lineOfText = p_fwriter->ReadString()).empty()) {
			for (auto& i : lineOfText) {
				i ^= key;
			}
			p_fwriter->WriteString(lineOfText);
		}
		p_fwriter->CloseFiles();
	}
	catch (const std::exception& exception) {
		p_fwriter->CloseFiles();
		throw;
	}
}