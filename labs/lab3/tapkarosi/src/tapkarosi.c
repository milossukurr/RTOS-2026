#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define MAX_TAPKAROS 10
#define TICKET_NUMBER 40
/***
 * Napraviti program koji simulira prodaju ulaznica pomocu programskih
 * niti. Sistem prodaje ulaznica se sastoji od maksimalno 10 tapkarosa
 * koji prodaju 40 karata. Kako bi simulacija prodaje bila sto 
 * uvjerljivija, tapkaros prije svake prodaje odspava odredjeni broj
 * sekundi (u rasponu od 2 do 9).
 * Pri svakoj prodaji ulaznice, tapkaros ispisuje informaciju o sebi, 
 * zatim, koliko je ukupno do sada prodao ulaznica i na kraju, koliko 
 * je ulaznica jos u prodaji ostalo
 * ***/

static sem_t semafor_karte;
static int preostalo_karata = TICKET_NUMBER;

int random_range(int lower, int upper) 
{ 
    /* funkcija izracunava slucajan broj u rasponu
     * od broja lower do broja upper.
     * */
    return (rand() % (upper - lower + 1)) + lower; 
} 

void* tapkaros(void *pParam)
{
    int broj_tapkarosa = (int)(size_t)pParam;
    int prodao_karata = 0;
    int spavanje = 0;

    while (1)
    {
        /* kod za tapkarosa unutar kojeg prije svake prodaje karte mora
	 * da odspava proizvoljan broj sekundi u rasponu od 2 do 9 dobijenu
	 * funkcijom random_range*/

        spavanje = random_range(2, 9);
        sleep(spavanje);

        sem_wait(&semafor_karte);

        if (preostalo_karata > 0)
        {
            preostalo_karata--;
            prodao_karata++;

            printf("Tapkaros %d: prodao ukupno %d ulaznica, ostalo jos %d u prodaji.\n", 
                   broj_tapkarosa, prodao_karata, preostalo_karata);
            fflush(stdout);

            sem_post(&semafor_karte);
        }
        else
        {
            sem_post(&semafor_karte);
            break;
        }
    }

    return 0;
}

int main (void)
{
    /* inicijalizuj semafor(e) 
	 * kreiraj niti
	 * uradi sve sto je potrebno sa nitima i semaforima u sredini
	 * uradi sve sto je potrebno na kraju
	 * */
    pthread_t niti[MAX_TAPKAROS];
    int i;

    srand(time(NULL));

    sem_init(&semafor_karte, 0, 1);

    for (i = 0; i < MAX_TAPKAROS; i++)
    {
        pthread_create(&niti[i], NULL, tapkaros, (void*)(size_t)(i + 1));
    }

    for (i = 0; i < MAX_TAPKAROS; i++)
    {
        pthread_join(niti[i], NULL);
    }

    sem_destroy(&semafor_karte);

    printf("Prodate su sve karte\n"); 
    return 0;
}
