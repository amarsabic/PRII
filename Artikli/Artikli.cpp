#include<iostream>

using namespace std;

char const* crt = "\n----------------------------------------------------\n";

struct Datum {
	int * _dan, *_mjesec, *_godina;

	void Unos(int d, int m, int g) {
		_dan = new int(d);
		_mjesec = new int(m);
		_godina = new int(g);
	}
	void Ispis() { cout << *_dan << "/" << *_mjesec << "/" << *_godina << endl; }
	void Dealociraj() {
		delete _dan; delete _mjesec; delete _godina;
		_dan = _mjesec = _godina = nullptr;
	}
};

struct Artikal {
	char * _naziv;
	float _cijena;
	void Unos(char const* naziv, float cijena) {
		int vel = strlen(naziv) + 1;
		_naziv = new char[vel];
		strcpy_s(_naziv, vel, naziv);
		_cijena = cijena;
	}
	void Dealociraj() { delete[]_naziv; _naziv = nullptr; }
	void Ispis() { cout << _naziv << " " << _cijena << " KM "; }
};

struct Racun {
	//format broja racuna: 006005491
	//osigurati automatsko generisanje broja racuna pocevsi od 000000001
	char _brojRacuna[10];
	Datum _datumKreiranja;
	Artikal * _artikli;
	int * _kolicine;//cuva informaciju o kolicini svakog artikla na racunu
	int _brojArtikala;
	//Potpisi funkcija trebaju odgovarati onima u main-u
	//1. Unos
	//2. Dealociraj
	//3. DodajArtikal - dodaje novi artikal u listu artikala zajedno sa njegovom kolicinom. 
	//Onemoguciti ponavljanje artikala na nacin da se uvecava samo kolicina ukoliko
	//	korisnik vise puta pokusa dodati isti artikal.
	//	//4. Ispis - ispisuje racun u formatu prikazanom na slici (nije obavezno da bude 
	//	identican, ali je svakako pozeljno).Prilikom svakog ispisa, racun
	//	je potrebno spasiti u tekstualni fajl sa istim nazivom kao i broj racuna.
	//	//5. BinarnaPretraga - koristeci binarnu pretragu, na osnovu naziva, pronalazi i vraca 
	//	pokazivac na artikal.Ukoliko trazeni artikal ne postoji funkcija vraca nullptr.


	void Unos(char const * racun, Datum datum) {
		strcpy_s(_brojRacuna, 10, racun);
		_datumKreiranja.Unos(*datum._dan, *datum._mjesec, *datum._godina);
		_artikli = nullptr;
		_kolicine = nullptr;
		_brojArtikala = 0;
	}

	void Dealociraj() {
		_datumKreiranja.Dealociraj();
		for (int i = 0; i < _brojArtikala; i++)
		{
			_artikli[i].Dealociraj();
		}
		delete[] _artikli; _artikli = nullptr;
		delete[] _kolicine; _kolicine = nullptr;
	}

	void DodajArtikal(Artikal artikal, int kolicina) {

		for (int i = 0; i < _brojArtikala; i++)
		{
			if (strcmp(artikal._naziv, _artikli[i]._naziv) == 0 && artikal._cijena == _artikli[i]._cijena) {
				_kolicine[i] += kolicina;
				return;
			}
		}

		Artikal * temp = new Artikal[_brojArtikala + 1];
		int * tempKolicine = new int[_brojArtikala + 1];
		tempKolicine[_brojArtikala] = kolicina;

		for (int i = 0; i < _brojArtikala; i++)
		{
			temp[i] = _artikli[i];
			tempKolicine[i] = _kolicine[i];
		}
		delete[] _artikli; _artikli = nullptr;
		delete[] _kolicine; _kolicine = nullptr;

		temp[_brojArtikala].Unos(artikal._naziv, artikal._cijena);
		_brojArtikala++;

		_artikli = temp;
		_kolicine = tempKolicine;
	}

