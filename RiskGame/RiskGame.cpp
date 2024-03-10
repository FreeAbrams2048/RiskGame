#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// Forward declarations
class Player;
class Country;

class Player {
public:
	std::string name;
	std::vector<Country*> ownedCountries;
	int totalArmies;

	Player(const std::string& playerName) : name(playerName), totalArmies(0) {}

	void addCountry(Country* country);
	void removeCountry(Country* country);
	void addArmies(int numArmies);
	void deployArmies(Country* country, int numArmies);
};

class Country {
public:
	std::string name;
	Player* owner;
	int armyCount;
	std::vector<Country*> neighbours;

	Country(const std::string& countryName) : name(countryName), owner(nullptr), armyCount(0) {}

	void SetOwner(Player* player);
	void addArmy(int armiesAdded);
	void removeArmy(int armiesRemoved);
	void addNeighbour(Country* neighbour);
	bool areNeighbours(Country* other);
};
void Player::addCountry(Country* country) {
	ownedCountries.push_back(country);
}
void Player::removeCountry(Country* country) {
	ownedCountries.erase(std::remove(ownedCountries.begin(), ownedCountries.end(), country), ownedCountries.end());
}
void Player::addArmies(int numArmies) {
	totalArmies += numArmies;
}
void Player::deployArmies(Country* country, int numArmies) {
	if (numArmies <= totalArmies && std::find(ownedCountries.begin(), ownedCountries.end(), country) != ownedCountries.end()) {
		country->addArmy(numArmies);
		totalArmies -= numArmies;
	}
	else {
		std::cout << "Cannot deploy armies. Either not enough armies available or country not owned by player." << std::endl;
	}
}
void Country::SetOwner(Player* player) {
	owner = player;
	player->addCountry(this);
}

void Country::addArmy(int armiesAdded) {
	armyCount += armiesAdded;
}

void Country::removeArmy(int armiesRemoved) {
	armyCount -= armiesRemoved;
	if (armyCount < 0) {
		armyCount = 0;
	}
}

void Country::addNeighbour(Country* neighbour) {
	neighbours.push_back(neighbour);
}

bool Country::areNeighbours(Country* other) {
	return std::find(neighbours.begin(), neighbours.end(), other) != neighbours.end();
}
class Continent {
public:
	std::string name;
	int bonusArmies;
	std::vector<Country*> countries;

	Continent(const std::string& continentName, int bonus) : name(continentName), bonusArmies(bonus) {}

	void addCountry(Country* country) {
		countries.push_back(country);
	}

	int findBonusArmies(Player* player) {
		for (const auto& country : countries) {
			if (country->owner != player) {
				return 0;
			}
		}
		return bonusArmies;
	}
};
class Map {
public:
	std::vector<Country*> countries;
	std::vector<Continent*> continents;

	void AddCounty(Country* country) {
		countries.push_back(country);
	}
	void addContinent(Continent* continent) {
		continents.push_back(continent);
	}
	Country* findCountryByName(const std::string& name) {
		for (const auto& country : countries) {
			if (country->name == name) {
				return country;
			}
		}
		return nullptr;
	}
	Continent* findContinentByName(const std::string& name) {
		for (const auto& continent : continents) {
			if (continent->name == name) {
				return continent;
			}
		}
		return nullptr;
	}

	int reinforcmentPhase(Player* currentPlayer) {
		int reinforcments = 0;
		
		reinforcments += currentPlayer->ownedCountries.size() / 3;
		if (reinforcments < 3) { reinforcments = 3; }

		for (const auto& continent : continents) {
			reinforcments += continent->findBonusArmies(currentPlayer);
		}

		return reinforcments;
	}

	void attackPhase(Player* currentPlayer, Country* attackingCountry, Country* defendingCountry) {
		if (!attackingCountry->areNeighbours(defendingCountry)) {
			std::cout << attackingCountry << " can not attack" << defendingCountry << ".\n";
			return;
		}
		int attackerRole = rand() % 6 + 1;
		int defenderRole = rand() % 6 + 1;

		if (attackerRole > defenderRole) {
			defendingCountry->removeArmy(1);
		}
		else {
			attackingCountry->removeArmy(1);
		}

		if (defendingCountry->armyCount == 0) {
			defendingCountry->SetOwner(currentPlayer);
			defendingCountry->addArmy(1);
		}
	}

	void fortificationPhase(Player* currentPayer, Country* fromCountry, Country* toCountry, int armiesToMove) {
		if (fromCountry->owner == currentPayer && toCountry->owner == currentPayer && fromCountry->areNeighbours(toCountry)) {
			if (fromCountry->armyCount > armiesToMove) {
				fromCountry->removeArmy(armiesToMove);
				toCountry->addArmy(armiesToMove);
			}
			else {
				std::cout << "Not enough armies present in selected country to fortify\n";
			}
		}
		else {
			std::cout << "Invalid fortify move, double check ownership of both countries are verify that they are neighbours\n";
		}
	}
};

void main() {
	Map riskMap;

	Player* player1 = new Player("Player 1");
	Player* player2 = new Player("Player 2");

	Country* canada = new Country("Canada");
	Country* unitedStates = new Country("United States");
	Country* mexico = new Country("Mexico");

	canada->SetOwner(player1);
	mexico->SetOwner(player2);

	player1->addArmies(5);
	player2->addArmies(6);

	canada->addNeighbour(unitedStates);
	mexico->addNeighbour(unitedStates);
	unitedStates->addNeighbour(canada);
	unitedStates->addNeighbour(mexico);

	Continent* northAmerica = new Continent("North America", 5);
	northAmerica->addCountry(canada);
	northAmerica->addCountry(unitedStates);
	northAmerica->addCountry(mexico);

	riskMap.addContinent(northAmerica);

	int reinforcment = riskMap.reinforcmentPhase(player1);
	std::cout << reinforcment << std::endl;
	player1->addArmies(reinforcment);
	std::cout << player1->totalArmies<<std::endl;
}