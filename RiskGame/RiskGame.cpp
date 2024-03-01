#include <iostream>
#include <vector>
#include <string>

class Country;

class Player {
public:
    std::string name;
    std::vector<Country*> ownedCountries;
    int totalArmies;

    Player(const std::string& playerName) : name(playerName), totalArmies(0) {}

    void addCountry(Country* country) {
        ownedCountries.push_back(country);
    }

    void removeCountry(Country* country) {
        ownedCountries.erase(std::remove(ownedCountries.begin(), ownedCountries.end(), country), ownedCountries.end());
    }

    void addArmies(int numArmies) {
        totalArmies += numArmies;
    }

    void deployArmies(Country* country, int numArmies) {
        if (numArmies <= totalArmies && std::find(ownedCountries.begin(), ownedCountries.end(), country) != ownedCountries.end()) {
            country->addArmy(numArmies);
            totalArmies -= numArmies;
        } else {
            std::cout << "Cannot deploy armies. Either not enough armies available or country not owned by player." << std::endl;
        }
    }
};


class Country {
public:
	std::string name;
	Player* owner;
	int armyCount;
	std::vector<Country*> neighbours;

	Country(const std::string& countryName) : name(countryName), owner(nullptr), armyCount(0) {}

	void SetOwner(Player* player) {
		owner = player;
		player->addCountry(this);
	}

	void addArmy(int armiesAdded) {
		armyCount += armiesAdded;
	}
	void removeArmy(int armiesRemoved) {
		armyCount -= armiesRemoved;
		if (armyCount < 0) {
			armyCount = 0;
		}
	}
	void addNeighbour(Country* neighbour) {
		neighbours.push_back(neighbour);
	}
	bool areNeaighbours(Country* other) {
		for (const auto& neighbor : neighbours) {
			if (neighbor == other) {
				return true;
			}
		}
		return false;
	}
};
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
};