
#include <iostream>
using namespace std;


//narednu liniju code-a ignorisite, osim u slucaju da vam bude predstavljala smetnje u radu
#pragma warning(disable:4996)

const char * crt = "\n-------------------------------\n";
const int MAX_ODGOVORA = 10;

char * AlocirajNizKaraktera(const char * sadrzaj) {
	if (sadrzaj == nullptr)
		return nullptr;
	int vel = strlen(sadrzaj) + 1;
	char * temp = new char[vel];
	strcpy_s(temp, vel, sadrzaj);
	return temp;
}

struct Kandidat {
	char * _imePrezime;
	char _JMBG[14];
	void Unos(const char * ip, const char * jmbg) {
		_imePrezime = AlocirajNizKaraktera(ip);
		strcpy_s(_JMBG, jmbg);
	}
	void Dealociraj() {
		delete[] _imePrezime; _imePrezime = nullptr;
	}
	void Ispis() {
		cout << _imePrezime << " " << _JMBG << " ";
	}
};

struct Pitanje {
	char * _tekstPitanja;
	char * _odgovori[MAX_ODGOVORA];//maksimalno 10 odgovora
								   /* svako pitanje moze imati jedan ili vise tacnih odgovora, a njihove lokacije se pohranjuju u niz _tacni*/
	int _tacni[MAX_ODGOVORA];
	/*ako pitanje ima vise tacnih odgovora, onda se broj osvojenih bodova (tokom izrade testa) odredjuje na osnovu broj tacnih odgovora (npr. ako pitanje nosi 5 boda i ima 2 tacna odgovora,
	a student oznaci 1 tacan odgovor onda ce osvojeni broj bodova na tom pitanju iznositi 2.5. drugim rijecima, vrijednost ovog atributa je bitna zbog  odredjivanja uspjeha kandidata prilikom izrade ispita)*/
	int _bodova;

	void Unos(const char * tekstPitanja, int bodova) {
		_tekstPitanja = AlocirajNizKaraktera(tekstPitanja);
		/*umjesto dodavanja brojaca iskoristite defaultne vrijednosti clanova niza kako biste
		manipulisali njihovim vrijednostima (dodavanje, uklanjanje)*/
		for (size_t i = 0; i < MAX_ODGOVORA; i++) {
			_odgovori[i] = nullptr;
			_tacni[i] = -1;
		}
		_bodova = bodova;
	}
	void Dealociraj() {
		delete[] _tekstPitanja; _tekstPitanja = nullptr;
		for (size_t i = 0; i < MAX_ODGOVORA; i++) {
			if (_odgovori[i] == nullptr)
				break;
			delete[] _odgovori[i]; _odgovori[i] = nullptr;
		}
	}

	bool AddOdgovor(char const * tekstPitanja, bool daLiJeTacanOdgovor) {
		/*dodaje novi odgovor u listu ponudjenih odgovora. onemoguciti ponavljanje odgovora, te voditi racuna da vise odgovora moze biti tacno*/

		for (int i = 0; i < MAX_ODGOVORA; i++)
		{
			if (_odgovori[i] != nullptr && strcmp(_odgovori[i], tekstPitanja) == 0) {
				return false;
			}

			if (_odgovori[i] == nullptr) {
				_odgovori[i] = new char[strlen(tekstPitanja) + 1];
				strcpy_s(_odgovori[i], strlen(tekstPitanja)+1, tekstPitanja);
				if (daLiJeTacanOdgovor) {
					for (int j = 0; j < MAX_ODGOVORA; j++)
					{
						if (_tacni[j] == -1) {
							_tacni[j] = i;
							return true;
						}
					}
				}
				return true;
			}
		}
		return false;
	}

	bool RemoveOdgovor(int lokacija) {
		///*na osnovu primljene lokacije uklanja podatke o ponudjenom odgovoru. prilikom uklanjanja ocuvati redoslijed dodavanja odgovora.
		//takodjer, potrebno je onemoguciti uklanjanje svih tacnih odgovora na nacin da se prije njegovog uklanjanja mora unijeti novi tacan odgovor */
		

		/*if (_odgovori[lokacija] == nullptr) return true;

		for (int i = 0; i < MAX_ODGOVORA; i++)
		{
			if (_odgovori[i] != NULL && strcmp(_odgovori[i], _odgovori[lokacija]) == 0) {
				for (int j = 0; j < MAX_ODGOVORA; j++)
				{
					if (_tacni[j] == i) {
						_tacni[j] == -1;
					}
				}
			}
		}

		delete[] _odgovori[lokacija-1]; _odgovori[lokacija-1] = nullptr;
		return true;*/


		
		for (int i = 0; i < MAX_ODGOVORA; i++)
		{
			if (_odgovori[i] != NULL && strcmp(_odgovori[i], _odgovori[lokacija]) == 0) {
				for (int j = 0; j < MAX_ODGOVORA; j++)
				{
					if (_tacni[j] = i) {
						_tacni[j] = -1;
					}
				}
			}
		}
		delete[] _odgovori[lokacija-1]; _odgovori[lokacija-1] = nullptr;
		return true;
	}

