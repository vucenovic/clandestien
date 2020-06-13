#pragma once

struct Items {
	bool key;
	bool filmreel;
};

class Inventory
{
	public:
		static Inventory& instance() {
			static Inventory _instance;
			return _instance;
		}

		void addFilmreel() { i.filmreel = true; };
		void removeFilmreel() { i.filmreel = false; };
		void addKey() { i.key = true; };
		void removeKey() { i.key = false; };
		Items getInventory() { return i; };

	private:
		Items i = { false };
		Inventory() = default;
		Inventory(const Inventory&);
		Inventory & operator = (const Inventory &) = delete ;

};

