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
#include <errno.h>

typedef struct {
	short sem_num;
	short sem_op;
	short sem_flg;
} sembuf;

void oposc(int grupa, short num) {
	struct sembuf s;
	s.sem_num = num;
	s.sem_op = -1;
	s.sem_flg = 0;
	if(semop(grupa, &s, 1) == -1)
		printf("Blad operacji P(S)\n");
}

void podnies(int grupa, short num) {
	struct sembuf s;
	s.sem_num = num;
	s.sem_op = 1;
	s.sem_flg = 0;
	if(semop(grupa, &s, 1) == -1)
		printf("Blad operacji V(S)\n");
}

Szachownica iSzachownice(Szachownica szachownica) {
	int kolumna;
	int wiersz;
	for(kolumna=0; kolumna<SZACHOWNICA_ROZMIAR; kolumna++) {
		for(wiersz=0; wiersz<SZACHOWNICA_ROZMIAR; wiersz++) {
			szachownica.plansza[kolumna][wiersz] = FIGURA_BRAK;
		}
	}
	for(kolumna=0; kolumna<SZACHOWNICA_ROZMIAR; kolumna++) {
		szachownica.plansza[kolumna][1] = FIGURA_PIONEK_BIALY;
		szachownica.plansza[kolumna][6] = FIGURA_PIONEK_CZARNY;
	}
	szachownica.plansza[0][0] = FIGURA_WIEZA_BIALY;
	szachownica.plansza[7][0] = FIGURA_WIEZA_BIALY;
	szachownica.plansza[0][7] = FIGURA_WIEZA_CZARNY;
	szachownica.plansza[7][7] = FIGURA_WIEZA_CZARNY;
	szachownica.plansza[1][0] = FIGURA_SKOCZEK_BIALY;
	szachownica.plansza[6][0] = FIGURA_SKOCZEK_BIALY;
	szachownica.plansza[1][7] = FIGURA_SKOCZEK_CZARNY;
	szachownica.plansza[6][7] = FIGURA_SKOCZEK_CZARNY;
	szachownica.plansza[2][0] = FIGURA_GONIEC_BIALY;
	szachownica.plansza[5][0] = FIGURA_GONIEC_BIALY;
	szachownica.plansza[2][7] = FIGURA_GONIEC_CZARNY;
	szachownica.plansza[5][7] = FIGURA_GONIEC_CZARNY;
	szachownica.plansza[4][0] = FIGURA_KROL_BIALY;
	szachownica.plansza[3][7] = FIGURA_KROL_CZARNY;
	szachownica.plansza[3][0] = FIGURA_HETMAN_BIALY;
	szachownica.plansza[4][7] = FIGURA_HETMAN_CZARNY;
	return szachownica;
}