	void Ispis() {
		/*
	-------------------------------
	1. U kojem gradu se nalazi FIT?
	-------------------------------
	1. Mostar
	2. Sarajevo
	3. Tuzla
	4. Zenica
	-------------------------------
	*/
		cout << crt;
		cout << _tekstPitanja << endl;
		cout << crt;
		for (int i = 0; i < MAX_ODGOVORA; i++)
		{
			if (_odgovori[i] != NULL) {
				cout <<i+1<<"."<< _odgovori[i] << endl;
			}
		}
		cout << crt;
	}
};

struct PrijemniIspit {
	//kandidati prijavljeni za prijemni ispit
	Kandidat * _prijavljeniKandidati;
	/*nakon sto urade ispit tj. nakon sto odgovore na pitanja za svakog od kandidata se cuva ostvareni uspjeh u okviru niza _uspjehKandidata. 
	broj elemenata u nizu _uspjehKandidata bi trebao biti identican broju elemenata u nizu _prijavljeniKandidati*/
	float * _uspjehKandidata;
	int _trenutnoKandidata;
	//prag prolaznosti na prijemnom ispitu npr. 60.5 procenata
	float _pragProlaznosti;
	float _kazneniBodovi;

	/*vrijednost kazenog boda (npr. 0.30) se mnozi sa svakim netacnim odgovorom na pitanje i oduzima od osvojenih bodova za to pitanje
	npr. ako pitanje nosi 5 bodova i ima 2 tacna odgovora, a student odabere 1 tacan i 2 netacna odgovora, broj osvojenih bodova ce biti 1.9
	(1 tacan = 2.5; 2 netacna * 0.30 kaznenih = 0.6; 2.5 - 0.6 = 1.9)*/

	void Unos(float pragProlaznosti, float kazneniBodovi) {
		_pragProlaznosti = pragProlaznosti;
		_kazneniBodovi = kazneniBodovi;
		_prijavljeniKandidati = nullptr;
		_uspjehKandidata = nullptr;
		_trenutnoKandidata = 0;
	}
	void Dealociraj() {
		for (size_t i = 0; i < _trenutnoKandidata; i++)
			_prijavljeniKandidati[i].Dealociraj();
		delete[] _prijavljeniKandidati; _prijavljeniKandidati = nullptr;
		delete[] _uspjehKandidata; _uspjehKandidata = nullptr;
	}

	bool DodajKandidata(Kandidat kandidat) {
		/*dodaje novog kandidata u listu prijavljenih. onemoguciti dodavanje kandidata sa istim jmbg-om*/

		for (int i = 0; i < _trenutnoKandidata; i++)
		{
			if (strcmp(_prijavljeniKandidati[i]._JMBG, kandidat._JMBG) == 0) {
				return false;
			}
		}

		Kandidat * temp = new Kandidat[_trenutnoKandidata + 1];
		for (int i = 0; i < _trenutnoKandidata; i++)
		{
			temp[i].Unos(_prijavljeniKandidati[i]._imePrezime, _prijavljeniKandidati[i]._JMBG);
			_prijavljeniKandidati[i].Dealociraj();
		}
		if (_prijavljeniKandidati != NULL) {
			delete[] _prijavljeniKandidati;
		}

		temp[_trenutnoKandidata].Unos(kandidat._imePrezime, kandidat._JMBG);
		_prijavljeniKandidati = temp;
		_trenutnoKandidata++;
		return true;
	}
};

