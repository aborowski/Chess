#ifndef PROTOKOL_H
#define PROTOKOL_H
#include <stdbool.h>
#include <sys/types.h>

#define MAX_PSEUDONIM 50
#define MAX_GRACZY 40
#define MAX_OBSERWATORZY 20
#define MAX_ROZGRYWKI 10
#define SZACHOWNICA_ROZMIAR 8

enum {	KLIENT_GRACZ = 1,
	KLIENT_ROZGRYWKA,
	KLIENT_OBSERWATOR,
	KLIENT_OBSERWATOR_ROZGRYWKA,
	KLIENT_SZACHOWNICA
};

enum {
	SERWER_GRACZ = 10,
	SERWER_ROZGRYWKA,
	SERWER_LISTAGRACZY,
	SERWER_LISTAROZGRYWEK,
	SERWER_OBSERWATOR,
	SERWER_SZACHOWNICA
};

typedef enum {	KOLOR_BIALY, KOLOR_CZARNY } KOLOR_TYP;

typedef struct {
	char pseudonim[MAX_PSEUDONIM];
	KOLOR_TYP kolor;
	int stopienZaawansowania;
} Preferencje;

typedef struct {
	long typ;
	int id;
	pid_t pid;
	int kolejkaNumer;
	bool zajety;
	int pamiecNumer;
} Gracz;

typedef struct {
	long typ;
	Gracz listaGraczy[MAX_GRACZY];
	int liczbaGraczy;
} listaGraczy;

typedef struct {
	long typ;
	int idRozgrywka;
	pid_t pid;
	int kolejkaNumer;
} Obserwator;

typedef struct {
	long typ;
	int id;
	pid_t pid;
	Gracz graczPierwszy;
	Gracz graczDrugi;
	Obserwator listaObserwatorow[MAX_OBSERWATORZY];
	int liczbaObserwatorow;
} Rozgrywka;

typedef struct {
	long typ;
	Rozgrywka listaRozgrywek[MAX_ROZGRYWKI];
	int liczbaRozgrywek;
} listaRozgrywek;

typedef enum { 	FIGURA_BRAK = 1,
		FIGURA_HETMAN_BIALY = 10,
		FIGURA_WIEZA_BIALY,
		FIGURA_SKOCZEK_BIALY,
		FIGURA_GONIEC_BIALY,
		FIGURA_PIONEK_BIALY,
		FIGURA_KROL_BIALY,
		FIGURA_HETMAN_CZARNY = 20,
		FIGURA_WIEZA_CZARNY,
		FIGURA_SKOCZEK_CZARNY,
		FIGURA_GONIEC_CZARNY,
		FIGURA_PIONEK_CZARNY,
		FIGURA_KROL_CZARNY } FIGURA_TYP;

typedef struct {
	long typ;
	Gracz zwyciezca;
	Gracz ruch;
	int kolejkaNumer;
	FIGURA_TYP plansza[SZACHOWNICA_ROZMIAR][SZACHOWNICA_ROZMIAR];
} Szachownica;

#endif
