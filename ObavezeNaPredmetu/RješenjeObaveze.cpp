#include<iostream>
#include<memory>
using namespace std;

//narednu liniju code-a ignorisite, osim u slucaju da vam bude predstavljala smetnje u radu
#pragma warning(disable:4996)

enum VrstaObaveze { Seminarski, Parcijalni1, Parcijalni2, Integralni, Prakticni };
char const * CharVrstaObaveze[] = { "Seminarski", "Parcijalni1", "Parcijalni2", "Integralni", "Prakticni" };

struct Datum {
	int * _dan, *_mjesec, *_godina;
	void Unos(int d, int m, int g) {
		_dan = new int(d);
		_mjesec = new int(m);
		_godina = new int(g);
	}
	void Ispis() {
		cout << *_dan << "/" << *_mjesec << "/" << *_godina << endl;
	}
	void Dealociraj() { delete _dan; delete _mjesec; delete _godina; }

	int sumaDana() {
		int suma = 0;
		suma = *_godina * 365 + *_mjesec * 30 + *_dan;
		return suma;
	}
};
struct ObavezeNaPredmetu {
	shared_ptr<VrstaObaveze> _vrstaObaveze;
	Datum _datumIzvrsenja;
	char * _napomena;
	int _ocjena; // 5 - 10 
	void Unos(VrstaObaveze vrsta, Datum datum, int ocjena, char const * napomena) {
		_vrstaObaveze = make_shared<VrstaObaveze>(vrsta);
		_datumIzvrsenja.Unos(*datum._dan, *datum._mjesec, *datum._godina);
		_napomena = new char[strlen(napomena) + 1];
		strcpy_s(_napomena, strlen(napomena) + 1, napomena);
		_ocjena = ocjena;
	}
	void Ispis() {
		cout << CharVrstaObaveze[*_vrstaObaveze] << " " << _ocjena << " " << _napomena;
		_datumIzvrsenja.Ispis();
		cout << endl;
	}
	void Dealociraj() {
		_datumIzvrsenja.Dealociraj();
		delete[] _napomena; _napomena = nullptr;
	}
};

char const * _kljucneRijeci[15] = { NULL }; /*da bi se odredjena obaveza na predmetu smatrala uspjesno izvrsenom, u napomeni (objekta ObavezeNaPredmetu)
											ne smije stajati niti jedna od rijeci koja se nalazi u nizu _kljucneRijeci*/

struct PolozeniPredmet {
	Datum _datumPolaganja;//datum koji ce se evidentirati kao datum kada je predmet polozen tj. kada je formirana konacna ocjena
	char * _nazivPredmeta;
	ObavezeNaPredmetu * _listaIzvrsenihObaveza;
	int _trenutnoIzvrsenihObaveza;
	int _minimalanBrojDana; //odnosi se na minimalan broj dana koji mora proci od izvrsenja pojedinih obaveza na predmetu (npr. 7 dana)
	int _konacnaOcjena; //formira se na osnovu ocjena izvrsenih obaveza

	void Unos(char const * naziv, int minimalanBrojDana) {
		_nazivPredmeta = new char[strlen(naziv) + 1];
		strcpy_s(_nazivPredmeta, strlen(naziv) + 1, naziv);
		_listaIzvrsenihObaveza = nullptr;
		_trenutnoIzvrsenihObaveza = 0;
		_minimalanBrojDana = minimalanBrojDana;
		_konacnaOcjena = 0;
	}
	void Dealociraj() {
		_datumPolaganja.Dealociraj();
		delete[] _nazivPredmeta; _nazivPredmeta = nullptr;
		for (size_t i = 0; i < _trenutnoIzvrsenihObaveza; i++)
			_listaIzvrsenihObaveza[i].Dealociraj();
		delete[]_listaIzvrsenihObaveza;
	}

