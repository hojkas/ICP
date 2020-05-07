Just [hojkas], trying to wrap my head around the task. Feel free to add stuff. Feel free to ignore this. Added for the slight chance it could be usefull.
Toto nemá co dělat s odevzdávkou.

# Howto: ICP

snaha rozložit velkou neznámou kterou je projekt na podproblémy

by: hojkas the pepega

to: hojkas the pepega

# Overview ideas

Možné rozdělení na moduly:

- General GUI (clock tick, update signals, setup, atd.)
- Map handler (load map, move map, zoom map)
- Bus handler (load track, draw current)

## GUI

Návrh přes QT, to vytvoří samo. Propojení s funkčností samotnou pomocí signálů a socketů...

### Mapa

Ze souboru nahrané body a vykreslené. Hádám že vše bude nějak v souřadnici.

### Přiblížení/posun věcí

Protože musíme řešit oddalování/přibližování, jednoduchý kód na přepočítání zobrazení? Bude chtít tlačítka plus/mínus + posuny do boku, řekněme 4 úrovně přiblížení (aka přepočítání zobrazení bodů)? Přidat vrstvu mezi vykreslením co to podle parametru přepočítá?

## Třídy

### Mapa

Seznam čar.

### Čára

Informace: bod1, bod2, [název ulice].

### Zastávka

Objekt. Informace: bod, bod2, [název], poloha k bod (%?).

Nebo jen objekt čáry a případě hodnota polohy? Sounds better.

### Linka

Objekt. Informace: Číslo, zastávka0, čas do další zastávky, zastávka1, ... zastávkaN.

> 45 Z1 2 Z2 3 Z3 1 Z4

Aka č. 45 jedoucí ze zastávky1 do Z4 přes Z2 a Z3, čísla mezi jsou minuty. Pouze orientační zápis.

### Autobus

Objekt. Informace: Linka které náleží, čas výjezdu, aktuální pozice (null není-li na trati), možná dopočítat čas odstranění z mapy.

## Filesystem

Formát? CVS? Json? Something else?

Co je potřeba uložit:

### Mapa

Aka body a název ulice mezi nimi.

### Linky

Číslo, zastávky (ty stylem název ulice? případně nějaký offset jak blízko je to k bodu kraje?) a čas na projetí?

### Jízdní řád

Linka (čas na projetí zde nebo u linky) a časy kdy vyjíždí spoj z které strany?

# Howto: Start

## Simple af GUI

Okno. Nějaké prostředí (layout) na vykreslení mapy. Možná nějaké záhlaví zápatí a zatím nic nedělající tlačítka co později použijeme na oddálení/přiblížení a další shity.

## Načtení a vykreslení mapy

Doporučuji vlastní modul mapy. Volaný na začátku pro vykreslení hádám.

### Načtení

Zpracování file (ať už jakéhokoliv).

Vytvoření objektů (seznam čar mapy, vytvoření objektů čar).

### Vykreslení

Projití seznamu čar bod po bodu, vytvoření objektů na mapě skrz to.

**!** Co měřítko? Zatím neřešit? Nebo už toto všechno řešit v relativním poměru? (Stačí asi v absolutní hodnotě, zbytek se poté dá upravit)



