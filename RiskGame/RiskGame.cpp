#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <thread>
#include <chrono>
#include <random>
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::to_string;

// Forward declarations
class Player;
class Country;
class Continent;

class Player
{
public:
	string Name;
	vector<Country *> OwnedCountries;
	int TotalArmies;

	Player(const std::string &playerName) : Name(playerName), TotalArmies(0) {}

	void addCountry(Country *country);
	void removeCountry(Country *country);
	void addArmies(int numArmies);
	void deployArmies(Country *country, int numArmies);

	vector<Country *> getCountries()
	{
		return OwnedCountries;
	}
};

class Country
{
public:
	string Name;
	Player *Owner;
	int ArmyCount;
	std::vector<Country *> Neighbours;

	Country(const std::string &countryName) : Name(countryName), Owner(nullptr), ArmyCount(1) {}

	void SetOwner(Player *player);
	void AddArmy(int armiesAdded);
	void RemoveArmy(int armiesRemoved);
	void AddNeighbour(Country *neighbour);
	bool areNeighbours(Country *other);
};
void Player::addCountry(Country *country)
{
	OwnedCountries.push_back(country);
}
void Player::removeCountry(Country *country)
{
	OwnedCountries.erase(std::remove(OwnedCountries.begin(), OwnedCountries.end(), country), OwnedCountries.end());
}
void Player::addArmies(int numArmies)
{
	TotalArmies += numArmies;
}
void Player::deployArmies(Country *country, int numArmies)
{
	if (numArmies <= TotalArmies && std::find(OwnedCountries.begin(), OwnedCountries.end(), country) != OwnedCountries.end())
	{
		country->AddArmy(numArmies);
		TotalArmies -= numArmies;
	}
	else
	{
		std::cout << "Cannot deploy armies. Either not enough armies available or country not owned by player." << std::endl;
	}
}
void Country::SetOwner(Player *player)
{
	Owner = player;
	// player->addCountry(this);//This shouldnt be here
}

void Country::AddArmy(int armiesAdded)
{
	ArmyCount += armiesAdded;
}

void Country::RemoveArmy(int armiesRemoved)
{
	ArmyCount -= armiesRemoved;
	if (ArmyCount < 0)
	{
		ArmyCount = 0;
	}
}

void Country::AddNeighbour(Country *neighbour)
{
	Neighbours.push_back(neighbour);
}

bool Country::areNeighbours(Country *other)
{
	return std::find(Neighbours.begin(), Neighbours.end(), other) != Neighbours.end();
}
class Continent
{
public:
	string Name;
	int ControlValue; // etermining the number of armies given to a player each turn if they can control the entire continent.
	vector<Country *> Countries;

	Continent(const string &continentName, int controlValue) : Name(continentName), ControlValue(controlValue) {}
	/*
	Tells the continent that it has a new country in it.
	*/
	void AddCountry(Country *_country)
	{
		Countries.push_back(_country);
	}
	/*
	Returns the control value of the continent if the player owns all the countries in the continent.
	Otherwise, returns 0.
	*/
	int IsPlayerInControl(Player *player)
	{
		for (const auto &country : Countries)
		{
			if (country->Owner != player)
			{
				return 0;
			}
		}
		return ControlValue;
	}
};
class Map
{
private:
	void addCountry(Country *_country)
	{
		countries.push_back(_country);
	}

public:
	vector<Country *> countries;
	vector<Continent *> continents;

	void AddContinent(Continent *continent)
	{
		continents.push_back(continent);
		// add all countries in the continent to the map
		for (const auto &country : continent->Countries)
		{
			addCountry(country);
		}
	}
	Country *findCountryByName(const std::string &name)
	{
		for (const auto &country : countries)
		{
			if (country->Name == name)
			{
				return country;
			}
		}
		return nullptr;
	}
	Continent *findContinentByName(const std::string &name)
	{
		for (const auto &continent : continents)
		{
			if (continent->Name == name)
			{
				return continent;
			}
		}
		return nullptr;
	}