	bool DodajIzvrsenuObavezu(VrstaObaveze vrstaObaveze, Datum datum, int ocjena, char const * napomena) {
		/*Na osnovu vrijednosti primljenih parametara osigurati dodavanje novoizvrsene obaveze na predmetu. Potrebno je onemoguciti dodavanje identicnih obaveza,
	a izmedju izvrsenja pojedinih obaveza mora proci najmanje onoliko dana koliko je definisano vrijednoscu atributa _minimalanBrojDana. Identicna vrsta obaveze se moze dodati samo
	u slucaju da je prethodno dodana obaveza (identicne vrste) imala ocjenu 5 ili je u napomeni sadrzavala neku od zabranjenih rijeci (definisanih nizom _kljucneRijeci).*/

		if (_trenutnoIzvrsenihObaveza != 0 && abs(datum.sumaDana() - _listaIzvrsenihObaveza[_trenutnoIzvrsenihObaveza - 1]._datumIzvrsenja.sumaDana()) < _minimalanBrojDana) {
			return false;
		}

		for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
		{
			if (*_listaIzvrsenihObaveza[i]._vrstaObaveze == vrstaObaveze && *_listaIzvrsenihObaveza[i]._vrstaObaveze != Seminarski) {
				bool sadrziKljucnu = false;
				for (int j = 0; j < 15; j++)
				{
					if (_kljucneRijeci[j] != NULL && strstr(_listaIzvrsenihObaveza[i]._napomena, _kljucneRijeci[j]) != nullptr) {
						sadrziKljucnu = true;
					}
				}

				if (_listaIzvrsenihObaveza[i]._ocjena != 5 && !sadrziKljucnu) {
					return false;
				}
			}
		}

		ObavezeNaPredmetu * temp = new ObavezeNaPredmetu[_trenutnoIzvrsenihObaveza + 1];
		for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
		{
			temp[i].Unos(*_listaIzvrsenihObaveza[i]._vrstaObaveze, _listaIzvrsenihObaveza[i]._datumIzvrsenja, _listaIzvrsenihObaveza[i]._ocjena, _listaIzvrsenihObaveza[i]._napomena);
			_listaIzvrsenihObaveza[i].Dealociraj();
		}
		if (_listaIzvrsenihObaveza != NULL) {
			delete[] _listaIzvrsenihObaveza;
		}

		temp[_trenutnoIzvrsenihObaveza].Unos(vrstaObaveze, datum, ocjena, napomena);
		_listaIzvrsenihObaveza = temp;
		_trenutnoIzvrsenihObaveza++;
		return true;
	}

