Just [hojkas], trying to wrap my head around the task. Feel free to add stuff. Feel free to ignore this. Added for the slight chance it could be usefull.
Toto nemá co dělat s odevzdávkou.

[toc]

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

HOWTO implement:

* One way: Vlastní widget classa, event stuff.

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

# Maybe important code stuff

## Ladící výpisy

```cpp
#include <QDebug>
//sum code
qDebug << "shit happened" << value;
```

## Signály + Sloty

**What for**

Zpracování signálu z různých tlačíteka hejbátek co budou na interface.

Lze použít na timer, který každým tikem vyvolá signál na obnovu mapy. (`QTimer`)

### Chytání signálů

**Code**

```cpp
// example ze streamu
// mainwindow.h v MainWindow class
private slots:
	void onValueChange(int value);
// mainwindow.cpp v class MainWIndow creatoru (?)
connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onValueChange(int)))
// samostatna funkce v mainwindow.cpp
void MainWindow::onValueChange(int val)
{
    //what will happen
}
//onValueChange je naše jméno funkce, valueChanged je definovaný signál z daného prvku
```

### Přesměrování signálu

Jde taky napojit signál na signál.

```cpp
// .h v classe
signals:
	void valChanged(int val);
// .cpp v kodu
connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SIGNAL(valChanged(int)));
// aka to místo napojení na slot co to zpracuje prostě jen změní a pošle jiný signál
// kdybychom potom chtěli signál použít v té samé classe, slot ofc musí být definovaný viz kód nad tímto
connect(this, SIGNAL(valChanged(int)), this, SLOT(sumSlot(int)));
```

**!** aby signály bylo možné využívat, musí být v objektu, který dědí z QObject (např. MainWindow to splňuje) a zároveň musí mít v .h v definici classy na prvním řádku Q_OBJECT

```cpp
class MainWindow : public QMainWindow
{
    Q_OBJECT
    //sumShit
signals:
    //sumSignalDef
private slots:
    //sumSlotDef
}
```

### Vysílání signálu

```cpp
//např. v nějaké funkci/objektu/slotu
emit valChanged(value);
// signál valChanged musí být definovaný v .h (viz první Signál code)
```

## Eventy

Seznam eventů se dá nice najít v helpu pod danou classou (např. QWidget)

```cpp
//do .h do classy
//toto copy-paste z help
protected:
	virtual void paintEvent(QPaintEvent *event) override;
//v .cpp
#include <QPainter>
void MyWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawEllipse(sum_params);
    //btw event->pos() vrátí pozici eventu, could be usefull?
}
```

Když řešíme override, dobré je uvnitř acceptnout event zavolat tu bázovou třídu. (Mohou v ní být extra akce, které by se měly provést pokaždé or something. Accept aby už se nezpracovával, jen třeba aby se zaškrtlo tlačítko, atd. Don’t think about it, do it.)

```cpp
void MyWidget::mousePressEvent(QMouseEvent *event)
{
    //myNonInterestingStuff
    event->accept();
    QWidget::mousePressEvent(event);
}
```

## Vlastní widget

Vytvořit classu (header, cpp).

V Návrhu dát prostor pro Widget (pod Containers) . Kliknout pravým. “Promote to own something”, tam vyplnit jakou naši třídu tam chceme.

**!** Bacha na includes. Je potřeba toho pro widgety občas dost… `#include <QtWidgets>` většinu zahrnuje.