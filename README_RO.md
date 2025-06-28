# Client Server App
Barbu Alexandru Daniel

## Cuprins

- [Context](#context)
- [Descriere proiect](#descriere-proiect)
- [Ce am învățat](#ce-am-învățat-din-acest-proiect)
- [Mod de rulare](#mod-de-rulare)
- [Comenzi posibile](#comenzi-posibile)
  - [Server](#server)
  - [Client](#client)
- [Detalii de implementare](#detalii-de-implementare)
  - [De ce epoll și nu poll](#de-ce-epoll-și-nu-poll)
  - [Cum transmit datele de la client la server și invers?](#cum-transmit-datele-de-la-client-la-server-și-invers)
  - [Cum și când fac exit și în ce ordine](#cum-și-când-fac-exit-și-în-ce-ordine)
  - [Cum am făcut faza cu wildcarduri?](#cum-am-făcut-faza-cu-wildcarduri)
  - [De ce număr mic pe listen?](#de-ce-număr-mic-pe-listen)
  - [Câți clienți se pot conecta la server?](#câți-clienți-se-pot-conecta-la-server)
  - [Output client](#output-client)
  - [De ce C++?](#de-ce-c)
  - [Makefile](#makefile)
- [Ce am mai adăugat](#ce-am-mai-adăugat)
- [Ce ar merge schimbat](#ce-ar-merge-schimbat)
- [Impresii despre temă](#impresii-despre-temă)

---

## Context

Acest proiect a fost inițial dezvoltat ca temă pentru un curs universitar de rețele. A fost extins și îmbunătățit ulterior pentru a reflecta o arhitectură mai clară și o abordare orientată spre practică profesională.

---

## Descriere proiect

Proiectul este o aplicație client-server.

Mai mulți clienți UDP se pot conecta la un server și pot trimite mesaje la niște topicuri, la care se pot abona mai mulți clienți TCP.

---

## Ce am învățat din acest proiect

- Am dobândit experiență practică în lucrul cu **socket-uri** și în implementarea unei aplicații client-server robuste.
- Am consolidat cunoștințele de **programare orientată pe obiect** folosind **clase în C++**, structurând codul într-un mod clar și modular.
- Am aplicat în mod conștient principiile **RAII (Resource Acquisition Is Initialization)** pentru a gestiona eficient resursele în componente precum `Socket.h` și `Epoll.h`.
- Am învățat cum să **integrez cod C++ cu cod C**, ceea ce mi-a oferit o perspectivă mai clară asupra interoperabilității dintre cele două limbaje și mi-a extins capacitatea de a lucra cu biblioteci existente.
- M-am familiarizat cu concepte din STL, precum **`std::vector`**, utilizat pentru gestionarea dinamică a datelor.
- Am învățat să-mi **planific și structurez mai bine munca**, anticipând pașii necesari și ajustându-mi abordarea pentru a duce proiectul la bun sfârșit într-un mod eficient și sustenabil.

---

## Mod de rulare

> NOTE: Este nevoie de un client UDP implementat pentru a testa la maxim aplicația.

```bash
make # compilează tot
make run_server # pornește serverul pe portul 4040
make run_subscriber # pornește clientul cu id "C1" pe adresa IP 127.0.0.1 pe portul 4040 al serverului
```

## Comenzi posibile
### Server

Pe server se poate da numai `exit`. Orice alt string sau capitalizare greșită a lui `exit` este ignorată.

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
### De ce epoll și nu poll

Sincer să fiu, am pornit din greșeală cu `epoll`. Eram familiar cu termenul și, apucându-mă târziu de temă, am crezut că la laborator am făcut `epoll`, așa că am mers mai departe cu `epoll`. Din ce am înțeles din research pe net, `epoll` este mai rapid decât `poll` sau `select`, făcând selecția în `O(1)`.

Articolul din care m-am documentat cum se folosește `epoll` este [acesta](https://copyconstruct.medium.com/the-method-to-epolls-madness-d9d2d6378642).

### Cum transmit datele de la client la server și invers?

Folosind _**Alex Simple Protocol (ASP)**_ :)))) Este o invenție proprie care transmite în format "human-readable" datele, controlând ce se întâmplă cu o serie de flaguri.

### Cum și când fac exit și în ce ordine

Clientul, când face exit, transmite la server că face exit și apoi pur și simplu iese din execuție. Dacă primește un mesaj de la server destinat lui unde se menționează că se poate ieși și clientul nu a ieșit deja, va ieși. Nu există motive ca un client să mai aștepte să primească un semnal de confirmare de la server sau să nu iasă dacă serverul spune "nu", dar am zis să las, poate mai adaug ceva.

Serverul dă exit numai după ce închide toți clienții, le dă unsubscribe tuturor de la topicuri și dealocă arborele de topicuri.

### Cum am făcut faza cu wildcarduri?

> CONTEXT: Un client poate face subscribe sau unsubscribe și la topicuri precum `ana/*` care se potrivește pe topicuri care încep cu `ana/` și se termină sau sunt urmate de oricâte nivele. Același concept este valabil și pentru `+` care face în mare cam același lucru.

Mi-am făcut un arbore din topicurile la care dau subscribe, în nodurile frunză rețin clienții care sunt abonați la acel topic. Notificarea clientului o fac traversând arborele și dând notify la clienții din nodul frunză.

Când dau de cuvântul meu sau `+` în parsarea topicului, îl iau de bun și merg mai departe. Dacă dau de `*`, îl iau de bun și merg mai departe în parcurgerea grafului ținând cont că vreau să caut și restul cuvintelor din topic printre copiii nodului `*`.

> NOTE: Este posibil ca testele care îmi ies din timp să iasă din timp din cauza algoritmului pentru `*`.

### De ce număr mic pe listen?

Din ce am înțeles, numărul maxim de utilizatori pus în funcția `listen` este o coadă de clienți folosită doar dacă am clienți în așteptare, nu limitează în vreun fel câți clienți se pot conecta la server.

### Câți clienți se pot conecta la server?

Oricâți! Periodic verific dimensiunea lui `events_array` și o dublez dacă e nevoie.

### Output client

Se cerea `<IP client udp>:<port client udp> ....`. Eu am implementat acest feature, dar l-am comentat pentru a face checker-ul să meargă.

### De ce C++?

Voiam să învăț C++ și doream să arăt că mă descurc cu acest limbaj.

### Makefile

Nu eu l-am făcut, inteligența artificială m-a ajutat să o fac. Spre apărarea mea, aveam un makefile primitiv făcut, dar nu era destul de generic pentru atâtea fișiere. Singur m-am gândit, în schimb, să generez fișierele obiect și binarele intermediare în foldere separate.

## Ce am mai adăugat

Varianta originală a folosit o implementare naivă de sockets cu multe `goto`-uri și verificări ale apelurilor de sistem. Ca să unific procesul, mi-am făcut o clasă de Socket și Epoll ca atunci când ceva merge prost să dau `return` și să am socket-ii automat închiși.

## Ce ar merge schimbat

Alocările de buffere: Acum că știu mai multe, aș fi putut folosi la greu `vector<char>data.resize` să fac `memcpy`, `memset` etc. apoi să mă folosesc de `data.data()`.

## Impresii despre temă

Abia aștept să o pun pe Git, să o mai finisez, să o pun în CV etc. Pe scurt, mi-a plăcut mai mult pe final când am dat debug și chiar am înțeles ce se întâmplă de fapt. Tema a fost interesantă și a fost o provocare binevenită.