	int FormirajKonacnuOcjenu() {
		/*Konacna ocjene predstavlja prosjecnu ocjenu na predmetu (prosjecnu ocjenu izvrsenih obaveza koje ispunjavaju uslove za uspjesno polaganje predmeta),
		a za njeno formiranje student mora posjedovati polozen integralni ili dva parijcalna ispita. Ukoliko je ispit polozen putem parcijalnih ispita, student takodjer mora imati pozitivno
		(ocjenom vecom od 5) ocijenjena najmanje dva seminarska i jedan prakticni rad. Takodjer, datum polaganja drugog parcijalnog ispita ne smije biti raniji (manji) od datuma polaganja prvog parcijalnog ispita.
		Konacna ocjena ne smije biti formirana u slucaju da napomena bilo koje odbaveze (koja je ucestvovala u formiranju konacne ocjene) sadrzi neku od zabranjenih rijeci (niz _kljucneRijeci).
		U slucaju da neki od navedenih uslova nije zadovoljen konacna ocjena treba biti postavljena na vrijednost 5. Ukoliko su ispunjeni svi uslovi, osim formiranja konacne ocjene, datum polaganja je potrebno
		postaviti na vrijednost datuma posljednje izvrsene obaveze sa najvecom ocjenom.
	*/

		int sumaOcjena = 0;
		bool integralni = false;
		bool parcijalni1 = false;
		bool parcijalni2 = false;
		int brojacSeminarskih = 0;
		int najvecaOcjena = 0;
		Datum datumOcjene;
		datumOcjene.Unos(0, 0, 0);


		for (int i = 0; i < _trenutnoIzvrsenihObaveza; i++)
		{
			if (*_listaIzvrsenihObaveza[i]._vrstaObaveze == Integralni && _listaIzvrsenihObaveza[i]._ocjena > 5) {
				sumaOcjena += _listaIzvrsenihObaveza[i]._ocjena;
				integralni = true;

				break;
			}

			if (*_listaIzvrsenihObaveza[i]._vrstaObaveze == Parcijalni1 && _listaIzvrsenihObaveza[i]._ocjena > 5) {
				sumaOcjena += _listaIzvrsenihObaveza[i]._ocjena;
				parcijalni1 = true;
			}

			if (*_listaIzvrsenihObaveza[i]._vrstaObaveze == Parcijalni2 && _listaIzvrsenihObaveza[i]._ocjena > 5) {
				sumaOcjena += _listaIzvrsenihObaveza[i]._ocjena;
				parcijalni2 = true;
			}

			if (*_listaIzvrsenihObaveza[i]._vrstaObaveze == Seminarski && _listaIzvrsenihObaveza[i]._ocjena > 5) {
				sumaOcjena += _listaIzvrsenihObaveza[i]._ocjena;
				brojacSeminarskih++;
			}

			if (_listaIzvrsenihObaveza[i]._ocjena > najvecaOcjena && _listaIzvrsenihObaveza[i]._datumIzvrsenja.sumaDana() > datumOcjene.sumaDana()) {
				datumOcjene.Unos(*_listaIzvrsenihObaveza[i]._datumIzvrsenja._dan, *_listaIzvrsenihObaveza[i]._datumIzvrsenja._mjesec, *_listaIzvrsenihObaveza[i]._datumIzvrsenja._godina);
			}
		}

		if (integralni) {
			_konacnaOcjena = sumaOcjena;
		}
		else if (parcijalni1 && parcijalni2 && brojacSeminarskih >= 2) {
			_konacnaOcjena = sumaOcjena / (2 + brojacSeminarskih);
		}
		else {
			_konacnaOcjena = 5;
		}
		_datumPolaganja.Unos(*datumOcjene._dan, *datumOcjene._mjesec, *datumOcjene._godina);
		return _konacnaOcjena;

	}
};

float PretragaRekurzivno(PolozeniPredmet predmet, VrstaObaveze obaveza, Datum OD, Datum DO, int brojac, int indeks, float suma = 0) {

	if (predmet._trenutnoIzvrsenihObaveza == indeks)
		return suma / brojac;
	else
		if (*predmet._listaIzvrsenihObaveza[indeks]._vrstaObaveze == obaveza)
			if (predmet._listaIzvrsenihObaveza[indeks]._ocjena > 5)
				if (predmet._listaIzvrsenihObaveza[indeks]._datumIzvrsenja.sumaDana() > OD.sumaDana() && predmet._listaIzvrsenihObaveza[indeks]._datumIzvrsenja.sumaDana() < DO.sumaDana()) {
					suma += predmet._listaIzvrsenihObaveza[indeks]._ocjena;
					brojac++;
				}
	return PretragaRekurzivno(predmet, obaveza, OD, DO, brojac, indeks + 1, suma);
}

