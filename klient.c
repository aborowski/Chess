#include <stdbool.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "protokol.h"
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>

void printSzachownica(Szachownica szachownica) {
        printf("-|AA|BB|CC|DD|EE|FF|GG||HH|\n---------------------------\n");
        int kolumna;
        int wiersz;
        for(wiersz=0; wiersz<8; wiersz++) {
                printf("%d|", wiersz+1);
                for(kolumna=0; kolumna<8; kolumna++) {
                        switch(szachownica.plansza[kolumna][wiersz]) {
                                case FIGURA_BRAK:
                                        printf("00|");
                                        break;
                                case FIGURA_HETMAN_BIALY:
                                        printf("BH|");
                                        break;
                                case FIGURA_WIEZA_BIALY:
                                        printf("BW|");
                                        break;
                                case FIGURA_SKOCZEK_BIALY:
                                        printf("BS|");
                                        break;
				case FIGURA_GONIEC_BIALY:
                                        printf("BG|");
                                        break;
                                case FIGURA_PIONEK_BIALY:
                                        printf("BP|");
                                        break;
                                case FIGURA_KROL_BIALY:
                                        printf("BK|");
                                        break;
                                case FIGURA_HETMAN_CZARNY:
                                        printf("CH|");
                                        break;
                                case FIGURA_WIEZA_CZARNY:
                                        printf("CW|");
                                        break;
                                case FIGURA_SKOCZEK_CZARNY:
                                        printf("CS|");
                                        break;
                                case FIGURA_GONIEC_CZARNY:
                                        printf("CG|");
                                        break;
                                case FIGURA_PIONEK_CZARNY:
                                        printf("CP|");
					break;
				case FIGURA_KROL_CZARNY:
                                        printf("CK|");
                                        break;
                        }
                }
                printf("\n---------------------------\n");
        }
}


