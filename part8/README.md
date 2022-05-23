# Разработка под macOS
## Оглавление
1. [Обзор системы, особенности](#r1)
    1) [Обзор файловых систем macOS](#r1_1)
    2. [Обзор безопасности macOS](#r1_2)
    3. [Application bundles](#r1_3)
    4. [Обзор исполняемого файла Macho file](#r1_4)
2. [Objective C](#r2)
    1. [Обзор языка](#r2_1)
    2. [Синтаксис](#r2_2)
    3. [Property](#r2_3)
    4. [Categories](#r2_4)
    5. [Protocols](#r2_5)
    6. [Memory management](#r2_6)
3. [Swift](#r3) 
    1. [Синтаксис](#r3_1) 
    2. [Closures](#r3_2) 
    3. [Generics](#r3_3) 
    4. [Extensions](#r3_4) 
4. [Objective C vs Swift](#r4)
5. [Frameworks](#r5)
6. [Литература](#r6)

## <a name="r1">Обзор системы, особенности</a>
**macOS** — *POSIX*-совместимая операционная система *Darwin*, являющаяся свободным программным обеспечением. Её ядром является *XNU*, в котором используется микроядро *Mach* и стандартные службы *BSD*. Все возможности *Unix* в *macOS* доступны через консоль.

### <a name="r1_1">Обзор файловых систем macOS</a>
В компьютерах Apple используется два основных типа файловых систем HFS+ и APFS. 

**HFS+** использует древовидную структуру, называемую B*-дерево, для хранения большей части метаданных.  

**APFS** увеличивает скорость чтения/записи данных с SSD, а также увеличивает пространство для хранния путем подсчитывания свободного места на диске.  

**Особенности APFS:**  
- Full-disk encryption;  
- 64-bit architecture;  
- Space sharing - если в одном условном разделе не хватает места для установки файла, автоматически будет использоваться пространство другого;  
- Snapshots - используются для более эффективного резервного копирования;  
- Clones - практически мгновенное копирование файла или директории, при котором не требуется дополнительное место для хранения данных. При модификации клона файловая система записывает только изменение данных. Таким образом, новая файловая система может хранить много версий больших файлов, отнимая меньше дискового пространства.

### <a name="r1_2">Обзор безопасности macOS</a>
**Keychain** - это зашифрованный контейнер, в котором безопасно хранятся пароли, учетные записи и другая конфиденциальная информация, которую обычно используют в ежедневной жизни.  

**System Integrity Protection** – фича, которую Apple разработали для обеспечения высокого уровня безопасности системы.  

**SIP включает следующие механизмы защиты:**
- защита файловой системы и пермиссий
- защита процессов от code injection
- защита от установки неподписанных kernel extension.

**Code signing** – используется для подтверждения, что приложение было создано конкретным разработчиком. После того, как приложение было подписано, система может обнаружить любое изменение в приложении, независимо от того, внесено ли это изменение случайно или с помощью вредоносного кода.  

**Notarization** – процесс проверки подписанного приложения компанией Apple на наличие вредоносных компонентов.

### <a name="r1_3">Application bundles</a>
**Bundle** – это файловая директория с определенной структурой и расширением файла, позволяющая группировать связанные файлы как концептуально единый элемент.
Bundle нужен для упрощения процесса установки и удаления приложения, так для установки приложения пользователю достаточно просто скопировать бандл на компьютер, а для удаления - просто удалить этот бандл.

##### **Пример application bundle для macOS:**
~~~Swift
MyApp.app/
   Contents/
      Info.plist
      MacOS/
      Resources/
~~~

| Директория/Файл | Описание |
| --------------- | -------- |
|MacOS|(Обязательная) Содержит исполняемый файл приложения, который запускается при нажатии двойным кликом на бандл. |
| Resources | Содержит все ресурсные файлы приложения. |
| Frameworks | Содержит любые фреймворки, библиотеки, которые используются исполняемым файлом |
| Info.plist | Содержит информацию, для идентификации приложения. Файл содержит данные в формате xml, которые определяют конфигурацию данного бандла. |

### <a name="r1_4">Обзор исполняемого файла Macho file</a>
**Macho file** – это нативный формат исполняемого файла / бинаря в macOS. Бинарный формат определяет порядок, по которому данные и код считываются в память системы.  

**Macho file состоит из трех частей:**
1. **Header** — здесь хранится общая информация о файле: магическое число (разрядность архитектуры) архитектура, тип файла (либа, драйвер, бандл и др), количество загружаемых команд и др.
2. **Load commands** — является основной частью Mach-O файла. Существует множетство различных лоад команд, парсинг которых зависит от типа файла, который указан в хидере.  
Команды загрузки  сообщают как и куда загружать части Mach-O: сегменты, таблицы символов, а также — от каких библиотек зависит этот файл, чтобы сперва загрузить их.  
**Некоторые команды загрузки:**
    - `LC_SEGMENT` — содержит различную информацию о некотором сегменте: размер, количество секций, смещение в файле и, после загрузки, в памяти;
    - `LC_SYMTAB` — загружает таблицу символов и строк;
    - `LC_DYSYMTAB` — создает таблицу импорта;
    - `LC_LOAD_DYLIB` — указывает зависимость от некоторой сторонней библиотеки

3. **Segments** — описывают регионы памяти, куда загружать секции с кодом или данными.  
**Наиболее важные сегменты следующие:**
    - `__TEXT` — исполняемый код и другие данные только для чтения;
    - `__DATA` — данные, доступные для записи; в том числе и таблицы импорта, которые имеют свойство изменяться динамическим загрузчиком во время позднего связывания;
    - `__OBJC` — различная информация стандартной библиотеки языка Objective-C времени выполнения;
    - `__LINKEDIT` — здесь динамический загрузчик располагает свои данные для уже загруженных модулей: таблицы символов, строк и прочее.

Чтобы посмотреть содержимое macho файла можно использовать или консольную утилиту otool или же гуювую утилиту MachoView.

## <a name="r2">Objective C</a>
### <a name="r2_1">Обзор языка</a>
Objective C – это объектно-ориентированный язык программирования, который совместим с С/С++. Objective C является message-oriented, то есть вызовы метода интерпретируются не как вызов функции, а именно как посылка сообщения (с именем и аргументами) объекту.

### <a name="r2_2">Синтаксис</a>
Класс описывается двумя файлами: заголовочным (.h) и реализации (.m).   
##### **Пример:**
~~~ObjC 
//
//  Book.h
//
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface Book : NSObject
{
    int _pageCount;
}

@property (nonatomic, readonly) NSString* name;

- (nonnull NSString*)showInfo;

@end

NS_ASSUME_NONNULL_END


//
//  Book.m
//
#import "Book.h"

@implementation Book

- (instancetype)initWithName:(nonnull NSString*)name pageCount:(int)pageCount
{
    self = [super init];
    if (self)
    {
        _name = name;
        _pageCount = pageCount;
    }

    return self;
}

- (nonnull NSString*)showInfo
{
    return [NSString stringWithFormat:@"Name: %@. Page count: %d", _name, _pageCount];
}

@end
~~~


### <a name="r2_3">Property</a>
Property предоставляют методы сеттер и геттер при обращении к данным.  
##### **Пример:**
~~~ObjC 
@property (nonatomic, readonly) NSString* name;
~~~

**Свойство может содержать различные атрибуты:**
- атрибуты доступности: `readwrite` / `readonly`;
- `nullability` атрибут: `nonnull` / `nullable`;
- атрибут атомарности: `atomic` / `nonatomic`;
- атрибуты владения: `retain` / `strong` / `copy` / `weak`;

### <a name="r2_4">Categories</a>
Категории используются для расширения функционала существующего класса.  
##### **Пример:**
~~~ObjC 
@interface NSString(MyAdditions)
+(NSString *)getCopyRightString;
@end

@implementation NSString(MyAdditions)
+(NSString *)getCopyRightString {
   return @"Copyright macOSDevelopment.com 2021";
}
@end
~~~

### <a name="r2_5">Protocols</a>
Протоколы позволяют определять методы, которые ожидаются, что будут реализованы в классах, которые наследуют определенный протокол. Протоколы очень похожи на интерфейсы в С#.  
##### **Пример:**
~~~ObjC 
@protocol TestProtocol
@required
-(void) test;

@optional
-(void) testWithName: (NSString *) name;
@end



@interface TestClass : NSObject <TestProtocol, NSCoding>
...
@end
~~~

### <a name="r2_6">Memory management</a>
**Существует два метода управления памятью в Objective-C:**
- "Manual Retain-Release" or MRR
- "Automatic Reference Counting" or ARC

**Manual Retain-Release** – явное управление памятью. Метод основан на подсчете ссылок, которые предоставляет NSObject.  
##### **Пример:**
~~~ObjC 
@interface SampleClass:NSObject
- (void)sampleMethod;
@end

@implementation SampleClass
- (void)sampleMethod {
   NSLog(@"Hello, World! \n");
}

- (void)dealloc  {
  NSLog(@"Object deallocated");
  [super dealloc];
}

@end

int main() {
   
   SampleClass *sampleClass = [[SampleClass alloc]init];
   [sampleClass sampleMethod];
   
   NSLog(@"Retain Count after initial allocation: %d", 
   [sampleClass retainCount]);
   [sampleClass retain];
   
   NSLog(@"Retain Count after retain: %d", [sampleClass retainCount]);
   [sampleClass release];
   NSLog(@"Retain Count after release: %d", [sampleClass retainCount]);
   [sampleClass release];
   NSLog(@"SampleClass dealloc will be called before this");
   
   sampleClass = nil;

   return 0;
}
~~~

**Вывод:**
~~~ObjC
2013-09-28 04:39:52.310 demo[8385] Hello, World!
2013-09-28 04:39:52.311 demo[8385] Retain Count after initial allocation: 1
2013-09-28 04:39:52.311 demo[8385] Retain Count after retain: 2
2013-09-28 04:39:52.311 demo[8385] Retain Count after release: 1
2013-09-28 04:39:52.311 demo[8385] Object deallocated
2013-09-28 04:39:52.311 demo[8385] SampleClass dealloc will be called before this
~~~

**Automatic Reference Counting** – подход управления памяти, только здесь уже система сама заботиться о подсчитывании ссылок на объект.
##### **Пример:**
~~~ObjC 
@interface SampleClass:NSObject
- (void)sampleMethod;
@end

@implementation SampleClass
- (void)sampleMethod {
   NSLog(@"Hello, World! \n");
}

- (void)dealloc  {
  NSLog(@"Object deallocated");
}

@end

int main() {
   @autoreleasepool {
      SampleClass *sampleClass = [[SampleClass alloc]init];
      [sampleClass sampleMethod];
      sampleClass = nil;
   }
   return 0;
}
~~~

**Вывод:**
~~~ObjC
2013-09-28 04:45:47.310 demo[8385] Hello, World!
2013-09-28 04:45:47.311 demo[8385] Object deallocated
~~~

### <a name="r3">Swift</a>
Swift – это новый язык программирования, разработанный Apple для разработки iOS и macOS. Swift соединил лучшее из С и Objective-C, без ограничений совместимости с С.

### <a name="r3_1">Синтаксис</a>
**Объявление переменных:**
~~~Swift
var variableName = <initial value>
~~~

Можно добавить **аннотацию типа:**
~~~Swift
var variableName:<data type> = <optional initial value>
~~~

Тип может быть **опциональным**, то есть переменная может содержать значение указанного типа и вообще ничего:
~~~Swift
var perhapsInt: Int?
~~~

**Объявление констант:**
~~~Swift
let variableName = <initial value>
~~~

**Синтаксис функций:**
~~~Swift
func funcname(Parameters) -> returntype {
   Statement1
   Statement2
   ---
   Statement N
   return parameters
}
~~~

**Синтаксис классов:**
~~~Swift
//
//  SwiftBook.swift
//  SwiftBook
//

import Foundation

class SwiftBook
{
    private let _pageCount: Int;
    private(set) var _name: String;

    init(name: String, pageCount: Int) {
        _pageCount = pageCount;
        _name = name;
    }

    func showInfo() -> String {
        return “Name: \( name). Page count: \( pageCount)”
    }
}


//
//  main.swift
//  SwiftBook
//

import Foundation

var book: SwiftBook = SwiftBook(name: "New Book", pageCount: 100);
print(book.showInfo())
~~~

### <a name="r3_2">Closures</a>
**Closures** - это самодостаточные блоки с определенным функционалом, которые могут быть переданы и использованы в вашем коде. Замыкания в Swift похожи на блоки в C и Objective-C, и лямбды в других языках программирования.

##### **Пример:**
~~~Swift
let divide = {
   (val1: Int, val2: Int) -> Int in 
   return val1 / val2 
}

let result = divide(200, 20)
print (result)
~~~

### <a name="r3_3">Generics</a>
**Generics** используются для написания гибких и многоразовых функций и типов.
##### **Пример:**
~~~Swift
func exchange<T>(a: inout T, b: inout T) {
   let temp = a
   a = b
   b = temp
}
var numb1 = 100
var numb2 = 200

print("Before Swapping Int values are: \(numb1) and \(numb2)")
exchange(a: &numb1, b: &numb2)
print("After Swapping Int values are: \(numb1) and \(numb2)")

var str1 = "Generics"
var str2 = "Functions"

print("Before Swapping String values are: \(str1) and \(str2)")
exchange(a: &str1, b: &str2)
print("After Swapping String values are: \(str1) and \(str2)")
~~~

### <a name="r3_3">Extensions</a>
Extensions это тоже самое, что и категории в Objective C. Позволяют расширять функционал существующего класса.
##### **Пример:**
~~~Swift
extension Int {
   var add: Int {return self + 100 }
   var sub: Int { return self - 10 }
   var mul: Int { return self * 10 }
   var div: Int { return self / 5 }
}

let addition = 3.add
print("Addition is \(addition)")

let subtraction = 120.sub
print("Subtraction is \(subtraction)")

let multiplication = 39.mul
print("Multiplication is \(multiplication)")

let division = 55.div
print("Division is \(division)")

let mix = 30.add + 34.sub
print("Mixed Type is \(mix)")
~~~

## <a name="r4">Objective C vs Swift</a>
**Критерии сравнения Objective C и Swift:**
1. **Призводительность**  
Swift обеспечивает высокую скорость выполнения программы и работает быстрее Objective C в 2,6 раз. Основная причина того, что Objective C работает медленее это использование компиляции в рантайм коде. Это означает, что когда объект Objective C вызывает другой объект в коде, возникает дополнительный уровень косвенного обращения, что является затратно при вызове множества обращений.
2. **Безопасность** 
Objective C предоставляет возможность вызывать метод с нулевым указателем. В такой момент ничего не происходит. Такое выражение в коде называется  no-operation (NOP). На первый взгляд это кажется полезным, посколько не будет никаких сбоев в случае обращения к нулевому указателю, однако это приводит к непредсказуемым результатам, которые трубно устранить и трудно найти.  
Swift сразу же бросает ошибку в случае обращения к пустому указателю.
3. **Поддержка**  
Разработчики Objective-C поддерживают два отдельных файла кода, чтобы повысить эффективность и время разработки приложения.   
Swift поддерживает один файл программного кода.
4. **Управление памятью**  
Objective C использует ARC для управления памятью объектно ориентированного кода. Однако для С (например Core Graphics) ARC не поддерживается.   
Swift также использует ARC, который поддерживается для всех APIs.

## <a name="r5">Frameworks</a>
Framework – иерархический каталог (бандл), который инкапсулирует общие ресурсы: динамические библиотеки, nib файлы, изображения, файлы локализации строк, хидеры, документацию в один пакет.  

**Популярные фреймворки для разработки под macOS:**
- Foundation – базовый фреймворк, который предоставляет классы примитивных объектов: NSString, NSMutableString, NSArray, NSDictionary, NSSet, Notifications и др.
- CoreData – содержит интерфейсы для управления моделью данных (кеш, база данных).
- CFNetwork – содержит интерфейсы для сетевого взаимодействия с использованием HTTP, сокетов и др.
- AVFoundation – предоставляет интерфейсы для воспроизведения, записи, редактирования аудиофайлов.
- CoreGraphics – предоставляет интерфейсы для работы с графическим контеном.
- IOKit – содержит основные интерфейсы для создания драйверов, взаимодействия с драйверами ядра.
 
## <a name="r6">Литература:</a>
1. macOS internals: 
    - https://developer.apple.com/library/archive/documentation/MacOSX/Conceptual/OSX_Technology_Overview/About/About.html#//apple_ref/doc/uid/TP40001067
    - http://newosxbook.com/MOXiI.pdf

2. File System:
    - https://ru.bmstu.wiki/HFS_Plus
    - https://habr.com/ru/post/395253/
    - https://ddr5.com.ua/faylovye-sistemy-mac-os/

3. Application bundles:
    - https://developer.apple.com/library/archive/documentation/CoreFoundation/Conceptual/CFBundles/BundleTypes/BundleTypes.html#//apple_ref/doc/uid/10000123i-CH101-SW1

4. Security:
    - https://developer.apple.com/documentation/security/notarizing_macos_software_before_distribution
    - https://developer.apple.com/library/archive/documentation/Security/Conceptual/CodeSigningGuide/Introduction/Introduction.html

5. Macho files:
    - https://redmaple.tech/blogs/macho-files/
    - https://developer.apple.com/library/archive/documentation/Performance/Conceptual/CodeFootprint/Articles/MachOOverview.html

6. Objective C:
    - https://www.tutorialspoint.com/objective_c/index.htm
    - https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/ProgrammingWithObjectiveC/Introduction/Introduction.html

7. Swift:
    - https://docs.swift.org/swift-book/GuidedTour/GuidedTour.html
    - https://www.tutorialspoint.com/swift/swift_closures.htm
    - https://swiftbook.ru/content/swift-tour/

8. macOS  Frameworks:
    - https://developer.apple.com/library/archive/documentation/MacOSX/Conceptual/OSX_Technology_Overview/SystemFrameworks/SystemFrameworks.html
