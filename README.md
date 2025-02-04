
# PWI-DRAGONTOWER: Projekt grupowy na studia

Prezentacja: https://www.youtube.com/watch?v=8hTzj1mkZc0&ab_channel=g42

**Dragon Tower** to popularna gra kasynowa, w której głównym celem jest dojście do szczytu wybierając odpowiednie pola. Gra została napisana w **C++** z wykorzystaniem biblioteki **Qt**.

---

## Spis treści
- [Wprowadzenie](#wprowadzenie)
- [Funkcjonalności](#funkcjonalności)
- [Wymagania systemowe](#wymagania-systemowe)
- [Uruchomienie](#uruchomienie)
- [Problemy i rozwiązania](#problemy-i-rozwiązania)
- [Autorzy](#autorzy)
- [Podział zadań](#podział-zadań)

---

## Wprowadzenie

**Dragon Tower** to gra oparta na losowości. W każdym rzędzie znajduje się od 1 do 3 ukrytych jajek. Wybór poprawnego okienka powoduje przemnożenie stawki zakładu przez odwrotność prawdopodobieństwa sukcesu. Jednak uwaga – wybór złego okienka skutkuje utratą całego zakładu.

Gracz ma możliwość zakończenia gry w dowolnym momencie i wypłaty aktualnej wygranej. Warto jednak pamiętać, że kasyno pobiera **2% prowizji** od każdej gry.

---

## Funkcjonalności

- **Ustawienie depozytu**: Jednorazowe okno dialogowe pytające przed grą o ustawienie stanu depozytu.
- **Ustawianie stawki zakładu**: Pole pozwalające graczowi ustawić wysokość zakładu.
- **Ustawianie liczby jajek**: Pole umożliwiające określenie liczby jajek w rzędzie.
- **Rozpoczynanie nowej gry**: Przycisk "New Game" inicjuje nową rozgrywkę.
- **Wypłata wygranej**: Przycisk "Cashout" pozwala na wypłatę aktualnej wygranej, jednocześnie kończąc grę.
- **Pola wyboru w grze**: Pola, które podświetlają się na kolor różowy, pozwalają graczowi wybrać odpowiednie jajko.

---

## Wymagania systemowe

### System operacyjny
- **Windows** lub **Linux**

### Dla Linuxa
Aby uruchomić grę, należy zainstalować następujące pakiety:
- `g++`
- `make`
- `qtbase5-dev`
- `qtchooser`
- `qt5-qmake`
- `libqt5charts5-dev`

Instalację można przeprowadzić w systemie Ubuntu za pomocą polecenia:
```bash
sudo apt install g++ make qtbase5-dev qtchooser qt5-qmake libqt5charts5-dev
```

---

## Uruchomienie

### Windows
1. Otwórz folder `WIN`.
2. Uruchom plik **DragonTower.exe**.

### Linux
1. Zainstaluj wymagane pakiety (zgodnie z listą w sekcji [Wymagania systemowe](#wymagania-systemowe)).
2. W katalogu projektu uruchom skrypt **build.sh** za pomocą polecenia:
   ```bash
   bash build.sh
   ```
3. Po zakończeniu kompilacji zostanie utworzony plik wykonywalny **DragonTower**, który można uruchomić w terminalu.

---

## Problemy i rozwiązania

### Problem: Brak bibliotek Qt na systemie Linux
**Rozwiązanie**: Upewnij się, że wszystkie pakiety wymienione w sekcji [Wymagania systemowe](#wymagania-systemowe) zostały poprawnie zainstalowane.

---

## Autorzy

Projekt został zrealizowany przez:
- **Wiktor Lewicki**
- **Illia Fursov**
- **Milena Oberzig**
- **Olivia Szymanowska**
- **Kyrylo Kalivanov**

---

## Podział zadań
**Wiktor Lewicki** - Zarządzał projektem i podziałem zadań, zaprogramował wszystko co jest w QT, zadbał o multiplatformowość.

**Illia Fursov**, **Kyrylo Kalivanov** - Stworzyli i napisali w większości gameplay.h i gameplay.cpp

**Milena Oberzig** i **Olivia Szymanowska** - Zrobiły piękne grafiki które zostały wykorzystane w projekcie, testowały działanie programu.

---