int main(int argc, char *argv[])
{
	int nrKolejki;
	int nrPamieciLG;
	int nrPamieciLR;
	int sem;
	if((nrKolejki = msgget(IPC_PRIVATE, IPC_CREAT|0660)) == (int) -1) {
		printf("Blad tworzenia kolejki kom\n");
		return -1;
	}
	if((nrPamieciLG = shmget(IPC_PRIVATE, sizeof(listaGraczy), IPC_CREAT|0660)) == -1) {
		printf("Blad tworzenia pamieci dla listy graczy\n");
		return -1;
	}
	if((nrPamieciLR = shmget(IPC_PRIVATE, sizeof(listaRozgrywek), IPC_CREAT|0660)) == -1) {
		printf("Blad tworzenia pamieci dla listy rozgrywek\n");
		return -1;
	}
	if((sem = semget(IPC_PRIVATE, 2, IPC_CREAT|0660)) == -1) {
		printf("Blad tworzenia semaforow");
		return -1;
	}
	listaGraczy *LG;
	if((LG = shmat(nrPamieciLG, NULL, 0)) == (void*) -1)
		printf("przylaczenie LG niepowodzenie\n");
	LG->liczbaGraczy = 0;
	shmdt(LG);
	listaRozgrywek *LR;
	if((LR = shmat(nrPamieciLR, NULL, 0)) == (void*) -1)
		printf("przylaczenie LR niepowodzenie\n");
	LR->liczbaRozgrywek = 0;
	shmdt(LR);
	pid_t listaProc[4];
	semctl(sem, 0, SETVAL, (int) 1);
	semctl(sem, 1, SETVAL, (int) 1);
	if(nrKolejki == -1) {
		printf("Blad tworzenia kolejki\n");
	}
	else {
		printf("Serwerowa kolejka %d\n", nrKolejki);
		//Rozbicie programu na procesy odp. za nasluchiwanie
		listaProc[0] = fork();
		if(listaProc[0]>0) {
			printf("PID0: %d\n", listaProc[0]);
			//Matka
			listaProc[1] = fork();
			if(listaProc[1]>0) {
				printf("PID1: %d\n", listaProc[1]);
				//Matka
				listaProc[2] = fork();
				if(listaProc[2]>0) {
					printf("PID2: %d\n", listaProc[2]);
					//Matka
					listaProc[3] = fork();
					if(listaProc[3]>0) {
						printf("PID3: %d\n", listaProc[3]);
						//Matka
						bool koniec = false;
						char znakWyj = 'a';
						while(!koniec) {
							printf("Podaj znak Q aby wyjsc \n");
							scanf("%c", &znakWyj);
							if(znakWyj == 'Q')
								koniec = true;
						}
						printf("Czas ubijac\n");
						//Ubijanie procesow
						if(kill(listaProc[0], SIGTERM) == -1)
							printf("Blad ubijania %d\n", listaProc[0]);
						if(kill(listaProc[1], SIGTERM) == -1)
							printf("Blad ubijania %d\n", listaProc[1]);
						if(kill(listaProc[2], SIGTERM) == -1)
							printf("Blad ubijania %d\n", listaProc[2]);
						if(kill(listaProc[3], SIGTERM) == -1)
							printf("Blad ubijania %d\n", listaProc[3]);
						if(shmctl(nrPamieciLR, IPC_RMID, NULL) == -1)
							printf("Blad usuwania pamieci LR\n");
						if(shmctl(nrPamieciLG, IPC_RMID, NULL) == -1)
							printf("Blad usuwania pamieci LG\n");
						if(msgctl(nrKolejki, IPC_RMID, NULL) == -1)
							printf("Blad usuwania kolejki serwera\n");
					}
					else {//Potomek, sprawdza czy odpowiednie procesy zyja
						printf("Proces sprawdzacz dziala!\n");
						listaGraczy *lGracz;
						if((lGracz = shmat(nrPamieciLG, NULL, 0)) == (void*) -1)
							printf("Sprawdzacz: niepowodzenie doczepienia LG\n");
						listaRozgrywek *lRoz;
						if((lRoz = shmat(nrPamieciLR, NULL, 0)) == (void*) -1)
							printf("Sprawdzacz: niepowodzenie doczepienia LR\n");
						while(true) {
							int i=0;
							while(i<lGracz->liczbaGraczy) {
								oposc(sem, 0);
								if(kill(lGracz->listaGraczy[i].pid, 0) == -1) {
									if(i != lGracz->liczbaGraczy - 1) {
										lGracz->listaGraczy[i] = lGracz->listaGraczy[lGracz->liczbaGraczy];
									}
									lGracz->liczbaGraczy--;
								}
								else {
									i++;
								}
								podnies(sem, 0);
							}
							i=0;
							bool niepoprawna = false;
							Szachownica tempSzach;
							tempSzach.zwyciezca.id = -1;
							tempSzach.typ = SERWER_SZACHOWNICA;
							while(i<lRoz->liczbaRozgrywek) {
								//Sprawdzanie czy rozgrywka nie jest skonczona z przyczyn naturalnych
								oposc(sem, 1);
								if(kill(lRoz->listaRozgrywek[i].pid, 0) == -1) {
									niepoprawna = true;
								}
								if(!niepoprawna) {
									if(kill(lRoz->listaRozgrywek[i].graczPierwszy.pid, 0) == -1) {
										//Gracz 1 wyszedl z gry
										niepoprawna = true;
										tempSzach.zwyciezca = lRoz->listaRozgrywek[i].graczDrugi;
										msgsnd(lRoz->listaRozgrywek[i].graczDrugi.kolejkaNumer, &tempSzach, sizeof(Szachownica) - sizeof(long), 0);
										int j=0;
										for(j=0; j<lRoz->listaRozgrywek[i].liczbaObserwatorow; j++) {
											msgsnd(lRoz->listaRozgrywek[i].listaObserwatorow[j].kolejkaNumer, &tempSzach, sizeof(Szachownica) - sizeof(long), 0);
										}
									}
									if(kill(lRoz->listaRozgrywek[i].graczDrugi.pid, 0) == -1) {
										//Gracz drugi wyszedl z gry
										niepoprawna = true;
										tempSzach.zwyciezca = lRoz->listaRozgrywek[i].graczPierwszy;
										msgsnd(lRoz->listaRozgrywek[i].graczPierwszy.kolejkaNumer, &tempSzach, sizeof(Szachownica) - sizeof(long), 0);
										int j=0;
										for(j=0; j<lRoz->listaRozgrywek[i].liczbaObserwatorow; j++) {
											msgsnd(lRoz->listaRozgrywek[i].listaObserwatorow[j].kolejkaNumer, &tempSzach, sizeof(Szachownica) - sizeof(long), 0);
										}
									}
								}
								if(!niepoprawna) {//Sprawdzanie czy obserwatorzy nie uschli z nudow
									int j=0;
									while(j<lRoz->listaRozgrywek[i].liczbaObserwatorow) {
										bool zyje = true;
										if(kill(lRoz->listaRozgrywek[i].listaObserwatorow[j].pid, 0) == -1) {
											zyje = false;
											if(j != lRoz->listaRozgrywek[i].liczbaObserwatorow - 1) {
												lRoz->listaRozgrywek[i].listaObserwatorow[j] = lRoz->listaRozgrywek[i].listaObserwatorow[lRoz->listaRozgrywek[i].liczbaObserwatorow - 1];
											}
											lRoz->listaRozgrywek[i].liczbaObserwatorow--;
										}
										if(zyje)
											j++;	
									}
									i++;
								}
								else {
									if(i != lRoz->liczbaRozgrywek - 1) {
										lRoz->listaRozgrywek[i] = lRoz->listaRozgrywek[lRoz->liczbaRozgrywek - 1];
        	                                                        }
									lRoz->liczbaRozgrywek--;
								}
								podnies(sem, 1);
							}
						}
						printf("Proces sprawdzacz poza petla!\n");
					}
				}
				else {
					//Potomek komunikacja z graczem, lista graczy
					printf("Proces komunikacja z graczem dziala!\n");
					int uniqId = 1;
					Gracz graczLok;
					listaGraczy *lGraczLok;
					if((lGraczLok = shmat(nrPamieciLG, NULL, 0)) == (void*) -1)
						printf("KomGracz: niepowodzenie doczepienia LG\n");
					while(true) {
						msgrcv(nrKolejki, &graczLok, sizeof(Gracz) - sizeof(long), KLIENT_GRACZ, 0);
						printf("KomGracz: Odebrano gracza\n");
						bool moznaDodac = true;
						if(graczLok.id<0) {
							Preferencje *sprawdzany;
							if((sprawdzany = shmat(graczLok.pamiecNumer, NULL, 0)) == (void*) -1) {
								printf("KomGracz: Blad przypisywania pamieci sprawdzanego\n");
								moznaDodac = false;
							}
							else {
								Preferencje *przeciw;
								int i=0;
								oposc(sem, 0);
								for(i=0; i<lGraczLok->liczbaGraczy; i++)	{
									if((przeciw = shmat(lGraczLok->listaGraczy[i].pamiecNumer, NULL, 0)) != (void*) -1) {
										if(strcmp((*sprawdzany).pseudonim, (*przeciw).pseudonim) == 0)
											moznaDodac = false;
										shmdt(przeciw);
									}
									else
										printf("KomGracz: Blad dodawania pamieci z listy\n");
								}	
								podnies(sem, 0);	
								shmdt(sprawdzany);	
							}
						}
						if(!moznaDodac) { //Gracza nie mozna dodac
							graczLok.typ = SERWER_GRACZ;
							graczLok.id = -1;
							msgsnd(graczLok.kolejkaNumer, &graczLok, sizeof(Gracz) - sizeof(long), 0);
							printf("Wyslano komunikat do gracza, niemozna dodac\n");		
						}
						else {	//Gracza mozna dodac
							if(graczLok.id == -1) {
								graczLok.id = uniqId;
								uniqId++;
							}
							else {
								int j=0;
								oposc(sem, 0);
								for(j=0; j<lGraczLok->liczbaGraczy; j++) {
									if(graczLok.id == lGraczLok->listaGraczy[j].id && graczLok.zajety != lGraczLok->listaGraczy[j].zajety)
										lGraczLok->listaGraczy[j].zajety = graczLok.zajety;
								}
								podnies(sem, 0);
							}
							oposc(sem, 0);
							if(lGraczLok->liczbaGraczy < MAX_GRACZY) {
								lGraczLok->listaGraczy[lGraczLok->liczbaGraczy] = graczLok;
								lGraczLok->liczbaGraczy++;	
								graczLok.typ = SERWER_GRACZ;
								msgsnd(graczLok.kolejkaNumer, &graczLok, sizeof(Gracz) - sizeof(long), 0);
								printf("KomGracz: wyslano komunikat o dodaniu gracza\n");
							}
							else {	//Lista graczy jest pelna
								graczLok.typ = SERWER_GRACZ;
								graczLok.id = -1;
								msgsnd(graczLok.kolejkaNumer, &graczLok, sizeof(Gracz) - sizeof(long), 0);
								printf("KomGracz: nie mozna dodac gracza, przepelnienie listy\n");
							}
							podnies(sem, 0);
						}
						// Tworzenie listy przeciwnikow, o ile gracz zostal dodany
						if(graczLok.id != -1) {
							listaGraczy przeciwnicy;
							przeciwnicy.liczbaGraczy = 0;
							Preferencje *sprawdzany;
							Preferencje *przeciwnik;
							if((sprawdzany = shmat(graczLok.pamiecNumer, NULL, 0)) == (void*) -1)
								printf("KomGracz: niepowodzenie dodawania pamieci sprawdzany, tworzenie listy\n");
							int i=0;
							oposc(sem, 0);
							for(i=0; i<lGraczLok->liczbaGraczy; i++) {
								if((przeciwnik = shmat(lGraczLok->listaGraczy[i].pamiecNumer, NULL, 0)) == (void*) -1)
									printf("KomGracz: niepowodzenie przylaczania pamieci przeciwnika\n");
								if(sprawdzany->stopienZaawansowania == przeciwnik->stopienZaawansowania && sprawdzany->kolor != przeciwnik->kolor && !lGraczLok->listaGraczy[i].zajety && lGraczLok->listaGraczy[i].id != graczLok.id) {
									przeciwnicy.listaGraczy[przeciwnicy.liczbaGraczy] = lGraczLok->listaGraczy[i];
									przeciwnicy.liczbaGraczy++;
								}
								shmdt(przeciwnik);
								
							}
							podnies(sem, 0);
							shmdt(sprawdzany);
							przeciwnicy.typ = SERWER_LISTAGRACZY;
							msgsnd(graczLok.kolejkaNumer, &przeciwnicy, sizeof(listaGraczy) - sizeof(long), 0);
							printf("KomGracz: wyslano liste przeciwnikow\n");
						}	
					}
					printf("Proces komunikacji poza petla!\n");
				}
			}
			else {
				//Potomek, obsluga obserwatorow
				printf("Proces obslugi obserwatorow dziala!\n");
				Obserwator obserLok;
				listaRozgrywek *lRozgrywek;
				if((lRozgrywek = shmat(nrPamieciLR, NULL, 0)) == (void*) -1)
					printf("KomObs: blad przylaczania pamieci LR\n");
					
				while(true) {
					if(msgrcv(nrKolejki, &obserLok, sizeof(Obserwator) - sizeof(long), KLIENT_OBSERWATOR, 0) == -1)
						printf("Blad odbierania obserwatora\n");
					else
						printf("Obserwator kolejka: %d\n", obserLok.kolejkaNumer);
					listaRozgrywek lr;
					int x;
					for(x=0; x<MAX_ROZGRYWKI; x++) {
						lr.listaRozgrywek[x].id = -1;
						lr.listaRozgrywek[x].typ = -1;
						lr.listaRozgrywek[x].graczPierwszy.typ = -1;
						lr.listaRozgrywek[x].graczPierwszy.id = -1;
						lr.listaRozgrywek[x].graczPierwszy.kolejkaNumer = -1;
						lr.listaRozgrywek[x].graczPierwszy.pamiecNumer = -1;
						lr.listaRozgrywek[x].graczDrugi.typ = -1;
						lr.listaRozgrywek[x].graczDrugi.id = -1;
						lr.listaRozgrywek[x].graczDrugi.kolejkaNumer = -1;
						lr.listaRozgrywek[x].graczDrugi.pamiecNumer = -1;
						lr.listaRozgrywek[x].liczbaObserwatorow = 0;
						int xx;
						for(xx=0; xx<MAX_OBSERWATORZY; xx++) {
							lr.listaRozgrywek[x].listaObserwatorow[xx].typ = -1;
							lr.listaRozgrywek[x].listaObserwatorow[xx].idRozgrywka = -1;
							lr.listaRozgrywek[x].listaObserwatorow[xx].kolejkaNumer = -1;
						}
					}
					oposc(sem, 1);
					//Przepisywanie z lRozgrywki
					for(x=0; x<lRozgrywek->liczbaRozgrywek; x++) {
						lr.listaRozgrywek[x] = lRozgrywek->listaRozgrywek[x];
					}
					lr.liczbaRozgrywek = lRozgrywek->liczbaRozgrywek;
					podnies(sem, 1);
					lr.typ = SERWER_LISTAROZGRYWEK;
					if(msgsnd(obserLok.kolejkaNumer, &lr, sizeof(listaRozgrywek) - sizeof(long), 0) == -1) {
						perror("Dayum");
					}
					msgrcv(nrKolejki, &obserLok, sizeof(Obserwator) - sizeof(long), KLIENT_OBSERWATOR_ROZGRYWKA, 0);
					bool istnieje = false;
					int i=0;
					oposc(sem, 1);
					for(i=0; i<lRozgrywek->liczbaRozgrywek; i++) {
						if(lRozgrywek->listaRozgrywek[i].id == obserLok.idRozgrywka && kill(lRozgrywek->listaRozgrywek[i].pid, 0) != -1) {
							if(lRozgrywek->listaRozgrywek[i].liczbaObserwatorow < MAX_OBSERWATORZY) {
								istnieje = true;
								lRozgrywek->listaRozgrywek[i].listaObserwatorow[lRozgrywek->listaRozgrywek[i].liczbaObserwatorow] = obserLok;
								lRozgrywek->listaRozgrywek[i].liczbaObserwatorow++;
							}
							
						}
					}
					podnies(sem, 1);
					if(!istnieje) {
						obserLok.idRozgrywka = -1;
					}
					obserLok.typ = SERWER_OBSERWATOR;
					msgsnd(obserLok.kolejkaNumer, &obserLok, sizeof(Obserwator) - sizeof(long), 0);
				}
				printf("Proces obslugi obserwatorow poza petla!\n");
			}
		}
		else {
			//Potomek, tworzenie rozgrywek oraz procesow je obslugujących
			printf("Proces tworzacy rozgrywki dziala!\n");
			int uniqId = 1;
			listaGraczy *lGraczLok;
			listaRozgrywek *lRozLok;
			if((lGraczLok = shmat(nrPamieciLG, NULL, 0)) == (void*) -1)
				printf("Tworca: blad przylaczania pamieci LG\n");
			if((lRozLok = shmat(nrPamieciLR, NULL, 0)) == (void*) -1)
				printf("Tworca: blad przylaczania pamieci LR\n");
			Rozgrywka tempRoz;
			while(true) {
				msgrcv(nrKolejki, &tempRoz, sizeof(Rozgrywka) - sizeof(long), KLIENT_ROZGRYWKA, 0);
				printf("Tworca: odbieram chec stworzenia rozgrywki\n");
				bool graczeDostepni = true;
				int i=0;
				oposc(sem, 0);
				for(i=0; i<lGraczLok->liczbaGraczy; i++) {
					if(lGraczLok->listaGraczy[i].id == tempRoz.graczDrugi.id) {
						if(lGraczLok->listaGraczy[i].zajety) {
							graczeDostepni = false;
						}
					}
				}
				podnies(sem, 0);
				if(graczeDostepni) {
					//Jezeli gracze sa dostepni, trzeba zobaczyc czy jest miejsce na kolejna rozgrywke
					oposc(sem, 1);
					if(lRozLok->liczbaRozgrywek >= MAX_ROZGRYWKI) {
						graczeDostepni = false;
					}
					podnies(sem, 1);
				}
				tempRoz.typ = SERWER_ROZGRYWKA;
				//Wysylanie zalezne od sukcesu i porazki utworzenia rozgrywki
				//Przy sukcesie tworzymy nowy proces obslugi tej rozgrywki
				if(!graczeDostepni) {
					tempRoz.id = -1;
					msgsnd(tempRoz.graczPierwszy.kolejkaNumer, &tempRoz, sizeof(Rozgrywka) - sizeof(long), 0);
					printf("Tworca: porazka tworzenia, wysylam\n");
				}
				else {
					tempRoz.id = uniqId;
					uniqId++;
					msgsnd(tempRoz.graczPierwszy.kolejkaNumer, &tempRoz, sizeof(Rozgrywka) - sizeof(long), 0);
					msgsnd(tempRoz.graczDrugi.kolejkaNumer, &tempRoz, sizeof(Rozgrywka) -sizeof(long), 0);
					printf("Tworca: sukces tworzenia, wysylam obu\n");
					int pidRozgrywki = fork();
					if(pidRozgrywki) {
						//Matka, dodaje rozgrywke na liste
						tempRoz.pid = pidRozgrywki;
						tempRoz.liczbaObserwatorow = 0;
						oposc(sem, 1);
						lRozLok->listaRozgrywek[lRozLok->liczbaRozgrywek] = tempRoz;
						lRozLok->liczbaRozgrywek++;
						podnies(sem, 1);
					}
					else {
						printf("Rozgrywka zyje!\n");
						//Proces odpowiadajacy za rozgrywke
						bool graSieToczy = true;
						int nrKolejkiRozgrywka;
						int idRozgrywki = tempRoz.id;
						nrKolejkiRozgrywka = msgget(IPC_PRIVATE, IPC_CREAT|0660);
						Szachownica szachLok;
						Szachownica incomingSzachLok;
						szachLok.zwyciezca.id = -1;
						szachLok.kolejkaNumer = nrKolejkiRozgrywka;
						Gracz bylyRuch;
						Preferencje *prefLok;
						prefLok = shmat(tempRoz.graczPierwszy.pamiecNumer, NULL, 0);
						if(prefLok->kolor == KOLOR_BIALY) {
							szachLok.ruch = tempRoz.graczPierwszy;
						}
						else {
							szachLok.ruch = tempRoz.graczDrugi;
						}
						shmdt(prefLok);
						szachLok.typ = SERWER_SZACHOWNICA;
						listaRozgrywek *lista;
						if((lista = shmat(nrPamieciLR, NULL, 0)) == (void*) -1) {
							printf("Problem doczepiania pamieci dla rozgrywki");
							//Nie jestem co do tego pewien
							if(lRozLok != (void*) -1)
								lista = lRozLok;
						}
						szachLok = iSzachownice(szachLok);	
						msgsnd(szachLok.ruch.kolejkaNumer, &szachLok, sizeof(Szachownica) - sizeof(long), 0);
						bylyRuch = szachLok.ruch;
						while(graSieToczy) {
							//Petla przekazywania szachownicy oraz sprawdzania warunku wyjscia
							msgrcv(nrKolejkiRozgrywka, &incomingSzachLok, sizeof(Szachownica) - sizeof(long), KLIENT_SZACHOWNICA, 0);
							szachLok = incomingSzachLok;
							int i=0;
							for(i=0; i<lista->liczbaRozgrywek; i++) {
								if(lista->listaRozgrywek[i].id == idRozgrywki)
									tempRoz = lista->listaRozgrywek[i];
							}
							if(bylyRuch.id == szachLok.ruch.id) {
								if(szachLok.ruch.id == tempRoz.graczPierwszy.id) {
									szachLok.ruch = tempRoz.graczDrugi;
								}
								else {
									szachLok.ruch = tempRoz.graczPierwszy;
								}
							}
							bylyRuch = szachLok.ruch;
							szachLok.typ = SERWER_SZACHOWNICA;
							msgsnd(szachLok.ruch.kolejkaNumer, &szachLok, sizeof(Szachownica) - sizeof(long), 0);
							for(i=0; i<tempRoz.liczbaObserwatorow; i++) {
								msgsnd(tempRoz.listaObserwatorow[i].kolejkaNumer, &szachLok, sizeof(Szachownica) - sizeof(long), 0);
							}
							if(szachLok.zwyciezca.id != -1) {
								graSieToczy = false;
							}
						}
						printf("Proces rozgrywki %d sie zakonczyl\n", idRozgrywki);
						msgctl(nrKolejkiRozgrywka, IPC_RMID, NULL);
						return 0;
					}
				}
			}
			printf("Proces tworzacy rozgrywki poza petla!\n");
		}
	}
	return 0;
}
