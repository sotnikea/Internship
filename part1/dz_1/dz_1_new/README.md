# Ошибки в реализации
1. Зайвий кетч, достатньо виклику у деструкторі (-1 бал)  
~~~C++
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
~~~

2. `FileWriter* p` не найкращий неймінг (-1 бал)
3. Таке можна на стеку створювати. Якщо Ви хотіли продемонструвати `unique_ptr`, то треба було таки робити `unique_ptr<IEncrypt>` 
~~~C++
auto xorEncrypt = std::make_unique<XorEncriptionAdapter>(&test2);
~~~

# Обзор исправлений
В процессе...
