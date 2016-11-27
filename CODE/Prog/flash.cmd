avrdude.exe -p m168 -c ftbb -P ft0 -B 4800  -U hfuse:w:223:m -U lfuse:w:194:m -U lock:w:249:m
avrdude.exe -p m168 -c ftbb -P ft0 -U flash:w:../Debug/Clock.hex:a
pause