#include<iostream>
#include<tuple>
using namespace std;


const char * not_set = "NEMA_VRIJEDNOST";
const char * crt = "\n-------------------------------------------\n";
const int min_polgavlja = 3;
const int min_karaktera_po_poglavlju = 30;
const int max_zavrsnih = 30;

char * AlocirajNizKaraktera(const char * sadrzaj) {
	if (sadrzaj == nullptr)
		return nullptr;
	int vel = strlen(sadrzaj) + 1;
	char * temp = new char[vel];
	strcpy_s(temp, vel, sadrzaj);
	return temp;
}

struct Poglavlje {
	char * _naslov;
	char * _sadrzaj;
	bool _prihvaceno;
	int _ocjena;//da bi se poglavlje smatralo prihvacenim ocjena mora biti u opsegu od 6 - 10

	void Unos(const char * naslov = nullptr, const char * sadrzaj = nullptr) {
		_ocjena = 0; _prihvaceno = false;
		_naslov = AlocirajNizKaraktera(naslov);
		_sadrzaj = AlocirajNizKaraktera(sadrzaj);
	}
	void Dealociraj() {
		delete[] _naslov; _naslov = nullptr;
		delete[] _sadrzaj; _sadrzaj = nullptr;
	}
	void Ispis() {
		if (_naslov == nullptr || _sadrzaj == nullptr)
			return;
		cout << endl << "Poglavlje -> " << _naslov << endl << _sadrzaj << endl;
		if (_prihvaceno)
			cout << "Ocjena: " << _ocjena << endl;;
	}
	void OcijeniPoglavlje(int ocjena) {
		_ocjena = ocjena;
		if (_ocjena > 5 && ocjena <= 10)
			_prihvaceno = true;
	}
};

struct ZavrsniRad {

	char * _brojIndeksa;
	char * _tema;
	Poglavlje * _poglavljaRada;
	int _trenutnoPoglavlja;
	char * _datumOdbrane;
	float _konacnaOcjena; //konacna ocjena zavrsnog rada predstavlja prosjek ocjena svih poglavlja u zavrsnom radu koja se izracunava u momentu zakazivanja odbrane

	void Unos(const char * brojIndeksa, const char * nazivTeme = nullptr) {
		_konacnaOcjena = 0;
		_brojIndeksa = AlocirajNizKaraktera(brojIndeksa);
		_tema = AlocirajNizKaraktera(nazivTeme);
		_datumOdbrane = AlocirajNizKaraktera(not_set);
		_poglavljaRada = nullptr;
		_trenutnoPoglavlja = 0;
	}

	void DodajPoglavlje(char const * nazivPoglavlja, char const * sadrzajPoglavlja) {

		for (int i = 0; i < _trenutnoPoglavlja; i++)
		{
			if (strcmp(_poglavljaRada[i]._naslov, nazivPoglavlja) == 0) {
				int duzina = strlen(_poglavljaRada[i]._sadrzaj) + strlen(sadrzajPoglavlja) + 1 + 1;
				char * temp;
				temp = new char[duzina];
				strcpy_s(temp, duzina, _poglavljaRada[i]._sadrzaj);
				strcat_s(temp, duzina, " ");
				strcat_s(temp, duzina, sadrzajPoglavlja);

				delete[] _poglavljaRada[i]._sadrzaj; _poglavljaRada[i]._sadrzaj = nullptr;

				_poglavljaRada[i]._sadrzaj = temp;
				return;
			}
		}

		Poglavlje * temp = new Poglavlje[_trenutnoPoglavlja + 1];
		for (int i = 0; i < _trenutnoPoglavlja; i++)
		{
			temp[i] = _poglavljaRada[i];
		}

		temp[_trenutnoPoglavlja].Unos(nazivPoglavlja, sadrzajPoglavlja);

		_trenutnoPoglavlja++;

		delete[] _poglavljaRada;

		_poglavljaRada = temp;
	}

	void OcijeniPoglavlje(char const * nazivPoglavlja, int ocjena) {
		for (int i = 0; i < _trenutnoPoglavlja; i++)
		{
			if (strcmp(_poglavljaRada[i]._naslov, nazivPoglavlja) == 0) {
				_poglavljaRada[i].OcijeniPoglavlje(ocjena);
				return;
			}
		}
	}