bool fcjaRozgrywki(Gracz gracz, Rozgrywka rozgrywka) {
	Szachownica szachownica;
	Szachownica incomingSzachownica;
	bool krolZyje = false;
	Preferencje *preferencje;
	if((preferencje = shmat(gracz.pamiecNumer, NULL, 0)) == (void*) -1)
                        printf("Blad dolaczania wlasnej pamieci preferencji\n");
	msgrcv(gracz.kolejkaNumer, &incomingSzachownica, sizeof(Szachownica) - sizeof(long), SERWER_SZACHOWNICA, 0);
	szachownica = incomingSzachownica;
	if(szachownica.zwyciezca.id == -1) {//Nie zostal dotychczas rozstrzygniety zwyciezca
		printSzachownica(szachownica);
		printf("Wybierz pionka do ruchu, osobno kolumne (liczbowo 1-8) i wiersz(1-8)\n");
		int kol;
		int wie;
		bool dobryWybor = false;
		while(!dobryWybor) {
			printf("Kolumna:\n");
			scanf("%d", &kol);
			if(kol>0 && kol<9) {
				dobryWybor = true;
				kol--;
			}
		}
		dobryWybor = false;
		while(!dobryWybor) {
			printf("Wiersz:\n");
			scanf("%d", &wie);
			if(wie>0 && wie<9) {
				dobryWybor = true;
				wie--;
			}
		}
		printf("Wybierz komorke docelowa, taki sam sposob:\n");
		int kolD;
		int wieD;
		dobryWybor = false;
		while(!dobryWybor) {
			printf("Kolumna:\n");
			scanf("%d", &kolD);
			if(kolD>0 && kolD<9) {
				dobryWybor = true;
				kolD--;
			}
		}
		dobryWybor = false;
		while(!dobryWybor) {
			printf("Wiersz:\n");
			scanf("%d", &wieD);
			if(wieD>0 && wieD<<9) {
				dobryWybor = true;
				wieD--;
			}
		}
		//Przemieszczanie
		szachownica.plansza[kolD][wieD] = szachownica.plansza[kol][wie];
		szachownica.plansza[kol][wie] = FIGURA_BRAK;
		if(gracz.id == rozgrywka.graczPierwszy.id)
			szachownica.ruch = rozgrywka.graczDrugi;
		else
			szachownica.ruch = rozgrywka.graczPierwszy;
		szachownica.typ = KLIENT_SZACHOWNICA;
		//if(msgsnd(incomingSzachownica.kolejkaNumer, &szachownica, sizeof(Szachownica) - sizeof(long), 0) == -1) 
		//	perror("Dayum2");			
		int kolumna;
                int wiersz;
                for(kolumna=0; kolumna<SZACHOWNICA_ROZMIAR; kolumna++) {
                        for(wiersz=0; wiersz<SZACHOWNICA_ROZMIAR; wiersz++) {
                                if(preferencje->kolor == KOLOR_CZARNY) {
                                        if(szachownica.plansza[kolumna][wiersz] == FIGURA_KROL_BIALY)
                                                krolZyje = true;
                                }
                                else {
                                        if(szachownica.plansza[kolumna][wiersz] == FIGURA_KROL_CZARNY)
                                                krolZyje = true;
                                }
                        }
                }
		if(!krolZyje) {
                        printSzachownica(szachownica);
                        printf("Wygrales!\n");
                        if(rozgrywka.graczPierwszy.id == gracz.id)
                                szachownica.zwyciezca = rozgrywka.graczPierwszy;
                        else
                                szachownica.zwyciezca = rozgrywka.graczDrugi;
                        szachownica.typ = KLIENT_SZACHOWNICA;
                        if(msgsnd(incomingSzachownica.kolejkaNumer, &szachownica, sizeof(Szachownica) - sizeof(long), 0) == -1) {
                                perror("Dayum");
                        }
                        getchar();
                        shmdt(preferencje);
                        return true;
                }
		else {
			printSzachownica(szachownica);
			printf("Oczekiwanie na przeciwnika\n");
			if(msgsnd(incomingSzachownica.kolejkaNumer, &szachownica, sizeof(Szachownica) - sizeof(long), 0) == -1) 
			      perror("Dayum2");  
			shmdt(preferencje);
			return false;
		}


	}
	else {//Przegrana
		printSzachownica(szachownica);
		if(szachownica.zwyciezca.id == gracz.id)
			printf("Wygrales!\n");
		else
			printf("Przegrales!\n");
		getchar();
		shmdt(preferencje);
		return true;
	}
}

