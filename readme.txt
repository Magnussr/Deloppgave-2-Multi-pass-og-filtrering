L�sningen jeg valgte til og l�se PG6200 kontinuasjonseksamen del 2. var og lage en state machine hvor jeg kan skifte mellom filtere ved og trykke p� tast 1 for blur og tast 2 for grayscale og tast 3 for combo. Tast 0 tar bort filterne. 
 
Renderingen starter f�rst med og rendere kaninen til et frambuffer object ved � bruke phong shaderen som fulgte oppgaven.
Det siste som skjer er at det som er i framebufferen blir rendert til en quad som dekker skjermen via passthrough shader programmet. Dersom et filter er valgt blir det brukt en annen fram buffer som mellom lager for � rendere filter f�r det siste steget.

Blur og greyscale blir gjort i fragment shaderen. Derfor kan gjennbruke vertex passthrough shaderen p� alle filter shaderne.

Med blur s� gj�r man f�rst et pass med vertical s� et pass med horizontal bluring. Jeg fant en nett side hvor jeg kunne regne ut en kernal verdi som jeg kunne bruke til � regne ut vektingen i blur passene. 
http://dev.theomader.com/gaussian-kernel-calculator/
Dette fordi at jeg �nsket � ha smooth blur valgte jeg og bruke en kernel size p� 11.

Sort-hvit filteret eller greyscale gjorde jeg ved og plusse sammen r,g,b verdiene og delte de p� 3 for og f� gjennomsnits fargen for og s� sette denne verdien p� alle r,g,b verdiene for det punktet for og f� en gr� nyanse.

Det siste valgfritt filteret kalte jeg combo hvor jeg kombinerte blur og greyscalen ved og f�rst impletere greyscalen og s� bluren. Det er et multi-pass filter siden det blir rendert flere ganger. Med Greyscale og vertical blur og horizontal blur.   
