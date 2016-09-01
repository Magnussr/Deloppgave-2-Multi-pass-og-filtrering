Løsningen jeg valgte til og løse PG6200 kontinuasjonseksamen del 2. var og lage en state machine hvor jeg kan skifte mellom filtere ved og trykke på tast 1 for blur og tast 2 for grayscale og tast 3 for combo. Tast 0 tar bort filterne. 
 
Renderingen starter først med og rendere kaninen til et frambuffer object ved å bruke phong shaderen som fulgte oppgaven.
Det siste som skjer er at det som er i framebufferen blir rendert til en quad som dekker skjermen via passthrough shader programmet. Dersom et filter er valgt blir det brukt en annen fram buffer som mellom lager for å rendere filter før det siste steget.

Blur og greyscale blir gjort i fragment shaderen. Derfor kan gjennbruke vertex passthrough shaderen på alle filter shaderne.

Med blur så gjør man først et pass med vertical så et pass med horizontal bluring. Jeg fant en nett side hvor jeg kunne regne ut en kernal verdi som jeg kunne bruke til å regne ut vektingen i blur passene. 
http://dev.theomader.com/gaussian-kernel-calculator/
Dette fordi at jeg ønsket å ha smooth blur valgte jeg og bruke en kernel size på 11.

Sort-hvit filteret eller greyscale gjorde jeg ved og plusse sammen r,g,b verdiene og delte de på 3 for og få gjennomsnits fargen for og så sette denne verdien på alle r,g,b verdiene for det punktet for og få en grå nyanse.

Det siste valgfritt filteret kalte jeg combo hvor jeg kombinerte blur og greyscalen ved og først impletere greyscalen og så bluren. Det er et multi-pass filter siden det blir rendert flere ganger. Med Greyscale og vertical blur og horizontal blur.   
