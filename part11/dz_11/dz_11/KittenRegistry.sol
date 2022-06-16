pragma solidity 0.5.0;

contract KittenRegistry {
    event Transfer(address indexed _from, address indexed _to, uint256 indexed _catId);
    event Approval(address indexed _owner, address indexed _approved, uint256 indexed _catId);
    event ApprovalForAll(address indexed _owner, address indexed _operator, bool _approved);

    mapping (uint256 => address) private _kittenOwners;
    mapping (uint256 => address) private _kittenApprovals;
    mapping (address => uint256) private _ownedCatsCount;
    address petStore;
    
    constructor(address _petStore) public {
        petStore = _petStore;
    }

    /**
     * @dev Gets the balance of the specified address
     * @param owner address to query the balance of
     * @return uint256 representing the amount owned by the passed address
     */
    function balanceOf(address owner) public view returns (uint256) {
        require(owner != address(0));
        return _ownedCatsCount[owner];
    }

    /**
     * @dev Gets the owner of the specified cat ID
     * @param catId uint256 ID of the cat to query the owner of
     * @return address currently marked as the owner of the given cat ID
     */
    function ownerOf(uint256 catId) public view returns (address) {
        address owner = _kittenOwners[catId];
        require(owner != address(0));
        return owner;
    }

    /**
     * @dev Approves another address to transfer the given cat ID
     * The zero address indicates there is no approved address.
     * There can only be one approved address per cat at a given time.
     * Can only be called by the cat owner or an approved operator.
     * @param to address to be approved for the given cat ID
     * @param catId uint256 ID of the cat to be approved
     */
    function approve(address to, uint256 catId) public {
        address owner = ownerOf(catId);
        require(to != owner);
        require(msg.sender == owner);

        _kittenApprovals[catId] = to;
        emit Approval(owner, to, catId);
    }

    /**
     * @dev Gets the approved address for a cat ID, or zero if no address set
     * Reverts if the cat does not exist.
     * @param catId uint256 ID of the cat to query the approval of
     * @return address currently approved for the given cat ID
     */
    function getApproved(uint256 catId) public view returns (address) {
        require(_exists(catId));
        return _kittenApprovals[catId];
    }

    /**
     * @dev Transfers the ownership of a given cat ID to another address
     * Usage of this method is discouraged, use `safeTransferFrom` whenever possible
     * Requires the msg.sender to be the owner, approved, or operator
     * @param from current owner of the cat
     * @param to address to receive the ownership of the given cat ID
     * @param catId uint256 ID of the cat to be transferred
     */
    function transferFrom(address from, address to, uint256 catId) public {
        address owner = ownerOf(catId);
        require(msg.sender == owner || getApproved(catId) == msg.sender);

        if (_kittenApprovals[catId] != address(0)) {
            _kittenApprovals[catId] = address(0);
        }

        _ownedCatsCount[from] -= 1;
        _ownedCatsCount[to] += 1;

        _kittenOwners[catId] = to;

        emit Transfer(from, to, catId);
    }

    /**
     * @dev Function to mint a new cat
     * Reverts if the given cat ID already exists
     * @param to The address that will own the minted cat
     * @param catId uint256 ID of the cat to be minted
     */
    function mint(address to, uint256 catId) public {
        require(msg.sender == petStore);
        require(to != address(0));
        require(!_exists(catId));

        _kittenOwners[catId] = to;
        _ownedCatsCount[to] += 1;

        emit Transfer(address(0), to, catId);
    }

    /**
     * @dev Returns whether the specified cat exists
     * @param catId uint256 ID of the cat to query the existence of
     * @return bool whether the cat exists
     */
    function _exists(uint256 catId) internal view returns (bool) {
        address owner = _kittenOwners[catId];
        return owner != address(0);
    }
}

contract PetShop {
    address payable owner;
    uint256 catIDS;
    KittenRegistry registry;
    
    event NewCat(address owner, uint256 catId);
    
    constructor() public {
        owner = msg.sender;
    }
    
    function setKittenRegistry(address _registry) public {
        require(msg.sender == owner);
        registry = KittenRegistry(_registry);
    }

    function withdraw() public {
        owner.transfer(address(this).balance);
    }
    
    /**
     * @dev Sells a cat to users
     * @return uint256 the new catId
     */
    function buyAKitten() public payable returns (uint256) {
        require(msg.value == 0.5 ether, "You forgot to pay 0.5 ether for the cat.");
        uint256 catId = catIDS++;
        registry.mint(msg.sender, catId);
        emit NewCat(owner, catId);
        return catId;
    }
}
