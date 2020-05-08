Just [hojkas], trying to wrap my head around the task. Feel free to add stuff. Feel free to ignore this. Added for the slight chance it could be usefull.
Toto nemá co dělat s odevzdávkou.

http://www.fit.vutbr.cz/study/courses/ICP/public/ICP-PRJ-zadani.html.cs

[toc]

# Howto: ICP

snaha rozložit velkou neznámou kterou je projekt na podproblémy

by: hojkas the pepega

to: hojkas the pepega and Otterly the flying god

# Overview ideas

## GUI

Návrh přes QT, to vytvoří samo. Propojení s funkčností samotnou pomocí signálů a socketů...

### Přiblížení/posun věcí

Protože musíme řešit oddalování/přibližování, jednoduchý kód na přepočítání zobrazení? Bude chtít tlačítka plus/mínus + posuny do boku, řekněme 4 úrovně přiblížení (aka přepočítání zobrazení bodů)? Přidat vrstvu mezi vykreslením co to podle parametru přepočítá?

## Třídy

**!** Pokud se interaktivně něco mění (objížďka, dopravní situace)…

### Mapa

Seznam čar.

### Ulice

Informace: id, bod1, bod2, [název ulice], čas na projetí v sekundách. (uchováváme i ve file)

Multiplier (default 1). Interaktivně se dá měnit.

### Zastávka

není třeba jako objekt, we are lazy, zastávka == polovina ulice

### Linka

File: číslo linky, id ulice, zastávka bool

Objekt: čislo linky, oboustranně propojený seznam ulice a jestli je zde zastávka (seznam objekt: ulice + bool).

### Objížďky

Naklikání ulic k ostranění z linky, nový kousek seznamu, napojit, vymazat.

How:

* po dokončení objížky znovu načíst řád z file
* mít bool a jiný seznam pro objížďkový řád

### Ztížení provozu

Změna multiplieru, když je ulice se stejným jménem víckrát, klidně na všechny.

### Autobus

**!** Co opačný směr? (přinejhorším bool na otočení směru)

File (v node u linky): výjezdní čas, bool na směr.

Objekt: Odkaz na linku (třídu), výjezdní čas, aktuální x, aktuální y (pro výpis), odkaz na ulici, čas co už byl na ulici, bool na směr

**!** V každém tiku dopočítat aktuální x, y

### Timer

Tlačítka na zrychlení/nastavení času, vysílání signálu na aktualizaci

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
qDebug() << "shit happened" << value;
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