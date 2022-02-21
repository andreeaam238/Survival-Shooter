Blaga Ana-Maria-Andreea
334CB

Cateva detalii legate de tema:
	- Desenare jucator - L-am realizat pe cel din gif-urile de pe OCW: am folosit 3 cercuri crem si 3 negre pentru a realiza conturul. 
	- Miscare si orientare jucator - Se misca folosind tastele WASD si se orienteaza dupa Mouse
	- Harta (desen harta, desen obstacole) - Harta este un patrat scalat, pentru obstacole mi-am creat vectori de dimensiuni si coordonate pe 
											 care i-am folosit pentru a-mi crea obiectele de tip Obstacol pe care le pastrez intr-un vector si
											 le randez pe Harta.
	- Camera - Camera urmareste constant Playerul, acesta aflandu-se in mod permanent in centrul ferestrei de vizualizare. Am folosit sfatul 
			   oferit pe Moodle pentru a nu deforma scena la resize-ul ferestrei de afisare: cand raportul intre width si height pentru spatiul 
			   logic difera dintre raportul intre width si height pentru fereastra de afisare, aplic o scalare uniforma la Transformarea 
			   fereastra-poarta (am folosit functia VisualizationTransf2DUnif(logicSpace, viewSpace) din laboratorul 3 care intoarce matricea 
			   transformarii fereastra poarta, cu scalare uniforma)
	Desenare inamici - I-am realizat pe cel din gif-urile de pe OCW: am folosit 3 patrate magenta si 3 negre pentru a realiza conturul. 
	Miscare si orientare inamici - Inamicii se misca cu o viteza constanta, urmarind pozitia actuala a Playerului.
	Proiectile (tragere si desenare) - Le-am realizat pe cele de pe OCW: patrate negre care pornesc din player si se indreapta conform unghiului 
									   dintre Player si Mouse la momentul respectiv
	Coliziuni - 
		jucator - inamic - Am folosit resursele aditionale de pe OCW pentru a implementa AABB collision intre Player (cerc) si Inamic (patrat)
		jucator - perete harta - Am impus playerului o limita de deplasare pe harta astfel incat acesta sa nu treaca cu ochii de marginea hartii
		jucator - obstacol - Verific daca o posibila mutare a Playerului ar determina coliziunea cu un Obstacol din vectorul de Obstacole si in 
							 caz afirmattiv nu execut miscarea respectiva
		proiectil - inamic - Am folosit resursele aditionale de pe OCW pentru a implementa AABB collision intre Glont/Proiectil (patrat) si 
							 Inamic (patrat). Pentru aceasta a trebuit sa fac trecerea de la Bottom Left Corner Refrence Point la Top Left 
							 Refrence Point, respectiv Bottom Right Reference Point.
		proiectil - perete harta - In functie de unghiul sub care se afla glontul/proiectilul si de coordonatele acestuia stabilesc daca el 
								   atinge un perete al hartii si il sterg din vectorul de gloante/proiectile in caz afirmativ
		proiectil - obstacol - In functie de unghiul sub care se afla glontul/proiectilul si de coordonatele acestuia stabilesc daca el atinge 
							   vreun obiect si il sterg din vectorul de gloante/proiectile in caz afirmativ
		inamic - perete harta - Inamicii sunt spawnati in afar ferestrei de vizualizare, dar in interiorul hartii. Acestia urmaresc Playerul, 
								deci aceasta coliziune este evitata si nu necesita tratare.
	Scor si healthbar (functionalitate si UI) - Scorul este afisat in command line, fiecare hit al unui inamic cu un glont/proiectil genereaza 
												100 de puncte pentru Player, iar in funtie de numarul de inamici care au reusit sa loveasca 
												Playerul updatez Health Barul din coltul dreaota sus.