	void Kopiraj(ZavrsniRad & zavrsniRad) {
		Unos(zavrsniRad._brojIndeksa, zavrsniRad._tema);
		for (int i = 0; i < _trenutnoPoglavlja; i++)
		{
			DodajPoglavlje(zavrsniRad._poglavljaRada[i]._naslov, zavrsniRad._poglavljaRada[i]._sadrzaj);
			OcijeniPoglavlje(zavrsniRad._poglavljaRada[i]._naslov, zavrsniRad._poglavljaRada[i]._ocjena);
		}
	}

	void Dealociraj() {
		delete[] _tema; _tema = nullptr;
		delete[] _datumOdbrane; _datumOdbrane = nullptr;
		delete[] _brojIndeksa; _brojIndeksa = nullptr;
		for (size_t i = 0; i < _trenutnoPoglavlja; i++)
			_poglavljaRada[i].Dealociraj();
		delete[] _poglavljaRada; _poglavljaRada = nullptr;
	}

	void Ispis() {
		cout << "Tema rada: " << _tema << endl;
		cout << "Sadrzaj: " << endl;
		for (size_t i = 0; i < _trenutnoPoglavlja; i++)
			_poglavljaRada[i].Ispis();
		cout << "Datum odbrane rada: " << _datumOdbrane << endl << " Ocjena: " << _konacnaOcjena << endl;
	}
};

struct Nastavnik {

	char * _imePrezime;
	ZavrsniRad * _teme[max_zavrsnih] = { nullptr };

	void Unos(const char * imePrezime) {
		_imePrezime = AlocirajNizKaraktera(imePrezime);
	}

	bool DodajZavrsniRad(ZavrsniRad zavrsniRad) {

		for (int i = 0; i < max_zavrsnih; i++)
		{
			if (_teme[i] != NULL) {
				if (strcmp(_teme[i]->_tema, zavrsniRad._tema) == 0 || strcmp(_teme[i]->_brojIndeksa, zavrsniRad._brojIndeksa) == 0) {
					return false;
				}
			}
			if (_teme[i] == nullptr) {
				_teme[i] = new ZavrsniRad;
				_teme[i]->Kopiraj(zavrsniRad);
				return true;
			}
		}
	}

	void Dealociraj() {
		delete[] _imePrezime; _imePrezime = nullptr;
		for (size_t i = 0; i < max_zavrsnih; i++)
		{
			if (_teme[i] != nullptr) {
				_teme[i]->Dealociraj();
				delete _teme[i];
			}
		}
	}

	void Ispis() {
		cout << crt << _imePrezime << crt;
		for (size_t i = 0; i < max_zavrsnih; i++)
			if (_teme[i] != nullptr)
				_teme[i]->Ispis();
	}

	ZavrsniRad * ZakaziOdbranuRada(const char * brojIndeksa, const char * datumOdbrane) {

		for (int i = 0; i < max_zavrsnih; i++)
		{
			if (_teme[i] != nullptr && strcmp(brojIndeksa, _teme[i]->_brojIndeksa) == 0) {
				if (_teme[i]->_trenutnoPoglavlja < min_polgavlja)
					return nullptr;

				float prosjek = 0;

				for (int j = 0; j < _teme[i]->_trenutnoPoglavlja; j++)
				{
					if (strlen(_teme[i]->_poglavljaRada[j]._sadrzaj) < min_karaktera_po_poglavlju || !_teme[i]->_poglavljaRada[j]._prihvaceno)
						return nullptr;

					prosjek += _teme[i]->_poglavljaRada[j]._ocjena;
				}
				prosjek /= _teme[i]->_trenutnoPoglavlja;

				_teme[i]->_konacnaOcjena = int(prosjek * 100) / float(100);

				_teme[i]->_datumOdbrane = new char[strlen(datumOdbrane) + 1];
				strcpy_s(_teme[i]->_datumOdbrane, strlen(datumOdbrane) + 1, datumOdbrane);

				return _teme[i];
			}
		}
		return nullptr;
	}
};