int main(int agrc, char* argv[]) {
	char wybor = 'x';
	bool koniec = false;
	int nrKolejkiSerwer;
	bool gra = false;
	Obserwator obserwator;
	Gracz gracz;
	Rozgrywka rozgrywka;
	Szachownica szachownica;
	printf("Podaj numer kolejki serwera\n");
	scanf("%d", &nrKolejkiSerwer);
	printf("Wybierz czy chcesz byc graczem czy obserwatorem ('g'/'o')\n");
	scanf("%c", &wybor);
	while(!koniec) {
		if(wybor == 'g') {
			if(!gra) {
				//Gracz, przygotowania
				bool komunikacjaTrwa = true;
				if((gracz.pamiecNumer = shmget(IPC_PRIVATE, sizeof(Preferencje), IPC_CREAT|0660)) == -1)
					printf("Blad przydzielania pamieci Preferencje\n");
				Preferencje *pref;
				if((pref = shmat(gracz.pamiecNumer, NULL, 0)) == (void*) -1)
					printf("Blad przylaczania pamieci Preferencje\n");
				if((gracz.kolejkaNumer = msgget(IPC_PRIVATE, IPC_CREAT|0660)) == -1)
					printf("Blad tworzenia kolejki komunikatow\n");
				gracz.id = -1;
				gracz.pid = getpid();
				printf("Podaj pseudonim gracza\n");
				scanf("%s", &pref->pseudonim[0]);
				printf("podaj kolor gracza, 0 bialy, liczba inna niz 0 czarny\n");
				int kolor;
				scanf("%d", &kolor);
				if(kolor == 0)
					pref->kolor = KOLOR_BIALY;
				else
					pref->kolor = KOLOR_CZARNY;
				printf("Podaj stopien zaawansowania; 1-3\n");
				int stopien;
				scanf("%d", &stopien);
				if(stopien<1)
					stopien = 1;
				if(stopien>3)
					stopien = 3;
				pref->stopienZaawansowania = stopien;
				//Dane uzupelnione
				gracz.zajety = true;
				bool chceWybierac = false;
				while(komunikacjaTrwa) {
					//Komunikacja przed stworzeniem rozgrywki
					gracz.typ = KLIENT_GRACZ;
					listaGraczy lGraczLok;
					if(msgsnd(nrKolejkiSerwer, &gracz, sizeof(Gracz) - sizeof(long), 0) == -1)
						printf("Niepowodzenie wysylania komunikatu do serwera, moze podany zostal zly numer kolejki?\n");
					printf("Po wysylaniu siebie do serwera\n");
					msgrcv(gracz.kolejkaNumer, &gracz, sizeof(Gracz) - sizeof(long), SERWER_GRACZ, 0);
					printf("Po odebraniu gracza z serwera\n");
					if(gracz.id == -1) {
						printf("Gracz juz istnieje na liscie, podaj inny pseudonim\n");
						scanf("%s", &pref->pseudonim[0]);
						continue;
					}
					else {
						//Gracz dostal ID, jedziem dalej
						if(msgrcv(gracz.kolejkaNumer, &lGraczLok, sizeof(listaGraczy) - sizeof(long), SERWER_LISTAGRACZY, 0) == -1)
							printf("Blad otrzymywania listy graczy\n");
						printf("Wybierz ID gracza, z ktorym chcesz grac, lub -1 jesli chcesz byc wybierany:\n");
						Preferencje *przeciwnik;
						int i;
						for(i=0; i<lGraczLok.liczbaGraczy; i++) {
							if((przeciwnik = shmat(lGraczLok.listaGraczy[i].pamiecNumer, NULL, 0)) == (void*) -1)
								printf("Blad przyczepiania pamieci przeciwnika\n");
							printf("%s, %d\n", przeciwnik->pseudonim, lGraczLok.listaGraczy[i].id);
							if(shmdt(przeciwnik) == -1)
								printf("Blad odczepiania pamieci przeciwnika\n");
						}
						bool dobryWybor = false;
						while(!dobryWybor) {
							scanf("%d", &i);
							if(i == -1) {
								dobryWybor = true;
								chceWybierac = false;
							}
							else {
								chceWybierac = true;
								int j;
								for(j=0; j<lGraczLok.liczbaGraczy; j++) {
									if(i == lGraczLok.listaGraczy[j].id)
										dobryWybor = true;
								}
							}
						}
						//Rozgalezienie dzialania zaleznie od tego czy wybieramy, czy jestesmy wybierani
						if(chceWybierac) {
							rozgrywka.id = -1;
							rozgrywka.graczPierwszy = gracz;
							rozgrywka.liczbaObserwatorow = 0;
							int j;
							for(j=0; j<lGraczLok.liczbaGraczy; j++) {
								if(lGraczLok.listaGraczy[j].id == i)
									rozgrywka.graczDrugi = lGraczLok.listaGraczy[j];
							}
							rozgrywka.typ = KLIENT_ROZGRYWKA;
							msgsnd(nrKolejkiSerwer, &rozgrywka, sizeof(Rozgrywka) - sizeof(long), 0);
							msgrcv(gracz.kolejkaNumer, &rozgrywka, sizeof(Rozgrywka) - sizeof(long), SERWER_ROZGRYWKA, 0);
							if(rozgrywka.id == -1)
								continue;
							else {
								komunikacjaTrwa = false;
								gra = true;
							}
						}
						else {//Chce byc wybierany
							gracz.zajety = false;
							gracz.typ = KLIENT_GRACZ;
							msgsnd(nrKolejkiSerwer, &gracz, sizeof(Gracz) - sizeof(long), 0);
							msgrcv(gracz.kolejkaNumer, &rozgrywka, sizeof(Rozgrywka) - sizeof(long), SERWER_ROZGRYWKA, 0);
							komunikacjaTrwa = false;
							gra = true;
						}
					}
				}
			}
			//Tutaj powinna byc wywolana funkcja petli szachownicy
			koniec = fcjaRozgrywki(gracz, rozgrywka);
			if(koniec) {
				//Usuwanie zasobow
				shmctl(gracz.pamiecNumer, IPC_RMID, NULL);
				msgctl(gracz.kolejkaNumer, IPC_RMID, NULL);
			}
		}
		else if(wybor == 'o') {
			//Obserwator, przygotowania
			if(!gra) {
				if((obserwator.kolejkaNumer = msgget(IPC_PRIVATE, IPC_CREAT|0660)) == -1)
					printf("Blad tworzenia kolejki obserwatora\n");
				printf("Kolejka Obs: %d\n", obserwator.kolejkaNumer);
				bool komunikacjaTrwa = true;
				obserwator.idRozgrywka = -1;
				obserwator.pid = getpid();
				obserwator.typ = KLIENT_OBSERWATOR;
				listaRozgrywek lRozLok;
				while(komunikacjaTrwa) {
					msgsnd(nrKolejkiSerwer, &obserwator, sizeof(Obserwator) - sizeof(long), 0);
					printf("Wyslano obserwatora\n");
					msgrcv(obserwator.kolejkaNumer, &lRozLok, sizeof(listaRozgrywek) - sizeof(long), SERWER_LISTAROZGRYWEK, 0);
					int i;
					Preferencje *p1, *p2;
					printf("Wybierz rozgrywke (wpisz id)\n");
					for(i=0; i<lRozLok.liczbaRozgrywek; i++) {
						if((p1 = shmat(lRozLok.listaRozgrywek[i].graczPierwszy.pamiecNumer, NULL, 0)) == (void*) -1)
							printf("Bla przylaczania preferencji gracza1\n");
						if((p2 = shmat(lRozLok.listaRozgrywek[i].graczDrugi.pamiecNumer, NULL, 0)) == (void*) -1)
							printf("Blad przylaczania preferencji gracza2\n");
						printf("%s vs. %s, %d\n", p1->pseudonim, p2->pseudonim, lRozLok.listaRozgrywek[i].id);
						shmdt(p1);
						shmdt(p2);
					}
					int numerek;
					bool dobryWybor = false;
					while(!dobryWybor) {
						printf("ID:\n");
						scanf("%d", &numerek);
						int j;
						for(j=0; j<lRozLok.liczbaRozgrywek; j++) {
							if(lRozLok.listaRozgrywek[j].id == numerek) {
								obserwator.idRozgrywka = numerek;
								dobryWybor = true;
							}
						}
					}
					//Wybrano id
					obserwator.typ = KLIENT_OBSERWATOR_ROZGRYWKA;
					msgsnd(nrKolejkiSerwer, &obserwator, sizeof(Obserwator) - sizeof(long), 0);
					msgrcv(obserwator.kolejkaNumer, &obserwator, sizeof(Obserwator) - sizeof(long), SERWER_OBSERWATOR, 0);
					if(obserwator.idRozgrywka != -1) {
						komunikacjaTrwa = false;
						gra = true;
					}
				}
			}
			//Poza czescia inicjalizujaca i komunikacyjna
			msgrcv(obserwator.kolejkaNumer, &szachownica, sizeof(Szachownica) - sizeof(long), SERWER_SZACHOWNICA, 0);
			printSzachownica(szachownica);
			Preferencje *p;
			if(szachownica.zwyciezca.id == -1) {
				printf("Teraz ruch:\n");
				p = shmat(szachownica.ruch.pamiecNumer, NULL, 0);
				printf("%s\n", p->pseudonim);
				shmdt(p);
			}
			else {
				printf("Koniec gry");
				koniec = true;
			}
			if(koniec) {
				msgctl(obserwator.kolejkaNumer, IPC_RMID, NULL);
			}

		}
		else {//Wybranie zlego znaku, powtorka
			printf("Wybierz czy chcesz byc graczem czy obserwatorem ('g'/'o')\n");
			scanf("%c", &wybor);
		}
	}
	return 0;
}
