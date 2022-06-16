# Условие
Написать класс **CoolPlacesNearMe**, который выполняет следующее:

1. Принимает:
    - json c координатами мест (см. под условием);
    - принимает базовую точку: в задаче это будет офис на Владимира Великого 34Б (координаты **48.471207, 35.038810**)
    - принимает радиус поиска в км;
2. На выходе:
    - получаем список мест (названия) и расстояния до них в км;
    - выводим результат в консоль.

# Требования
Задача выполняется на языке **Swift** или **Objective C** – на выбор.

# Формат сдачи
Файл с решением **.swift** и файл **.json** с данными

# Входные данные 
.json файл в папке с файлом проекта
~~~json
{
   "candidates" : [
      {
         "geometry" : {
            "location" : {
               "lat" : 48.470247,
               "lng" : 35.036206
            }
         },
         "name" : "Coffee Life"
      },
      {
         "geometry" : {
            "location" : {
               "lat" : 48.469434,
               "lng" : 35.038985
            }
         },
         "name" : "Have Coffee"
      },
      {
         "geometry" : {
            "location" : {
               "lat" : 48.466561,
               "lng" : 35.045186
            }
         },
         "name" : "AROMA KAVA"
      },
      {
         "geometry" : {
            "location" : {
               "lat" : 48.463783,
               "lng" : 35.042595
            }
         },
         "name" : "Mr. Coffee"
      },
      {
         "geometry" : {
            "location" : {
               "lat" : 48.463516,
               "lng" : 35.048889
            }
         },
         "name" : "DoubleDecker"
      },
      {
         "geometry" : {
            "location" : {
               "lat" : 48.425694,
               "lng" : 35.025487
            }
         },
         "name" : "Mocco bar"
      },
      {
         "geometry" : {
            "location" : {
               "lat" : 48.451214,
               "lng" : 35.057789
            }
         },
         "name" : "Confetti"
      },
      {
         "geometry" : {
            "location" : {
               "lat" : 48.460239,
               "lng" : 35.069301
            }
         },
         "name" : "One dollar bar"
      },
      {
         "geometry" : {
            "location" : {
               "lat" : 48.459414,
               "lng" : 35.064731
            }
         },
         "name" : "Heisenberg Coffee Lab"
      },
      {
         "geometry" : {
            "location" : {
               "lat" : 48.423311,
               "lng" : 35.058121
            }
         },
         "name" : "Sopranos"
      }]
}

~~~

# Обзор решения
Создаем модель согласно структуры данных в json файле. Для формирования модели можно воспользоваться https://app.quicktype.io/

~~~Swift
struct PlacesList: Decodable {
    let candidates: [Candidates]
}

struct Candidates: Codable {
    let geometry: Geometry
    let name: String
}

struct Geometry: Codable {
    let location: Location
}

struct Location: Codable {
    let lat, lng: Double
}
~~~

Парсим данные из json файла под сформированную модель
~~~Swift
extension Bundle {
    func decode<T: Decodable>(file: String) -> T {
      
        //Check if json file with data doesn't exist
        guard let url = self.url(forResource: file, withExtension: nil) else {
            fatalError("Could not find \(file) in bundle.")
        }

        //Try load data from json file
        guard let data = try? Data(contentsOf: url) else {
            fatalError("Could not load \(file) from bundle.")
        }

        //Try parse data from json file
        guard let loadedData = try? JSONDecoder().decode(T.self,from:data) else{
            fatalError("Could not decode")
        }

        return loadedData
    }
}
~~~

Формируем класс, для работы с сохраненными в json файле локациями.    
Файл содержит:
- Конструктор, инициализируемый данными из json файла
- Структуру с полями для имени локации и расстояния к ней от базовой точки (для формирования данных для вывода)
- Метод расчета расстояния между двумя локациями. Стоит обратить внимание, что использована формула Гаверсинусов на основе Ортодромии. Данных подход не учитывает препятствия в виде строений и рассчитывает расстояние по прямой (в ином случае потребует подключение google api и библиотеке CoreLocation)
- Метод поиска локаций, находящихся в заданном радиусе от базовой точки

~~~Swift
class CoolPlacesNearMe{
  var allPlacesNear: PlacesList  

  //Use constructor to init data by data from json file
  init (fileName: String)  {        
    allPlacesNear = Bundle.main.decode(file: fileName)    
  } 

  //Structure for information about nearest to some point places
   struct Places{
      let m_name: String
      let m_dist: Double      
    }

  //Function for calculation distance between to point on the map
  //Use Haversine Formula based on Orthodromy
  func distanceBetweenTwoLocation(pointA: Location, pointB: Location) ->Double{
    let EARTH_RADIUS: Double = 6372795;//average radius of the earth in meters.
   
    //Converting angular coordinates to radians, for functions that work with radians
   let lat1 = pointA.lat * .pi / 180;
   let lat2 = pointB.lat * .pi / 180;
   let long1 = pointA.lng * .pi / 180;
   let long2 = pointB.lng * .pi / 180;
    
   //Get the angular distance in radians, and since in radians we simply multiply by the radius
   let a = acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(long2-long1));
   
    return EARTH_RADIUS * a/1000.0  //return distance in km
  }

  //Function for searching nearest places to some point
  func findNearestPlaces(basePoint: Location, radius: Double) -> [Places] {
    //information about all places that satisfying condition
    var listOfPlaces: [Places]=[]

    for p in allPlacesNear.candidates{
      let distance = distanceBetweenTwoLocation(pointA: p.geometry.location, pointB: basePoint)
      if distance<radius{       
        listOfPlaces.append(Places(m_name: p.name, m_dist: distance))       
      }
    }

    return listOfPlaces
  }  
  
}
~~~

Проверяем работу написанного класса
~~~Swift
//Base location by the task
let startLoc = Location(lat: 48.471207,lng: 35.038810)

//Create instance of CoolPlacesNearMe class and initialized it by json file
let myPlace = CoolPlacesNearMe (fileName: "coordinates.json")

//Get lists of place inside specified radius around base location
var listsOfPlace = myPlace.findNearestPlaces(basePoint: startLoc, radius: 1)

//Print all place to console
for p in listsOfPlace{
  print("Place: \(p.m_name) \nDistance: \(NSString(format:"%.2f", p.m_dist))km\n")
}
~~~

# Оценка результата
90 балов из 100  
Оценка не содержала фидбек с описанием возможных ошибок

# Ссылки
- Ортодромия: https://ru.wikipedia.org/wiki/%D0%9E%D1%80%D1%82%D0%BE%D0%B4%D1%80%D0%BE%D0%BC%D0%B8%D1%8F
- Формула гаверсинусов: https://www.kobzarev.com/programming/calculation-of-distances-between-cities-on-their-coordinates/
