#!/usr/bin/env python3

import pokerlab

print(f"Porcentaje;Manos;Acumulado")
for i in range(0, 101):
    todos = pokerlab.rangeSet(range=f"{i}%")
    if(i < 1):
        delta = ""
    else:
        delta = pokerlab.rangeSet(action="subtract", range=f"{i-1}%")
    print(f"{i};{delta};{todos}")

