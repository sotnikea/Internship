#pragma once
#include <string>
#include "FileWriter.h"
#include <exception>

class IEncrypt {
public:
	virtual void Encrypt(const int key) = 0;
	virtual ~IEncrypt() {}
};

class XorEncriptionAdapter : public IEncrypt {
public:
	explicit XorEncriptionAdapter(FileWriter* p);
	~XorEncriptionAdapter() {};
	void Encrypt(const int _key);

private:
	FileWriter* p_fwriter;
};