int main() {

	Kandidat jasmin, adel;
	jasmin.Unos("Jasmin Azemovic", "1111111111111");
	adel.Unos("Adel Handzic", "2222222222222");

	const int brojPitanja = 3;
	Pitanje * pitanja[brojPitanja];
	for (size_t i = 0; i < brojPitanja; i++)
		pitanja[i] = new Pitanje;

	//parametri: tekstPitanja, brojBodova
	pitanja[0]->Unos("U kojem gradu se nalazi FIT?", 4);

	/*dodaje novi odgovor u listu ponudjenih odgovora. onemoguciti ponavljanje odgovora, te voditi racuna da vise odgovora moze biti tacno*/
	//parametri: tekstOdgovora, daLiJeTacanTajOdgovor
	if (pitanja[0]->AddOdgovor("Mostar", true))
		cout << "Odgovor uspjesno dodan!" << crt;
	if (pitanja[0]->AddOdgovor("Sarajevo", false))
		cout << "Odgovor uspjesno dodan!" << crt;
	if (!pitanja[0]->AddOdgovor("Sarajevo", false))//Sarajevo je vec dodano
		cout << "Odgovor nije uspjesno dodan!" << crt;
	if (pitanja[0]->AddOdgovor("Tuzla", false))
		cout << "Odgovor uspjesno dodan!" << crt;
	if (pitanja[0]->AddOdgovor("Fojnica", false))
		cout << "Odgovor uspjesno dodan!" << crt;
	if (pitanja[0]->AddOdgovor("Zenica", false))
		cout << "Odgovor uspjesno dodan!" << crt;

	///*na osnovu primljene lokacije uklanja podatke o ponudjenom odgovoru. prilikom uklanjanja ocuvati redoslijed dodavanja odgovora. takodjer, potrebno je onemoguciti uklanjanje svih tacnih odgovora na nacin da se prije njegovog uklanjanja mora unijeti novi tacan odgovor */
	if (pitanja[0]->RemoveOdgovor(4))//uklanja odgovor Fojnica
		cout << "Odgovor uspjesno uklonjen!" << crt;

	/*
	-------------------------------
	1. U kojem gradu se nalazi FIT?
	-------------------------------
	1. Mostar
	2. Sarajevo
	3. Tuzla
	4. Zenica
	-------------------------------
	*/
	pitanja[0]->Ispis();//prethodno je prikazan ocekivani ispis pitanja

	pitanja[1]->Unos("Odaberite tacne odgovore.", 5);

	if (pitanja[1]->AddOdgovor("FIT se nalazi u Mostaru", true))
		cout << "Odgovor uspjesno dodan!" << crt;
	if (pitanja[1]->AddOdgovor("FIT je clanica UNMO-a", true))
		cout << "Odgovor uspjesno dodan!" << crt;
	if (pitanja[1]->AddOdgovor("U Mostaru je uvijek jako vruce", false))
		cout << "Odgovor uspjesno dodan!" << crt;

	pitanja[2]->Unos("Studij na FIT-u traje 4 godine?", 3);
	if (pitanja[2]->AddOdgovor("Ne", true))
		cout << "Odgovor uspjesno dodan!" << crt;
	if (pitanja[2]->AddOdgovor("Da", false))
		cout << "Odgovor uspjesno dodan!" << crt;
	/*....*/

	pitanja[2]->Ispis();

	PrijemniIspit prijemni2018;
	//parametri: pragProlaznosti, kazneniBodovi
	prijemni2018.Unos(60, 0.30);
	/*dodaje novog kandidata u listu prijavljenih. onemoguciti dodavanje kandidata sa istim jmbg-om*/
	if (prijemni2018.DodajKandidata(jasmin))
		cout << "Kandidat uspjesno dodan" << crt;
	if (prijemni2018.DodajKandidata(adel))
		cout << "Kandidat uspjesno dodan" << crt;

	///*kandidatu sa JMBG 1111111111111 pokrece ispit na nacin da mu se prikazuju pitanja, od njega zahtijevaju odgovori, te izracunava ostvareni uspjeh (da li je polozio ili ne, te sa kojim procentom uspjesnosti). prilikom izracuvanja uspjesnosti voditi racuna o kaznenim bodovima. po okoncanju ispita, uspjeh kandidata se pohranjuje u niz _uspjehKandidata. za pokretanje ispita kandidat prethodno mora biti dodan u listu. po
	//vlastitoj zelji implementirajte nacin oznacavanja veceg broja tacnih odgovoran na postavljeno pitanje (jedno pitanje moze imati vise tacnih odgovora)*/
	//if (prijemni2018.PokreniIspit("1111111111111", pitanja, brojPitanja))
	//	cout << "Ispit uspjesno okoncan!" << crt;
	//if (!prijemni2018.PokreniIspit("1111111111111", pitanja, brojPitanja))//kandidat jasmin je vec radio ispit
	//	cout << "Kandidat je vec polagao prijemni ispit!" << crt;

	//if (prijemni2018.PokreniIspit("2222222222222", pitanja, brojPitanja))
	//	cout << "Ispit uspjesno okoncan!" << crt;

	//Kandidat * najbolji = nullptr;
	//float uspjeh = 0;
	///*getNajbolji student je lambda funkcija koja treba vratiti podatke o studentu koji je ostvario najbolji uspjeh na prijemnom ispitu*/
	//tie(najbolji, uspjeh) = getNajboljiStudent();
	//if (najbolji != nullptr)
	//	cout << "Najbolji uspjeh od " << uspjeh << " % ostvario je kandidat " << najbolji->_imePrezime << "." << endl;


	jasmin.Dealociraj();
	adel.Dealociraj();
	for (size_t i = 0; i < brojPitanja; i++) {
		pitanja[i]->Dealociraj();
		pitanja[i] = nullptr;
	}

	system("pause>null");
	return 0;
}