	void Ispis() {

		cout << "::RACUN:: " << _brojRacuna << endl;
		cout << "--> ARTIKLI <--" << endl;
		for (int i = 0; i < _brojArtikala; i++)
		{
			_artikli[i].Ispis(); cout << "X " << _kolicine[i] << " =" << _artikli[i]._cijena*_kolicine[i] << endl;
		}
	}

	void Sortiraj() {

		for (int i = 0; i < _brojArtikala; i++)
		{
			for (int j = i + 1; j < _brojArtikala; j++)
			{
				if (strcmp(_artikli[i]._naziv, _artikli[j]._naziv) == 1) {
					swap(_artikli[i], _artikli[j]);
					swap(_kolicine[i], _kolicine[j]);
				}
			}
		}

	}

	Artikal * BinarnaPretraga(char const * naziv) {
		Sortiraj();

		int prvi = 0, srednji = 0, zadnji = _brojArtikala - 1;

		while (zadnji >= prvi) {
			srednji = (prvi + zadnji) / 2;
			if (strcmp(naziv, _artikli[srednji]._naziv) == 0)
				return &_artikli[srednji];
			if (strcmp(naziv, _artikli[srednji]._naziv) == -1)
				zadnji = srednji - 1;
			if (strcmp(naziv, _artikli[srednji]._naziv) == 1)
				prvi = srednji + 1;
		}
		return nullptr;
	}
};


//1. PronadjiNajskupljiArtikal - rekurzivna funkcija koja vraca pokazivac na artikal sa najvecom ukupnom cijenom (cijena*kolicina)
//2. GenerisiSljedeciBrojRacuna - generise i vraca naredni broj racuna

int broj = 1;
char * GenerisiSljedeciBrojRacuna() {

	char * racun;
	racun = new char[10];
	strcpy_s(racun, 10, "000000000");

	int temp = broj;
	int brojacDecimala = 0;

	while (temp > 0) {
		temp /= 10;
		brojacDecimala++;
	}

	_itoa_s(broj, &racun[9 - 1], sizeof(racun), 10);
	broj++;

	return racun;
}

Artikal * PronadjiNajskupljiArtikal(Racun racun, int indeks = 0, float najskuplji = 0, int artikal = 0) {

	if (indeks == racun._brojArtikala) {
		return &racun._artikli[artikal];
	}
	else {
		if ((racun._artikli[indeks]._cijena * racun._kolicine[indeks]) > najskuplji) {
			najskuplji = racun._artikli[indeks]._cijena * racun._kolicine[indeks];
			indeks = artikal;
		}

		return PronadjiNajskupljiArtikal(racun, indeks + 1, najskuplji);
	}
}

void main() {
	//za eventualne nejasnoce analizirati testni program i ispise na slici
	Datum danas; danas.Unos(13, 6, 2016);

	Artikal cigarete, cokolada, sok;
	cigarete.Unos("Cigarete", 2.23);
	cokolada.Unos("Cokolada", 1.23);
	sok.Unos("Cappy", 2.10);

	Racun racun;
	racun.Unos(GenerisiSljedeciBrojRacuna(), danas);
	racun.DodajArtikal(cokolada, 5);
	racun.DodajArtikal(sok, 10);
	racun.DodajArtikal(cigarete, 2);
	racun.DodajArtikal(cigarete, 5);
	racun.Ispis();


	Artikal * p1 = racun.BinarnaPretraga("Cokolada");
	cout << "Binarnom pretragom pronadjen artikal -> ";
	p1->Ispis();
	racun.Ispis();

	Artikal * p2 = PronadjiNajskupljiArtikal(racun);
	cout << "Rekurzivno pronadjen najskuplji artikal -> ";
	p2->Ispis();
	cout << endl;

	//dealocirati zauzetu memoriju
	danas.Dealociraj();
	cigarete.Dealociraj();
	cokolada.Dealociraj();
	sok.Dealociraj();
	racun.Dealociraj();

	system("pause>null");
}