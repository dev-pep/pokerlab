#!/usr/bin/env python3

# genera todas las estadísticas (win, lose, tie) de enfrentamientos entre
# ranges simples preflop (entradas únicas en la tabla preflop son unos
# rangos con 4, 6 o 12 combos: AKs, AKo, AA,...).

import pokerlab

def rangeVsRange(rangeH, rangeV):
    # suit combinations:
    combosPairs = ('cd', 'ch', 'cs', 'dh', 'ds', 'hs')
    combosSuited = ('cc', 'dd', 'hh', 'ss')
    combosOffsuit = ('cd', 'ch', 'cs', 'dc', 'dh', 'ds', 'hc', 'hd', 'hs', 'sc', 'sd', 'sh')
    win = lose = tie = 0
    # Veamos qué combos se deben considerar para cada range:
    if rangeH[0] == rangeH[1]:    # pocket pair
        combosH = combosPairs
    elif rangeH[2] == 'o':    # offsuit
        combosH = combosOffsuit
    else:    # suited
        combosH = combosSuited
    # Villano ahora:
    if rangeV[0] == rangeV[1]:    # pocket pair
        combosV = combosPairs
    elif rangeV[2] == 'o':    # offsuit
        combosV = combosOffsuit
    else:    # suited
        combosV = combosSuited
    # Empecemos el bucle:
    for h in combosH:
        for v in combosV:
            # Generemos las 4 cartas de los dos combos a enfrentar:
            h1 = rangeH[0] + h[0]
            h2 = rangeH[1] + h[1]
            v1 = rangeV[0] + v[0]
            v2 = rangeV[1] + v[1]
            # Ahora hay que comprobar que no haya cartas repetidas:
            cartas = set([h1, h2, v1, v2])
            if len(cartas) < 4:
                continue
            resul = pokerlab.pl_simHandVsHand(h1, h2, v1, v2)
            win += resul[0]
            lose += resul[1]
            tie += resul[2]
    return win, lose, tie

ranges = ('AA ', 'AKs', 'AKo', 'AQs', 'AQo', 'AJs', 'AJo', 'ATs', 'ATo', 'A9s', 'A9o', 'A8s', 'A8o',
          'A7s', 'A7o', 'A6s', 'A6o', 'A5s', 'A5o', 'A4s', 'A4o', 'A3s', 'A3o', 'A2s', 'A2o', 'KK ',
          'KQs', 'KQo', 'KJs', 'KJo', 'KTs', 'KTo', 'K9s', 'K9o', 'K8s', 'K8o', 'K7s', 'K7o', 'K6s',
          'K6o', 'K5s', 'K5o', 'K4s', 'K4o', 'K3s', 'K3o', 'K2s', 'K2o', 'QQ ', 'QJs', 'QJo', 'QTs',
          'QTo', 'Q9s', 'Q9o', 'Q8s', 'Q8o', 'Q7s', 'Q7o', 'Q6s', 'Q6o', 'Q5s', 'Q5o', 'Q4s', 'Q4o',
          'Q3s', 'Q3o', 'Q2s', 'Q2o', 'JJ ', 'JTs', 'JTo', 'J9s', 'J9o', 'J8s', 'J8o', 'J7s', 'J7o',
          'J6s', 'J6o', 'J5s', 'J5o', 'J4s', 'J4o', 'J3s', 'J3o', 'J2s', 'J2o', 'TT ', 'T9s', 'T9o',
          'T8s', 'T8o', 'T7s', 'T7o', 'T6s', 'T6o', 'T5s', 'T5o', 'T4s', 'T4o', 'T3s', 'T3o', 'T2s',
          'T2o', '99 ', '98s', '98o', '97s', '97o', '96s', '96o', '95s', '95o', '94s', '94o', '93s',
          '93o', '92s', '92o', '88 ', '87s', '87o', '86s', '86o', '85s', '85o', '84s', '84o', '83s',
          '83o', '82s', '82o', '77 ', '76s', '76o', '75s', '75o', '74s', '74o', '73s', '73o', '72s',
          '72o', '66 ', '65s', '65o', '64s', '64o', '63s', '63o', '62s', '62o', '55 ', '54s', '54o',
          '53s', '53o', '52s', '52o', '44 ', '43s', '43o', '42s', '42o', '33 ', '32s', '32o', '22 ')

print("Hero;Villain;Win;Lose;Tie")
for r1 in range(169):
    for r2 in range(r1 + 1, 169):
        resul = rangeVsRange(ranges[r1], ranges[r2])
        print(f"{ranges[r1].strip()};{ranges[r2].strip()};{resul[0]};{resul[1]};{resul[2]}")
        print(f"{ranges[r2].strip()};{ranges[r1].strip()};{resul[1]};{resul[0]};{resul[2]}")