	int GetReinforcementsAmount(Player *_currentPlayer)
	{
		int reinforcments = 0;

		reinforcments += _currentPlayer->OwnedCountries.size() / 3;

		for (const auto &continent : continents)
		{
			reinforcments += continent->IsPlayerInControl(_currentPlayer);
		}
		_currentPlayer->addArmies(reinforcments);

		return reinforcments;
	}
	// rolls the dice the amount of times specified and returns the highest roll
	int rollDice(int _rolls)
	{
		int highestRoll = 0;
		for (int i = 0; i < _rolls; ++i)
		{
			int roll = rand() % 6 + 1;
			if (roll > highestRoll)
			{
				highestRoll = roll;
			}
		}
		return highestRoll;
	}

	void AttackCountry(Player *currentPlayer, Country *attackingCountry, Country *defendingCountry)
	{
		cout << "Print" << endl;
		if (!attackingCountry->areNeighbours(defendingCountry))
		{
			std::cout << attackingCountry << " can not attack" << defendingCountry << ".\n";
			return;
		}
		// repeat until attacker or defender has no armies left
		int _diceRolls = 0;

		do
		{
			int attackerRole = rollDice(3);
			int defenderRole = rollDice(2);
			++_diceRolls;

			if (attackerRole > defenderRole)
			{
				defendingCountry->RemoveArmy(1);
				cout << "Defender lost the battle" << endl;
			}
			else
			{
				attackingCountry->RemoveArmy(1);
				cout << "Attacker lost the battle" << endl;
			}
			cout << "Attacking country: " << attackingCountry->Name << " has " << attackingCountry->ArmyCount << " armies left." << endl;
			cout << "Defending country: " << defendingCountry->Name << " has " << defendingCountry->ArmyCount << " armies left." << endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			if (defendingCountry->ArmyCount == 0)
			{
				defendingCountry->SetOwner(currentPlayer);
				// deploy armies equal to the number of dice rolled
				defendingCountry->AddArmy(1);
				cout << "Defender lost " << defendingCountry->Name << endl;
				cout << "Attacker won " << defendingCountry->Name << endl;
				break;
			}
		} while (attackingCountry->ArmyCount > 0 && defendingCountry->ArmyCount > 0);
	}

	void fortificationPhase(Player *currentPayer, Country *fromCountry, Country *toCountry, int armiesToMove)
	{

		if (fromCountry->Owner == currentPayer && toCountry->Owner == currentPayer && fromCountry->areNeighbours(toCountry))
		{
			if (fromCountry->ArmyCount > armiesToMove)
			{
				fromCountry->RemoveArmy(armiesToMove);
				toCountry->AddArmy(armiesToMove);
			}
			else
			{
				std::cout << "Not enough armies present in selected country to fortify\n";
			}
		}
		else
		{
			std::cout << "Invalid fortify move, double check ownership of both countries are verify that they are neighbours\n";
		}
	}
};

class Game
{

private:
	Map *map;
	vector<Player *> players;
	int currentPlayerIndex;
	int reinforcementCardValue;
	int numberOfPlayers;

public:
	Game(int numPlayers = 2) : currentPlayerIndex(0), reinforcementCardValue(5), numberOfPlayers(numPlayers)
	{
		map = new Map();

		for (int i = 0; i < numberOfPlayers; ++i)
		{
			players.push_back(new Player("Player " + std::to_string(i + 1)));
		}
	}

	~Game()
	{
		for (auto &player : players)
		{
			delete player;
		}
		delete map;
	}

	void printOwnedCountries(Player *player)
	{
		std::cout << player->Name << " owns: ";
		for (auto country : player->OwnedCountries)
		{
			std::cout << country->Name << ", ";
		}
		std::cout << std::endl;
	}
	void assignCountries()
	{
		srand(static_cast<unsigned int>(time(0)));
		std::shuffle(map->countries.begin(), map->countries.end(), std::default_random_engine());

		for (size_t i = 0; i < map->countries.size(); ++i)
		{
			players[i % players.size()]->addCountry(map->countries[i]);
			map->countries[i]->SetOwner(players[i % players.size()]);
		}
		for (auto player : players)
		{
			printOwnedCountries(player);
		}
	}