tuple<char*, float, float> PronadjiNajStudenta(Nastavnik * nastavnici[], int brojNastavnika, float minimalnaKonacnaOcjena) {

	/*funkcija PronadjiNajStudenta ima zadatak da pronadje prvog studenta koji je zavrsni rad odbranio kod
	nastavnika/mentora koji ima najnizu prosjecnu ocjenu radova (sumaSvihOcjenaZavrsnihRadova/brojZavrsnihRadova), te
	tom prilikom ostvario (odnosi se na studenta) ocjenu vecu od proslijedjene (npr. 8.2)*/

	float prosjekStudenta = 0, prosjekMentora = 0;
	char * indeksStudenta = nullptr;

	Nastavnik * mentor = nullptr;

	for (int i = 0; i < brojNastavnika; i++)
	{
		float sumaSvihOcjenaZavrsnihRadova = 0;
		int brojZavrsnihRadova = 0;

		for (int j = 0; j < max_zavrsnih; j++)
		{
			if (nastavnici[i]->_teme[j] != nullptr && nastavnici[i]->_teme[j]->_konacnaOcjena > 0) {
				brojZavrsnihRadova++;
				sumaSvihOcjenaZavrsnihRadova += nastavnici[i]->_teme[j]->_konacnaOcjena;
			}
		}

		float prosjek = 0;
		if (brojZavrsnihRadova > 0)
			prosjek = sumaSvihOcjenaZavrsnihRadova / brojZavrsnihRadova;

		if (prosjekMentora == 0 || prosjek < prosjekMentora) {
			prosjekMentora = prosjek;
			mentor = nastavnici[i];
		}

	}

	if (mentor != nullptr) {
		float prosjek = 0;

		for (int i = 0; i < max_zavrsnih; i++)
		{
			if (mentor->_teme[i] != nullptr && mentor->_teme[i]->_konacnaOcjena >= minimalnaKonacnaOcjena) {
				indeksStudenta = mentor->_teme[i]->_brojIndeksa;
				prosjekStudenta = mentor->_teme[i]->_konacnaOcjena;
				break;
			}
		}
	}

	return make_tuple(indeksStudenta, prosjekStudenta, prosjekMentora);
}



