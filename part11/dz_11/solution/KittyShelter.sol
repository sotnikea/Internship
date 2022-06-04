pragma solidity 0.5.0;
import {KittenRegistry} from "./KittenRegistry.sol";

contract KittyShelter{
    //ref to KittenRegistry database
    KittenRegistry m_registry;  

    //adsress of owner
    address owner;

    //information about added to the shelter cats
   struct info_cats{       
        uint256 m_catId;  
        uint256 m_returnDate;       
   } 
  
    //every client can have a few cats    
    struct clientInfo{
        info_cats[] m_cats;   
    }
    
       
   
    //information about clients of shelter
    mapping(address => clientInfo) clients;

    //save information about owner in constructor
    constructor()public{      
       owner = msg.sender;       
    }

    //set information about registryDatabase address
    function setRegistryAdress(KittenRegistry registry) public{        
        require(owner == msg.sender, "Address of client doesn't matches with adrees running those functione");  
        m_registry = registry;        
    }

    //sanding cat to shelter
    function storeKitty(uint256 catId, uint256 time) public{   
        //check if owner already approve transfer cat to shelter
        require(m_registry.getApproved(catId) == address(this), "Cat transfer is not approved yet");    

        //transfer cat from owner to shelter
        m_registry.transferFrom(msg.sender,address(this),catId);  

        //create information about added to shelter cat and his owner    
        info_cats memory tmp;

        //set cat id
        tmp.m_catId = catId;

        //set return date
        //now - current date
        //time*86400 amount of seconds that cat must leave in shelter
        tmp.m_returnDate = now + time*86400;

        //add to the clients id of cat
        clients[msg.sender].m_cats.push(tmp); 
    }

    //take cat from shelter
    function retrieveKitty(uint256 catId) public{      
        //check if address of cat's owner matches with address of client that run function
        require(m_registry.ownerOf(catId) == address(this),"It is not your cat!");

        //check if there no data about such clients        
        require(clients[msg.sender].m_cats.length>0,"Can't find user!");

        //if clients send few cats to the shelter, find it index in array
        int256 index = getCatId(catId,msg.sender);

        //if index -1, such cat doesn't exist in shelter
        require(index != -1,"You don't leave cat with such id");

        //check end date
        require(clients[msg.sender].m_cats[uint256(index)].m_returnDate < now, "It's to early to take cat back");   

        //transfer cat back to owner  
        m_registry.transferFrom(address(this),msg.sender,catId);        

        //delete info about cat in shelter
        delete clients[msg.sender].m_cats[uint256(index)];     
    }

    function getCatId(uint256 id, address human) private view returns(int256){             
        for(uint256 i = 0; i < clients[human].m_cats.length; i++){
            if (clients[human].m_cats[i].m_catId == id){                
                return int256(i);                
            }            
        }        
        //if there no cat with such id
        return -1;       
    }
}