	void attackPhase(Player *currentPlayer)
	{
		cout << currentPlayer->Name << ", it's your attack phase." << std::endl;
		// ask player if they want to attack
		string choice;
		cout << "Do you want to attack? (yes/no): ";
		getline(cin, choice);
		if (choice != "yes")
		{
			return;
		}

		// print countries with more than 1 army
		// store them in an vector and then allow player to select from them
		cout << "Countries with more than 1 army: ";
		vector<Country *> _countriesWithArmies;
		for (auto country : currentPlayer->OwnedCountries)
		{
			if (country->ArmyCount > 1)
			{
				cout << country->Name << " | ";
				_countriesWithArmies.push_back(country);
			}
		}
		cout << endl;
		if (_countriesWithArmies.size() == 0)
		{
			std::cout << "No countries with more than 1 army to attack from." << std::endl;
			return;
		}
		Country *attackingCountry;
		while (true)
		{
			cout << "Select a country to attack from: ";
			string countryName;
			getline(cin, countryName);
			for (auto country : _countriesWithArmies)
			{
				if (country->Name == countryName)
				{
					attackingCountry = country;
					cout << "Selected country: " << attackingCountry->Name << endl;
					break;
				}
			}
			if (attackingCountry != nullptr)
			{
				break;
			}
			cout << "Invalid selection: country does not have enough armies or not owned." << endl;
		}
		vector<Country *> _neighbourCountries;
		// get the neighbour countries of the attacking country
		cout << "Enemy neighbours of " << attackingCountry->Name << " are: ";
		for (auto neighbour : attackingCountry->Neighbours)
		{
			// check if its not owned by the player
			if (neighbour->Owner != currentPlayer)
				continue;
			cout << neighbour->Name << " | ";
			_neighbourCountries.push_back(neighbour);
		}
		cout << endl;
		if (_neighbourCountries.size() == 0)
		{
			std::cout << "No neighbours to attack." << std::endl;
			return;
		}
		Country *defendingCountry;
		while (true)
		{
			cout << "Select a country to attack: ";
			string countryName;
			getline(cin, countryName);
			for (auto country : _neighbourCountries)
			{
				if (country->Name == countryName)
				{
					defendingCountry = country;
					break;
				}
			}
			if (defendingCountry != nullptr)
			{
				break;
			}
			cout << "Invalid selection: country does not have enough armies or not owned." << endl;
		}

		map->AttackCountry(currentPlayer, attackingCountry, defendingCountry);
	}
	void fortificationPhase(Player *currentPlayer)
	{
		string input;
		cout << currentPlayer->Name << ", it's your fortification phase, do you wish to fortify(yes/no)" << std::endl;
		getline(cin, input);
		if (input != "yes")
			return;

		cout << "Countries you can reinforce from ";
		for (auto country : currentPlayer->OwnedCountries)
		{
			if (country->ArmyCount > 1)
			{
				cout << country->Name << " | ";
			}
		}
		cout << endl;
		Country *fromCountry = selectCountry(currentPlayer, "Which country do you want to move armies from: ");
		if (fromCountry == nullptr)
		{
			cout << "Fortification phase ended.\n";
			return;
		}

		//get neighbours from the selected country that are owned by player
		vector<Country *> _neighbourCountries;
		cout << "Neighbours owned by you: ";
		for(auto neighbour : fromCountry->Neighbours)
		{
			if(neighbour->Owner == currentPlayer)
			{
				_neighbourCountries.push_back(neighbour);
				cout << neighbour->Name << " | ";
			}
		}
		cout << endl;
		if (_neighbourCountries.size() == 0)
		{
			cout << "No neighbours to fortify." << endl;
			return;
		}
		// select a country to fortify
		Country *toCountry;
		cout << "Select a country to fortify: ";
		while(true)
		{			
			string countryName;
			getline(cin, countryName);
			for(auto country : _neighbourCountries)
			{
				if(countryName == country->Name)
				{
					toCountry = country;
					break;
				}
			}
			if(toCountry != nullptr)
			{
				break;
			}
			cout << "Invalid selection: country does not have enough armies or not owned." << endl;
		}

		int armiesToMove = selectArmyCount(fromCountry);
		if (armiesToMove == 0)
		{
			return;
		}

		map->fortificationPhase(currentPlayer, fromCountry, toCountry, armiesToMove);
		cout << "Moved " << armiesToMove << " from " << fromCountry->Name << " to " << toCountry->Name << endl;
	}