int main() {

	const int max = 2;
	Nastavnik * nastavnici[max];

	nastavnici[0] = new Nastavnik;     nastavnici[0]->Unos("Denis Music");
	nastavnici[1] = new Nastavnik;     nastavnici[1]->Unos("Emina Junuz");

	ZavrsniRad multimedijalni;
	//parametri: brojIndeksa, tema
	multimedijalni.Unos("IB120021", "Multimedijalni informacijski sistem za visoko - obrazovnu ustanovu");
	ZavrsniRad podrsa_operaterima;
	podrsa_operaterima.Unos("IB130031", "Sistem za podršku rada kablovskog operatera");
	ZavrsniRad analiza_sigurnosti;
	analiza_sigurnosti.Unos("IB140041", "Prakticna analiza sigurnosti bežiènih raèunarskih mreža");
	ZavrsniRad kriptografija;
	kriptografija.Unos("IB120021", "Primjena teorije informacija u procesu generisanja kriptografskih kljuèeva");

	/*u zavrsni rad dodaje novo poglavlje i njegov sadrzaj. ukoliko poglavlje vec postoji u zavrsnom radu, funkcija tom poglavlju treba dodati novi sadrzaj i pri tome zadrzi postojeci (izmedju postojeceg i novog sadrzaja se dodaje prazan prostor). u slucaju da poglavlje ne postoji, ono se dodaje zajedno sa sadrzajem*/
	//parametri: nazivPoglavlja, sadrzajPoglavlja
	multimedijalni.DodajPoglavlje("Uvod", "U ovom poglavlju ce biti rijeci");
	multimedijalni.DodajPoglavlje("Uvod", "o multimedijalnim sistemima koji se danas koriste");
	multimedijalni.DodajPoglavlje("Uvod", "u savremenom poslovanju");
	multimedijalni.DodajPoglavlje("Vrste multimedijalnih sistema", "Danas se moze govoriti o nekoliko vrsta multimedijalnih sistema, a neke od najznacajnijih su ...");
	multimedijalni.DodajPoglavlje("Teorija multimedije", "Sadrzaj koji bi trebao stajati na pocetku treceg poglavlja zavrsnog rada o multimediji studenta IB130011");

	//nazivPoglavlja, ocjena
	multimedijalni.OcijeniPoglavlje("Uvod", 8);


	///*funkcija DodajZavrsniRad ima zadatak da odredjenom nastavniku dodijeli mentorstvo na zavrsnom radu. sprijeciti dodavanje zavrsnih radova sa istom temom kao i mogucnost da jedan student kod istog nastavnika posjeduje vise zavrsnih radova*/
	//brojIndeksa, zavrsniRad
	if (nastavnici[0]->DodajZavrsniRad(multimedijalni))
		cout << "Zavrsni rad uspjesno dodat!" << endl;
	if (nastavnici[0]->DodajZavrsniRad(podrsa_operaterima))
		cout << "Zavrsni rad uspjesno dodat!" << endl;
	if (!nastavnici[0]->DodajZavrsniRad(podrsa_operaterima))//dupliranje rada, onemoguciti dodavanje
		cout << "Zavrsni rad nije dodat!" << endl;
	if (!nastavnici[0]->DodajZavrsniRad(kriptografija)) //studentu vec dodijeljen rad, onemoguciti dodavanje
		cout << "Zavrsni rad nije dodat!" << endl;
	if (nastavnici[1]->DodajZavrsniRad(analiza_sigurnosti))
		cout << "Zavrsni rad uspjesno dodat!" << endl;


	///*funkcija ZakaziOdbranuRada ima zadatak da studentu sa proslijedjenim brojem indeksa zakaze odbranu zavrsnog rada sto podrazumijeva
	//izracunavanje konacne ocjene i definisanje datuma odbrane. odbrana rada se moze zakazati samo studentu koji je rad prethodno prijavio.
	//Za zakazivanje odbrane rada moraju biti zadovoljeni sljedeci uslovi:
	//1. zavrsni rad mora imati broj poglavlja veci od minimalnog
	//2. svako poglavlje mora imati broj karaktera veci od minimalnog
	//3. svako poglavlje mora biti prihvaceno/odobreno
	//ukoliko su zadovoljeni prethodni kriteriji, izracunava se konacna ocjena rada (prosjek ocjena svih poglavlja), postavlja datum odbrane rada i vraca pokazivac na rad kome je zakazana odbrana.
	//u slucaju da student sa primljenim brojem indeksa nije prijavio zavrsni rad ili neki od postavljenih kriterija nije zadovoljen, funkcija vraca nullptr.
	//*/

	//paramteri: brojIndeksa, datumOdbrane
	ZavrsniRad * zr1 = nastavnici[0]->ZakaziOdbranuRada("IB120021", "25.09.2018");
	if (zr1 != nullptr)
		zr1->Ispis();

	zr1 = nastavnici[0]->ZakaziOdbranuRada("IB180081", "25.09.2018");//student sa brojem indeksa IB180081 jos uvijek nije prijavio rad
	if (zr1 == nullptr)
		cout << "Odbrana ne moze biti zakazana!" << endl;

	//ispisuje sve podatke o nastavniku i njegovim mentorstvima
	nastavnici[0]->Ispis();
	nastavnici[1]->Ispis();

	/*funkcija PronadjiNajStudenta ima zadatak da pronadje prvog studenta koji je zavrsni rad odbranio kod
	nastavnika/mentora koji ima najnizu prosjecnu ocjenu radova (sumaSvihOcjenaZavrsnihRadova/brojZavrsnihRadova), te
	tom prilikom ostvario (odnosi se na studenta) ocjenu vecu od proslijedjene (npr. 8.2)*/
	float prosjekStudenta = 0, prosjekMentora = 0;
	char * indeksStudenta;
	//parametri PronadjiNajStudenta: nastavnici, brojNastavnika, minimalnaKonacnaOcjena
	tie(indeksStudenta, prosjekMentora, prosjekStudenta) = PronadjiNajStudenta(nastavnici, max, 8.2);
	if (indeksStudenta != nullptr)
		cout << crt << indeksStudenta << " " << prosjekMentora << " " << prosjekStudenta << crt;

	for (int i = 0; i < max; i++) {
		nastavnici[i]->Dealociraj();
		delete nastavnici[i];
		nastavnici[i] = nullptr;
	}


	system("pause>0");
	return 0;
}