void main() {
	_kljucneRijeci[0] = "prepisivao";
	_kljucneRijeci[1] = "ometao";
	_kljucneRijeci[2] = "nije dostavio";

	Datum datumSeminarski1, datumSeminarski2, datumParcijalni1, datumParcijalni2, datumIntegralni;
	datumParcijalni1.Unos(22, 6, 2017);
	datumParcijalni2.Unos(30, 6, 2017);
	datumSeminarski1.Unos(10, 6, 2017);
	datumSeminarski2.Unos(18, 6, 2017);

	PolozeniPredmet prII;
	//naziv predmeta, minimalan broj dana
	prII.Unos("Programiranje II", 7);

	/*Na osnovu vrijednosti primljenih parametara osigurati dodavanje novoizvrsene obaveze na predmetu. Potrebno je onemoguciti dodavanje identicnih obaveza,
	a izmedju izvrsenja pojedinih obaveza mora proci najmanje onoliko dana koliko je definisano vrijednoscu atributa _minimalanBrojDana. Identicna vrsta obaveze se moze dodati samo
	u slucaju da je prethodno dodana obaveza (identicne vrste) imala ocjenu 5 ili je u napomeni sadrzavala neku od zabranjenih rijeci (definisanih nizom _kljucneRijeci).*/
	if (prII.DodajIzvrsenuObavezu(Parcijalni1, datumParcijalni1, 6, "uslovno polozen ispit"))
		cout << "Parcijalni 1...dodan" << endl;
	if (prII.DodajIzvrsenuObavezu(Parcijalni2, datumParcijalni2, 5, "previse gresaka, a tokom ispita ometao druge studente"))
		cout << "Parcijalni 2...dodan" << endl;
	if (prII.DodajIzvrsenuObavezu(Seminarski, datumSeminarski1, 8, "5implementirani svi algoritmi"))
		cout << "Seminarski1...dodan" << endl;
	if (prII.DodajIzvrsenuObavezu(Seminarski, datumSeminarski2, 6, "rad slican kao kod studenta IB150388"))
		cout << "Seminarski2...dodan" << endl;

	/*Konacna ocjene predstavlja prosjecnu ocjenu na predmetu (prosjecnu ocjenu izvrsenih obaveza koje ispunjavaju uslove za uspjesno polaganje predmeta),a za njeno formiranje student mora posjedovati polozen integralni ili dva parijcalna ispita. Ukoliko je ispit polozen putem parcijalnih ispita, student takodjer mora imati pozitivno (ocjenom vecom od 5) ocijenjena najmanje dva seminarska i jedan prakticni rad. Takodjer, datum polaganja drugog parcijalnog ispita ne smije biti raniji (manji) od datuma polaganja prvog parcijalnog ispita. Konacna ocjena ne smije biti formirana u slucaju da napomena bilo koje odbaveze (koja je ucestvovala u formiranju konacne ocjene) sadrzi neku od zabranjenih rijeci (niz _kljucneRijeci). U slucaju da neki od navedenih uslova nije zadovoljen konacna ocjena treba biti postavljena na vrijednost 5. Ukoliko su ispunjeni svi uslovi, osim formiranja konacne ocjene, datum polaganja je potrebno postaviti na vrijednost datuma posljednje izvrsene obaveze sa najvecom ocjenom.
	*/
	cout << "Konacna ocjena iz predmeta PRII je: " << prII.FormirajKonacnuOcjenu() << endl;

	datumParcijalni2.Unos(22, 7, 2017);
	if (prII.DodajIzvrsenuObavezu(Parcijalni2, datumParcijalni2, 6, ""))
		cout << "Parcijalni 2...dodan" << endl;
	cout << "Konacna ocjena iz predmeta PRII je: " << prII.FormirajKonacnuOcjenu() << endl;
	Datum OD, DO; OD.Unos(1, 1, 2017); DO.Unos(1, 7, 2017);

	/*Rekurzivna funkcija treba vratiti prosjecnu ocjenu uspjesno izvrsenih obaveze na predmetu odredjene vrste u definisanom periodu. Izbjegavati koristenje globalnih varijabli*/
	float prosjek = PretragaRekurzivno(prII, Seminarski, OD, DO, 0, 0);
	cout << "Prosjecna ocjena izvrsenih obaveza je: " << prosjek << endl;

	///*Koristeci lambda-u, unutar funkcije GetBrojObavezaIznadProsjeka pronaci broj izvrsenih obaveza koje imaju istu ili vecu ocjenu od prosjecne*/
	//int iznadProsjeka = GetBrojObavezaIznadProsjeka(prII, prosjek);
	//cout << "Broj izvrsenih obaveza iznad prosjeka je: " << iznadProsjeka << endl;

	datumParcijalni1.Dealociraj();
	datumParcijalni2.Dealociraj();
	datumSeminarski1.Dealociraj();
	datumSeminarski2.Dealociraj();
	//datumIntegralni.Dealociraj();
	prII.Dealociraj();

	system("pause>null");
}