	Country *selectCountry(Player *player, const std::string &prompt, Country *neighborOf = nullptr)
	{
		while (true)
		{
			cout << prompt;
			string countryName;
			getline(cin, countryName); // Get country name input

			Country *selectedCountry = map->findCountryByName(countryName);
			if (selectedCountry != nullptr && selectedCountry->Owner == player)
			{
				if (neighborOf == nullptr || selectedCountry->areNeighbours(neighborOf))
				{
					return selectedCountry;
				}
				else
				{
					cout << "Invalid selection: countries are not neighbors." << std::endl;
					// print the neighbours of the selected country
					cout << "Neighbours of " << selectedCountry->Name << " are: ";
					for (auto neighbour : selectedCountry->Neighbours)
					{
						cout << neighbour->Name << ", ";
					}
				}
			}
			else
			{
				cout << "\nInvalid selection: country not found or not owned by you." << std::endl;
				// print the countries owned by the player
				printOwnedCountries(player);
			}
		}
	}

	int selectArmyCount(Country *country)
	{
		while (true)
		{
			if (country->ArmyCount == 1)
			{
				std::cout << "No armies to move." << std::endl;
				return 0;
			}
			std::cout << "Enter the number of armies to move (max " << country->ArmyCount - 1 << "): ";
			int armiesToMove;
			std::cin >> armiesToMove;
			std::cin.ignore();

			if (armiesToMove >= 1 && armiesToMove <= country->ArmyCount - 1)
			{
				return armiesToMove;
			}
			else
			{
				std::cout << "Invalid number of armies. Please try again." << std::endl;
			}
		}
	}
	void deployReinforcements(Player *player, int numArmies)
	{
		int amountDeployed = 0;
		while (numArmies > 0)
		{
			cout << "How many armies to deploy: ";
			string temp;
			getline(cin, temp);
			amountDeployed = stoi(temp);
			while (amountDeployed > numArmies)
			{
				cout << "Number to deploy is greater then available armies" << endl;
				cout << "How many armies to deploy: ";
				getline(cin, temp);
				temp = to_string(amountDeployed);
				amountDeployed = stoi(temp);
			}
			while (amountDeployed <= 0)
			{
				cout << "Number to deploy must be greater than 0" << endl;
				cout << "How many armies to deploy: ";
				
				getline(cin, temp);
				temp = to_string(amountDeployed);
				amountDeployed = stoi(temp);
			}
			Country *country = selectCountry(player, "Select the country you wish to deploy them to: ");
			if (country == nullptr)
				break;
			while (amountDeployed > 0)
			{
				player->deployArmies(country, 1);
				amountDeployed--;
				numArmies--;
			}
			cout << numArmies << " remaining" << endl;
		}
	}
	void setCountries()
	{
		// NA
		Continent *_northAmerica = new Continent("North America", 5);

		Country *_alaska = new Country("Alaska");
		Country *_alberta = new Country("Alberta");
		Country *_centralAmerica = new Country("Central America");
		Country *_easternUnitedStates = new Country("Eastern United States");
		Country *_greenland = new Country("Greenland");
		Country *_northwestTerritory = new Country("Northwest Territory");
		Country *_ontario = new Country("Ontario");
		Country *_quebec = new Country("Quebec");
		Country *_westernUnitedStates = new Country("Western United States");

		_northAmerica->AddCountry(_alaska);
		_northAmerica->AddCountry(_alberta);
		_northAmerica->AddCountry(_centralAmerica);
		_northAmerica->AddCountry(_easternUnitedStates);
		_northAmerica->AddCountry(_greenland);
		_northAmerica->AddCountry(_northwestTerritory);
		_northAmerica->AddCountry(_ontario);
		_northAmerica->AddCountry(_quebec);
		_northAmerica->AddCountry(_westernUnitedStates);

		map->AddContinent(_northAmerica);

		_alaska->AddNeighbour(_alberta);
		_alaska->AddNeighbour(_northwestTerritory);
		_alberta->AddNeighbour(_alaska);
		_alberta->AddNeighbour(_northwestTerritory);
		_alberta->AddNeighbour(_ontario);
		_alberta->AddNeighbour(_westernUnitedStates);
		_centralAmerica->AddNeighbour(_easternUnitedStates);
		_centralAmerica->AddNeighbour(_westernUnitedStates);
		_easternUnitedStates->AddNeighbour(_centralAmerica);
		_easternUnitedStates->AddNeighbour(_ontario);
		_easternUnitedStates->AddNeighbour(_quebec);
		_easternUnitedStates->AddNeighbour(_westernUnitedStates);
		_greenland->AddNeighbour(_northwestTerritory);
		_greenland->AddNeighbour(_ontario);
		_greenland->AddNeighbour(_quebec);
		_northwestTerritory->AddNeighbour(_alaska);
		_northwestTerritory->AddNeighbour(_alberta);
		_northwestTerritory->AddNeighbour(_greenland);
		_northwestTerritory->AddNeighbour(_ontario);
		_ontario->AddNeighbour(_alberta);
		_ontario->AddNeighbour(_easternUnitedStates);
		_ontario->AddNeighbour(_greenland);
		_ontario->AddNeighbour(_northwestTerritory);
		_ontario->AddNeighbour(_quebec);
		_ontario->AddNeighbour(_westernUnitedStates);
		_quebec->AddNeighbour(_easternUnitedStates);
		_quebec->AddNeighbour(_greenland);
		_quebec->AddNeighbour(_ontario);
		_westernUnitedStates->AddNeighbour(_alberta);
		_westernUnitedStates->AddNeighbour(_centralAmerica);
		_westernUnitedStates->AddNeighbour(_easternUnitedStates);
		_westernUnitedStates->AddNeighbour(_ontario);

		// South America
		Continent *_southAmerica = new Continent("South America", 2);

		Country *_argentina = new Country("Argentina");
		Country *_brazil = new Country("Brazil");
		Country *peru = new Country("Peru");
		Country *venezuela = new Country("Venezuela");

		_southAmerica->AddCountry(_argentina);
		_southAmerica->AddCountry(_brazil);
		_southAmerica->AddCountry(peru);
		_southAmerica->AddCountry(venezuela);

		map->AddContinent(_southAmerica);

		_argentina->AddNeighbour(_brazil);
		_argentina->AddNeighbour(peru);
		_brazil->AddNeighbour(_argentina);
		_brazil->AddNeighbour(peru);
		_brazil->AddNeighbour(venezuela);
		peru->AddNeighbour(_argentina);
		peru->AddNeighbour(_brazil);
		peru->AddNeighbour(venezuela);
		venezuela->AddNeighbour(_brazil);
		venezuela->AddNeighbour(peru);

		// Europe
		Continent *_europe = new Continent("Europe", 5);

		Country *_greatBritain = new Country("Great Britain");
		Country *_iceland = new Country("Iceland");
		Country *_northernEurope = new Country("Northern Europe");
		Country *_scandinavia = new Country("Scandinavia");
		Country *_southernEurope = new Country("Southern Europe");
		Country *_ukraine = new Country("Ukraine");
		Country *_westernEurope = new Country("Western Europe");

		_europe->AddCountry(_greatBritain);
		_europe->AddCountry(_iceland);
		_europe->AddCountry(_northernEurope);
		_europe->AddCountry(_scandinavia);
		_europe->AddCountry(_southernEurope);
		_europe->AddCountry(_ukraine);
		_europe->AddCountry(_westernEurope);

		map->AddContinent(_europe);

		_greatBritain->AddNeighbour(_iceland);
		_greatBritain->AddNeighbour(_northernEurope);
		_greatBritain->AddNeighbour(_scandinavia);
		_greatBritain->AddNeighbour(_westernEurope);
		_iceland->AddNeighbour(_greatBritain);
		_iceland->AddNeighbour(_scandinavia);
		_northernEurope->AddNeighbour(_greatBritain);
		_northernEurope->AddNeighbour(_scandinavia);
		_northernEurope->AddNeighbour(_southernEurope);
		_northernEurope->AddNeighbour(_ukraine);
		_northernEurope->AddNeighbour(_westernEurope);
		_scandinavia->AddNeighbour(_greatBritain);
		_scandinavia->AddNeighbour(_iceland);
		_scandinavia->AddNeighbour(_northernEurope);
		_scandinavia->AddNeighbour(_ukraine);
		_southernEurope->AddNeighbour(_northernEurope);
		_southernEurope->AddNeighbour(_ukraine);
		_southernEurope->AddNeighbour(_westernEurope);
		_ukraine->AddNeighbour(_northernEurope);
		_ukraine->AddNeighbour(_scandinavia);
		_ukraine->AddNeighbour(_southernEurope);
		_westernEurope->AddNeighbour(_greatBritain);
		_westernEurope->AddNeighbour(_northernEurope);
		_westernEurope->AddNeighbour(_southernEurope);

		// Africa
		Continent *_africa = new Continent("Africa", 3);

		Country *_congo = new Country("Congo");
		Country *_eastAfrica = new Country("East Africa");
		Country *_egypt = new Country("Egypt");
		Country *_madagascar = new Country("Madagascar");
		Country *_northAfrica = new Country("North Africa");
		Country *_southAfrica = new Country("South Africa");

		_africa->AddCountry(_congo);
		_africa->AddCountry(_eastAfrica);
		_africa->AddCountry(_egypt);
		_africa->AddCountry(_madagascar);
		_africa->AddCountry(_northAfrica);
		_africa->AddCountry(_southAfrica);

		map->AddContinent(_africa);

		_congo->AddNeighbour(_eastAfrica);
		_congo->AddNeighbour(_northAfrica);
		_congo->AddNeighbour(_southAfrica);
		_eastAfrica->AddNeighbour(_congo);
		_eastAfrica->AddNeighbour(_egypt);
		_eastAfrica->AddNeighbour(_madagascar);
		_eastAfrica->AddNeighbour(_northAfrica);
		_eastAfrica->AddNeighbour(_southAfrica);
		_egypt->AddNeighbour(_eastAfrica);
		_egypt->AddNeighbour(_northAfrica);
		_madagascar->AddNeighbour(_eastAfrica);
		_madagascar->AddNeighbour(_southAfrica);
		_northAfrica->AddNeighbour(_congo);
		_northAfrica->AddNeighbour(_eastAfrica);
		_northAfrica->AddNeighbour(_egypt);
		_southAfrica->AddNeighbour(_congo);
		_southAfrica->AddNeighbour(_eastAfrica);
		_southAfrica->AddNeighbour(_madagascar);

		// Asia
		Continent *_asia = new Continent("Asia", 7);

		Country *_afghanistan = new Country("Afghanistan");
		Country *_china = new Country("China");
		Country *_india = new Country("India");
		Country *_irkutsk = new Country("Irkutsk");
		Country *_japan = new Country("Japan");
		Country *_kamchatka = new Country("Kamchatka");
		Country *_middleEast = new Country("Middle East");
		Country *_mongolia = new Country("Mongolia");
		Country *_siam = new Country("Siam");
		Country *_siberia = new Country("Siberia");
		Country *_ural = new Country("Ural");
		Country *_yakutsk = new Country("Yakutsk");

		_asia->AddCountry(_afghanistan);
		_asia->AddCountry(_china);
		_asia->AddCountry(_india);
		_asia->AddCountry(_irkutsk);
		_asia->AddCountry(_japan);
		_asia->AddCountry(_kamchatka);
		_asia->AddCountry(_middleEast);
		_asia->AddCountry(_mongolia);
		_asia->AddCountry(_siam);
		_asia->AddCountry(_siberia);
		_asia->AddCountry(_ural);
		_asia->AddCountry(_yakutsk);

		map->AddContinent(_asia);

		_afghanistan->AddNeighbour(_china);
		_afghanistan->AddNeighbour(_india);
		_afghanistan->AddNeighbour(_middleEast);
		_afghanistan->AddNeighbour(_ural);
		_china->AddNeighbour(_afghanistan);
		_china->AddNeighbour(_india);
		_china->AddNeighbour(_mongolia);
		_china->AddNeighbour(_siam);
		_china->AddNeighbour(_siberia);
		_china->AddNeighbour(_ural);
		_india->AddNeighbour(_afghanistan);
		_india->AddNeighbour(_china);
		_india->AddNeighbour(_middleEast);
		_india->AddNeighbour(_siam);
		_irkutsk->AddNeighbour(_kamchatka);
		_irkutsk->AddNeighbour(_mongolia);
		_irkutsk->AddNeighbour(_siberia);
		_irkutsk->AddNeighbour(_yakutsk);
		_japan->AddNeighbour(_kamchatka);
		_japan->AddNeighbour(_mongolia);
		_kamchatka->AddNeighbour(_irkutsk);
		_kamchatka->AddNeighbour(_japan);
		_kamchatka->AddNeighbour(_mongolia);
		_kamchatka->AddNeighbour(_yakutsk);
		_middleEast->AddNeighbour(_afghanistan);
		_middleEast->AddNeighbour(_india);
		_middleEast->AddNeighbour(_egypt);		// Connecting to Africa
		_middleEast->AddNeighbour(_eastAfrica); // Connecting to Africa
		_mongolia->AddNeighbour(_china);
		_mongolia->AddNeighbour(_irkutsk);
		_mongolia->AddNeighbour(_japan);
		_mongolia->AddNeighbour(_kamchatka);
		_mongolia->AddNeighbour(_siberia);
		_siam->AddNeighbour(_china);
		_siam->AddNeighbour(_india);
		_siberia->AddNeighbour(_china);
		_siberia->AddNeighbour(_irkutsk);
		_siberia->AddNeighbour(_mongolia);
		_siberia->AddNeighbour(_ural);
		_siberia->AddNeighbour(_yakutsk);
		_ural->AddNeighbour(_afghanistan);
		_ural->AddNeighbour(_china);
		_ural->AddNeighbour(_siberia);
		_yakutsk->AddNeighbour(_irkutsk);
		_yakutsk->AddNeighbour(_kamchatka);
		_yakutsk->AddNeighbour(_siberia);

		// Australia
		Continent *_australia = new Continent("Australia", 2);

		Country *_easternAustralia = new Country("Eastern Australia");
		Country *_indonesia = new Country("Indonesia");
		Country *_newGuinea = new Country("New Guinea");
		Country *_westernAustralia = new Country("Western Australia");

		_australia->AddCountry(_easternAustralia);
		_australia->AddCountry(_indonesia);
		_australia->AddCountry(_newGuinea);
		_australia->AddCountry(_westernAustralia);

		map->AddContinent(_australia);

		_easternAustralia->AddNeighbour(_newGuinea);
		_easternAustralia->AddNeighbour(_westernAustralia);
		_indonesia->AddNeighbour(_newGuinea);
		_indonesia->AddNeighbour(_westernAustralia);
		_indonesia->AddNeighbour(_siam); // Connecting to Asia
		_newGuinea->AddNeighbour(_easternAustralia);
		_newGuinea->AddNeighbour(_indonesia);
		_newGuinea->AddNeighbour(_westernAustralia);
		_westernAustralia->AddNeighbour(_easternAustralia);
		_westernAustralia->AddNeighbour(_indonesia);
		_westernAustralia->AddNeighbour(_newGuinea);

		// connections to other continents
		_alaska->AddNeighbour(_kamchatka);
		_kamchatka->AddNeighbour(_alaska);

		_northAfrica->AddNeighbour(_brazil);
		_northAfrica->AddNeighbour(_westernEurope);
		_northAfrica->AddNeighbour(_southernEurope);
		_brazil->AddNeighbour(_northAfrica);
		_westernEurope->AddNeighbour(_northAfrica);
		_southernEurope->AddNeighbour(_northAfrica);

		_egypt->AddNeighbour(_southernEurope);
		_southernEurope->AddNeighbour(_egypt);
	}

	void StartGame()
	{
		setCountries();

		assignCountries();

		while (!checkGameEnd())
		{
			currentPlayerIndex = currentPlayerIndex % players.size();
			Player *currentPlayer = players[currentPlayerIndex];

			int reinforcements = map->GetReinforcementsAmount(currentPlayer);
			std::cout << currentPlayer->Name << " receives " << reinforcements << " reinforcements." << std::endl;

			deployReinforcements(currentPlayer, reinforcements);

			attackPhase(currentPlayer);

			fortificationPhase(currentPlayer);

			std::cout << "__________________________________________________" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));

			++currentPlayerIndex;
		}

		Player *winner = players[currentPlayerIndex]; // Last standing player wins
		std::cout << "Game Over. " << winner->Name << " wins!" << std::endl;
	}

	bool checkGameEnd()
	{
		int playersWithCountries = 0;
		for (Player *player : players)
		{
			if (player->OwnedCountries.size() > 0)
			{
				++playersWithCountries;
			}
		}
		return playersWithCountries == 1; // True if only one player has territories
	}
};

int main()
{

	Game _riskGame;
	_riskGame.StartGame();
	return 0;
}