import Foundation

//Create model of data from file
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

//Parse data from json file
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


