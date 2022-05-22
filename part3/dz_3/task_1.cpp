#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

class HandleGuard {
public:
	HandleGuard(FILE *fptr):m_fptr(fptr){}
	~HandleGuard() {
		fclose(m_fptr);
	}

private:
	FILE *m_fptr;
};


int main(void) {
	{
		FILE* fp = fopen("text.txt", "wb");		
		HandleGuard guard(fp);

		fp = fopen("text2.txt", "wb");	//lost connection with text.txt
		fclose(fp);	//close text2.txt
	}	//Destructor of guard close text.txt

	return 0;
}