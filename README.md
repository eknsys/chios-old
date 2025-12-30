# eknsys - ChiOS

ChiOS ist ein kleines Hobby-Betriebssystem.
Der Name "ChiOS" ist inspiriert von der Mangareihe *Chi's sweet home* und außer einer ASCII-Katze die im System Nachrichten ausgibt, die nichts mit der Mangareihe zutun hat, besteht *keine* Verbindung zum Originalwerk.

## Herkunft des Codes

Der gesamte Code ist eine Eigenleistung und enthält keine fremden Libraries oder übernommenen Ausschnitte.  
Zur Unterstützung beim Schreiben des Codes und der Makefile-Datei wurde die Microsoft Copilot KI eingesetzt.

## Abhängigkeiten

ChiOS benötigt folgende externe Tools für den Build-Prozess:

- [GNU GRUB](https://www.gnu.org/software/grub/) (GPLv3)
- [xorriso](https://www.gnu.org/software/xorriso/) (GPLv3)
- [GNU Compiler Collection (gcc)](https://gcc.gnu.org/) (GPL)
- [GNU Linker (ld)](https://www.gnu.org/software/binutils/) (GPL)

Diese Tools werden ausschließlich zum Erstellen von Boot-Images und zum Kompilieren genutzt.  
ChiOS selbst steht unter der MIT-Lizenz und ist unabhängig von den Lizenzen der genannten Tools.