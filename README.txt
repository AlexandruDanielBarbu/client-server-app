# Tema 2 - PCOM
Barbu Alexandru Daniel 324CC

## Descirere proiect

Proiectul / tema / assignmentul este o aplicatie client server.
Mai multi clienti UDP se pot conecta la un server si pot trimite mesaje la niste topicuri, la care se pot abona mai multi clienti TCP.

## Mod de rulare

> NOTE: Este nevoie de un client udp implementat pentru a testa la maxim aplicatia.

```bash
make # compileaza tot
make run_server # porneste serverul pe portul 4040
make run_subscriber # porneste clientul cu id "C1" pe adresa ip 127.0.0.1 pe portul 4040 al serverului
```

## Comenzi posibile
### Server

Pe server se poate da numai `exit`. Orice alt string sau capitalizare gresita a lui exit este ignorata.
``` bash
exit
```

### Client
```bash
subscribe ana/are/mere          # se va face subscribe la topicul ana/are/mere
unsubscribe ana/are/pere        # se va face unsubscribe la topicul ana/are/pere
exit                            # se va face exit 
```

## Detalii de implementare
### De ce epoll si nu poll

Sincer sa fiu am pornit din greseala cu epoll, eram familiar de la SO cu termenul si apucandu-ma tarziu de tema
am crezut ca la lab am facut epoll asa ca am mers mai departe cu epoll, dar din ce am inteles din research pe net, epoll
este mai rapid decat poll sau select, facand selectia in O(1).

Articolul din care m-am documentat cum se foloseste epoll este [acesta](https://copyconstruct.medium.com/the-method-to-epolls-madness-d9d2d6378642).

### Cum transmit datele de la client la server si invers?

Folosind Alex Simple Protocol (ASP) :)))). Este o inventie proprie care transmite in format human readable datele, controland
ce se intampla cu o serie de flaguri.

### Cum si cand fac exit si in ce ordine

Clientul cand face exit transmite la server ca face exit si apoi pur si simplu iese din executie. Daca primeste un mesaj de la server
destinat lui unde se mentioneaza ca se poate iesi si clientul nu a iesit deja, va iesi. Nu exista motive ca un client sa mai astepte
sa primeasca un semnal de confirmare de la server sau sa nu iasa daca serverul spune nu, dar am zis sa las poate mai daug ceva.

Serverul da exit numai dupa ce inchide toti clientii, le da unsubscribe la toti de la topicuri si dealoca arborele de topicuri.

### Cum am facut faza cu wildcarduri?

Mi-am facut un arbore din topicurile la care dau subscribe, in nodurile frunza retin clientii care sunt abonati la acel topic.
Notificarea clientului o fac traversand arborele si dand notify la clientii din nodul frunza.

Cand dau de cuvantul meu sau + in parsarea topicului il iau de bun si merg mai departe, daca dau de * il iau de bun si merg mai departe
in parcurgerea grafului tinand cont ca vreau sa caut si restul cuvintelor din topic printre copii nodului *.

>NOTE: este posibil ca testele care imi ies din timp sa iasa din timp din cauza algoritmului pentru *.

### De ce numar mic pe listen?

Din ce am inteles numarul maxim de utilizatori pus in functia listen este o coada de clienti folosita doar daca am clienti in asteptare, nu limiteaza in vre-un fel cati clienti se pot conecta la server.

### Cati clienti se pot comecta la server?

Oricati! Periodic verific dimensiunea lui events_array si o dublez daca e nevoie.

### Output client

Se cerea <IP client udp>:<port client udp> .... Eu am implementat acest feature, dar l-am comentat pt a face checkerul sa mearga.

### De ce C++?

Voiam sa incerc ceva diferit si speram ca voi termina tema mai repede, dar nu a fost asa, cred ca mai bine faceam in c.
Ce am eu este un c 65% c++ restul.

### Makefile

Nu eu l-am facut, inteligenta artificiala m-a ajutat sa o fac. Spre apararea mea aveam un makefile primitiv facut,
dar nu era destul de generic pt asa multe fisiere. Singur m-am gandit in schimb sa generez fisierele obiect si binarele intermediare in foldere separate.

## Ce ar merge schimbar

Daca tot am facut in C++ cred ca as putea sa inglobez file descriptorii intr-o clasa ca atunci cand iese din scope si se dealoca de pe
stiva sa se inchida automat fd-ul.

Alocarile de buffere: Acum ca stiu mai multe as fi putut folosi la greu vector<char>data.resize sa fac memcpy memset etc apoi sa ma folosesc de data.data().

## Impresii despre tema

Abea astept sa o pun pe git, sa o mai finisez, sa o pun in cv etc. Pe scurt mi-a placut mai mult pe final cand am dat debug si chair am inteles ce se inatmpla e fapt.
Tema a fost grea dar e bine venita.